#include "rcktpch.h"
#include "OpenGLFramebuffer.h"

#include <glad.h>

namespace Rocket {

	static const uint32_t s_maxFramebufferSize = 8192;

	namespace Utils {

		static GLenum textureTarget(bool multisampled) {
			return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		}

		static void createTextures(bool multisampled, uint32_t* outID, uint32_t count) {
			glCreateTextures(textureTarget(multisampled), count, outID);
		}

		static void bindTexture(bool multisampled, uint32_t id) {
			glBindTexture(textureTarget(multisampled), id);
		}

		static void attachColorTexture(uint32_t id, int samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index) {
			glEnable(GL_MULTISAMPLE);

			bool multisampled = samples > 1;

			if (multisampled) {
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE); /*TODO: think about to set this true*/
			}
			else {
				if (internalFormat == GL_RGBA16F || internalFormat == GL_RGBA32F) /* TODO: convert this to function */
					glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_FLOAT, nullptr);
				else
					glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, textureTarget(multisampled), id, 0);
		}

		static void attachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height) {
			bool multisampled = samples > 1;

			if (multisampled) {
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
			}
			else {
				glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, textureTarget(multisampled), id, 0);
		}

		static bool isDepthFormat(FramebufferTextureFormat format) {
			switch (format) {
				case FramebufferTextureFormat::DEPTH24STENCIL8: return true;
			}

			return false;
		}

		static GLenum RocketFBTextureFormatToGL(FramebufferTextureFormat format) {
			switch (format) {
				case FramebufferTextureFormat::RGBA8:		return GL_RGBA8;
				case FramebufferTextureFormat::RGBA_16F:	return GL_RGBA16F;
				case FramebufferTextureFormat::RGBA_32F:	return GL_RGBA32F;
				case FramebufferTextureFormat::RED_INTEGER: return GL_RED_INTEGER;
			}

			RCKT_CORE_ASSERT(false, "Unknown framebuffer texture format");
			return 0;
		}

		static GLenum RocketFBTextureFormatToGLType(FramebufferTextureFormat format) {
			switch (format) {
				case FramebufferTextureFormat::RGBA8:			return GL_INT;
				case FramebufferTextureFormat::RGBA_16F:		return GL_FLOAT;
				case FramebufferTextureFormat::RGBA_32F:		return GL_FLOAT;
				case FramebufferTextureFormat::RED_INTEGER:		return GL_INT;
			}

			RCKT_CORE_ASSERT(false, "Unknown framebuffer texture format for getting the type!");
			return 0;
		}
	
	} // namespace Utils

	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec) :m_specification(spec) {
		for (auto format : m_specification.attachments.attachments) {
			if (!Utils::isDepthFormat(format.textureFormat))
				m_colorAttachmentSpecifications.emplace_back(format.textureFormat);
			else
				m_depthAttachmentSpecification = format.textureFormat;
		}
		
		invalidate();
	}

	OpenGLFramebuffer::~OpenGLFramebuffer() {
		glDeleteFramebuffers(1, &m_rendererID);
		glDeleteTextures(m_colorAttachments.size(), m_colorAttachments.data());
		glDeleteTextures(1, &m_depthAttachment);
	}

	void OpenGLFramebuffer::invalidate() {

		if (m_rendererID) {
			glDeleteFramebuffers(1, &m_rendererID);
			glDeleteTextures(m_colorAttachments.size(), m_colorAttachments.data());
			glDeleteTextures(1, &m_depthAttachment);

			m_colorAttachments.clear();
			m_depthAttachment = 0;
		}

		glCreateFramebuffers(1, &m_rendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);

		bool multisample = m_specification.samples > 1;

		//Attachments
		if (m_colorAttachmentSpecifications.size()) {

			m_colorAttachments.resize(m_colorAttachmentSpecifications.size());
			Utils::createTextures(multisample, m_colorAttachments.data(), m_colorAttachments.size());
			for (size_t i = 0; i < m_colorAttachments.size(); i++) {
				Utils::bindTexture(multisample, m_colorAttachments[i]);
				switch (m_colorAttachmentSpecifications[i].textureFormat) {
					case FramebufferTextureFormat::RGBA8:
						Utils::attachColorTexture(m_colorAttachments[i], m_specification.samples, GL_RGBA8, GL_RGBA, m_specification.width, m_specification.height, i);
						break;
					case FramebufferTextureFormat::RGBA_16F:
						Utils::attachColorTexture(m_colorAttachments[i], m_specification.samples, GL_RGBA16F, GL_RGBA, m_specification.width, m_specification.height, i);
						break;
					case FramebufferTextureFormat::RGBA_32F:
						Utils::attachColorTexture(m_colorAttachments[i], m_specification.samples, GL_RGBA32F, GL_RGBA, m_specification.width, m_specification.height, i);
						break;
					case FramebufferTextureFormat::RED_INTEGER:
						Utils::attachColorTexture(m_colorAttachments[i], m_specification.samples, GL_R32I, GL_RED_INTEGER, m_specification.width, m_specification.height, i);
						break;
				}
			}
		}

		if (m_depthAttachmentSpecification.textureFormat != FramebufferTextureFormat::None) {
			Utils::createTextures(multisample, &m_depthAttachment, 1);
			Utils::bindTexture(multisample, m_depthAttachment);
			switch (m_depthAttachmentSpecification.textureFormat) {
				case FramebufferTextureFormat::DEPTH24STENCIL8:
					Utils::attachDepthTexture(m_depthAttachment, m_specification.samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_specification.width, m_specification.height);
			}
		}

		if (m_colorAttachments.size() > 1) {
			RCKT_CORE_ASSERT(m_colorAttachments.size() <= 4, "Currently supported color attachments size is less than 5");
			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 , GL_COLOR_ATTACHMENT2 , GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(m_colorAttachments.size(), buffers);
		}
		else if (m_colorAttachments.empty()) {
			// only depth-pass
			glDrawBuffer(GL_NONE);
		}

		RCKT_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::bind() {
		glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);
		glViewport(0, 0, m_specification.width, m_specification.height);
	}

	void OpenGLFramebuffer::unbind() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::resize(uint32_t width, uint32_t height) {

		if (width == 0 || height == 0 || width > s_maxFramebufferSize || height > s_maxFramebufferSize) {
			RCKT_CORE_WARN("Attempted to resize framebuffer to {0}, {1}", width, height);
			return;
		}

		m_specification.width = width;
		m_specification.height = height;

		invalidate();
	}

	int OpenGLFramebuffer::readPixel(uint32_t attachmentIndex, int x, int y) {
		RCKT_CORE_ASSERT(attachmentIndex < m_colorAttachments.size(), "Given attachment index is higher than currently supported count of attachments");
		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
		int pixelData;
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);

		return pixelData;
	}

	void OpenGLFramebuffer::clearAttachment(uint32_t attachmentIndex, int value) {
		RCKT_CORE_ASSERT(attachmentIndex < m_colorAttachments.size(), "given attachment index is less than ");

		auto& spec = m_colorAttachmentSpecifications[attachmentIndex];
		glClearTexImage(m_colorAttachments[attachmentIndex], 0, Utils::RocketFBTextureFormatToGL(spec.textureFormat),
																Utils::RocketFBTextureFormatToGLType(spec.textureFormat), &value);
	}
	
} // namespace Rocket