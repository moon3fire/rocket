#pragma once

#include "Rocket/Renderer/Framebuffer.h"

namespace Rocket {

	class OpenGLFramebuffer : public Framebuffer {
	public:
		OpenGLFramebuffer(const FramebufferSpecification& spec);
		virtual ~OpenGLFramebuffer();

		void invalidate();

		virtual void bind() override;
		virtual void unbind() override;

		virtual void resize(uint32_t width, uint32_t height) override;
		virtual int readPixel(uint32_t attachmentIndex, int x, int y) override;

		virtual void clearAttachment(uint32_t attachmentIndex, int value) override; // fills the specified attachment with specified value

		virtual uint32_t getColorAttachmentRendererID(uint32_t index = 0) const override {
			RCKT_CORE_ASSERT(index < m_colorAttachments.size(), "given color attachment index is greater than attachments size");
			return m_colorAttachments[index];
		}

		virtual const FramebufferSpecification& getSpecification() const override { return m_specification; }
	private:
		uint32_t m_rendererID = 0;
		FramebufferSpecification m_specification;

		std::vector<FramebufferTextureSpecification> m_colorAttachmentSpecifications;
		FramebufferTextureSpecification m_depthAttachmentSpecification = FramebufferTextureFormat::None;

		std::vector<uint32_t> m_colorAttachments;
		uint32_t m_depthAttachment = 0;
	};

} // namespace Rocket

