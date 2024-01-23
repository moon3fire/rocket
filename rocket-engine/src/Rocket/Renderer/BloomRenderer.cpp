#include "rcktpch.h"

#include "BloomRenderer.h"
#include "RenderCommand.h"

//openGL
#include <glad.h>

namespace Rocket {
	BloomFramebuffer::BloomFramebuffer() :m_rendererID(-1) {}

	BloomFramebuffer::~BloomFramebuffer() {
		destroy();
	}

	bool BloomFramebuffer::init(uint32_t viewportWidth, uint32_t viewportHeight, unsigned int mipChainLength) {

		glGenFramebuffers(1, &m_rendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);

		glm::vec2 mipSize((float)viewportWidth, (float)viewportHeight);
		glm::ivec2 mipIntSize((int)viewportWidth, (int)viewportHeight);

		for (unsigned int i = 0; i < mipChainLength; i++) {
			BloomMip mip{};

			mipSize *= 0.5f;
			mipIntSize /= 2;
			mip.size = mipSize;
			mip.intSize = mipIntSize;

			glGenTextures(1, &mip.texture);
			glBindTexture(GL_TEXTURE_2D, mip.texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F, mipIntSize.x, mipIntSize.y, 0, GL_RGB, GL_FLOAT, nullptr); // maybe not float texture format

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			m_mipChain.emplace_back(mip);
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_mipChain[0].texture, 0);
		unsigned int attachments[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, attachments);

		// check completion status
		int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			RCKT_CORE_ERROR("gbuffer FBO error, status: {}", status);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			return false;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		m_isActive = true;
		return true;
	}

	void BloomFramebuffer::destroy() {
		for (int i = 0; i < m_mipChain.size(); i++) {
			glDeleteTextures(1, &m_mipChain[i].texture);
			m_mipChain[i].texture = 0;
		}
		glDeleteFramebuffers(1, &m_rendererID);
		m_rendererID = 0;
		m_isActive = false;
	}

	void BloomFramebuffer::bind() {
		glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);
	}

	void BloomFramebuffer::unbind() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	BloomRenderer::BloomRenderer() {}

	BloomRenderer::~BloomRenderer() {
		destroy();
	}

	bool BloomRenderer::init(uint32_t viewportWidth, uint32_t viewportHeight) {

		configureQuad();

		m_srcViewportSize = glm::ivec2(viewportWidth, viewportHeight);
		m_srcViewportSizeFloat = glm::vec2((float)viewportWidth, (float)viewportHeight);

		// Combiner
		m_combineFramebuffer.init(viewportWidth, viewportHeight);

		// Framebuffer
		const unsigned int num_bloom_mips = 5; // 5 mip levels
		bool status = m_bloomFramebuffer.init(viewportWidth, viewportHeight, num_bloom_mips);
		if (!status) {
			RCKT_CORE_ERROR("Failed to initialize bloom FBO - cannot create bloom renderer!");
			return false;
		}

		// Shaders
		m_downsampleShader = Shader::create("assets/shaders/Downsample.glsl");
		m_upsampleShader = Shader::create("assets/shaders/Upsample.glsl");
		m_combineShader = Shader::create("assets/shaders/Combine.glsl");

		// Downsample
		m_downsampleShader->bind();
		m_downsampleShader->setInt("srcTexture", 0);
		m_downsampleShader->unbind();

		// Upsample
		m_upsampleShader->bind();
		m_upsampleShader->setInt("srcTexture", 0);
		m_upsampleShader->unbind();

		m_isActive = true;
		return true;
	}

	void BloomRenderer::destroy() {
		m_bloomFramebuffer.destroy();
		m_isActive = false;
	}

	void BloomRenderer::renderBloomTexture(uint32_t srcTexture, float filterRadius) {
		m_bloomFramebuffer.bind();
		//RenderCommand::setClearColor({ 0.f, 0.f, 0.f, 0.0f });
		//RenderCommand::clear();

		renderDownsamples(srcTexture);
		renderUpsamples(filterRadius);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// Restore viewport
		glViewport(0, 0, m_srcViewportSize.x, m_srcViewportSize.y);
	}

	uint32_t BloomRenderer::combineTextures(uint32_t srcTexture, uint32_t bloomTexture, float exposure)
	{
		m_combineFramebuffer.bind();
		//RenderCommand::setClearColor({ 0.0f, 0.0f, 0.0f, 0.0f });
		//RenderCommand::clear();

		m_combineShader->bind();
		m_combineShader->setInt("originalTexture", 0);
		m_combineShader->setInt("blurredTexture", 1);

		m_combineShader->setFloat("u_exposure", exposure);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, srcTexture);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, bloomTexture);

		// Render screen-filled quad of resolution of current mip
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		m_combineShader->unbind();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// Restore viewport
		//glViewport(0, 0, m_srcViewportSize.x, m_srcViewportSize.y);

		return m_combineFramebuffer.getTexture();
	}

	void BloomRenderer::onResize(uint32_t width, uint32_t height) {
		m_srcViewportSize.x = width;
		m_srcViewportSize.y = height;

		m_srcViewportSizeFloat.x = (float)width;
		m_srcViewportSizeFloat.y = (float)height;

		glViewport(0, 0, width, height);
	
		//m_bloomFramebuffer.destroy();
		//m_bloomFramebuffer.init(width, height, m_bloomFramebuffer.getMipChain().size());
	}

	void BloomRenderer::renderDownsamples(uint32_t srcTexture) {
		const std::vector<BloomMip>& mipChain = m_bloomFramebuffer.getMipChain();

		m_downsampleShader->bind();
		m_downsampleShader->setFloat2("srcResolution", m_srcViewportSizeFloat);

		// Bind srcTexture (HDR color buffer) as initial texture input
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, srcTexture);

		// Progressively downsample through the mip chain
		for (int i = 0; i < mipChain.size(); i++)
		{
			const BloomMip& mip = mipChain[i];
			glViewport(0, 0, mip.size.x, mip.size.y);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_2D, mip.texture, 0);

			// Render screen-filled quad of resolution of current mip
			glBindVertexArray(quadVAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);

			// Set current mip resolution as srcResolution for next iteration
			m_downsampleShader->setFloat2("srcResolution", mip.size);
			// Set current mip as texture input for next iteration
			glBindTexture(GL_TEXTURE_2D, mip.texture);
		}

		m_downsampleShader->unbind();
	}

	void BloomRenderer::renderUpsamples(float filterRadius) {
		const std::vector<BloomMip>& mipChain = m_bloomFramebuffer.getMipChain();

		m_upsampleShader->bind();
		m_upsampleShader->setFloat("filterRadius", filterRadius);

		// Enable additive blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		glBlendEquation(GL_FUNC_ADD);

		for (int i = mipChain.size() - 1; i > 0; i--)
		{
			const BloomMip& mip = mipChain[i];
			const BloomMip& nextMip = mipChain[i - 1];

			// Bind viewport and texture from where to read
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mip.texture);

			// Set framebuffer render target (we write to this texture)
			glViewport(0, 0, nextMip.size.x, nextMip.size.y);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_2D, nextMip.texture, 0);

			// Render screen-filled quad of resolution of current mip
			glBindVertexArray(quadVAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
		}

		// Disable additive blending
		//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); // Restore if this was default
		glDisable(GL_BLEND);

		m_upsampleShader->unbind();
	}

	void BloomRenderer::configureQuad()
	{
		float quadVertices[] = {
			// upper-left triangle
			-1.0f, -1.0f, 0.0f, 0.0f, // position, texcoord
			-1.0f,  1.0f, 0.0f, 1.0f,
			 1.0f,  1.0f, 1.0f, 1.0f,
			 // lower-right triangle
			 -1.0f, -1.0f, 0.0f, 0.0f,
			  1.0f,  1.0f, 1.0f, 1.0f,
			  1.0f, -1.0f, 1.0f, 0.0f
		};

		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);

		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

		// position attribute
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// texture coordinate
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glBindVertexArray(0);
	}

	CombineFramebuffer::CombineFramebuffer() :m_rendererID(-1) {}

	CombineFramebuffer::~CombineFramebuffer()
	{
		destroy();
	}

	bool CombineFramebuffer::init(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		glGenFramebuffers(1, &m_rendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);

		glGenTextures(1, &m_colorTexture);
		glBindTexture(GL_TEXTURE_2D, m_colorTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, viewportWidth, viewportHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTexture, 0);
		unsigned int attachments[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, attachments);

		int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			RCKT_CORE_ERROR("gbuffer FBO error, status: {}", status);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			return false;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		m_isActive = true;
		return true;
	}

	void CombineFramebuffer::destroy()
	{
		glDeleteTextures(1, &m_colorTexture);
		glDeleteFramebuffers(1, &m_rendererID);
		m_isActive = false;
	}

	void CombineFramebuffer::bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);
	}

	void CombineFramebuffer::unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

} // namespace Rocket