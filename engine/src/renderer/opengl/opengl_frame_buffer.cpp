#include "opengl_frame_buffer.h"

#include <glad/glad.h>

namespace Enik {

static const uint32_t s_MaxFrameBufferSize = 8192;

namespace Utils {
	static GLenum TextureTarget(bool multisampled) {
		return (multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D);
	}

	static void CreateTextures(bool multisampled, uint32_t* outID, uint32_t count) {
		glCreateTextures(TextureTarget(multisampled), count, outID);
	}

	static void BindTexture(bool multisampled, uint32_t id) {
		glBindTexture(TextureTarget(multisampled), id);
	}

	static void AttachColorTexture(uint32_t id, int samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index) {
		bool multisampled = (samples > 1);
		if (multisampled) {
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
		}
		else {
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);
			
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);
	}

	static void AttachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height) {
		bool multisampled = (samples > 1);
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

		glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0);
	}

	
	static bool IsDepthFormat(FrameBufferTextureFromat format) {
		switch (format) {
			case FrameBufferTextureFromat::DEPTH24_STENCIL8: return true;
			default: break;
		}	
		return false;
	}
}


OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferSpecification& fbspec)
	: m_Specification(fbspec) {
	
	for (auto spec : fbspec.Attachments.Attachments) {
		if (not Utils::IsDepthFormat(spec.TextureFormat)) {
			m_ColorAttachmentSpecs.emplace_back(spec);
		}
		else {
			m_DepthAttachmentSpec = spec;
		}

	}

	
	Invalidate();
}

OpenGLFrameBuffer::~OpenGLFrameBuffer() {
	glDeleteFramebuffers(1, &m_RendererID);
	glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());

	glDeleteTextures(1, &m_DepthAttachment);
}

void OpenGLFrameBuffer::Invalidate() {

	if (m_RendererID) {
		glDeleteFramebuffers(1, &m_RendererID);
		glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
		glDeleteTextures(1, &m_DepthAttachment);

		m_ColorAttachments.clear();
		m_DepthAttachment = 0;
	}

	glCreateFramebuffers(1, &m_RendererID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

	bool mulitsample = m_Specification.Samles > 1;

	// Attachments 
	if (m_ColorAttachmentSpecs.size()) {
		m_ColorAttachments.resize(m_ColorAttachmentSpecs.size());
		Utils::CreateTextures(mulitsample, m_ColorAttachments.data(), m_ColorAttachments.size());

		for (size_t i = 0; i < m_ColorAttachments.size(); i++) {
			Utils::BindTexture(mulitsample, m_ColorAttachments[i]);

			switch (m_ColorAttachmentSpecs[i].TextureFormat) {
				case FrameBufferTextureFromat::RGBA8:
					Utils::AttachColorTexture(m_ColorAttachments[i], m_Specification.Samles, GL_RGBA8, GL_RGBA, m_Specification.Width, m_Specification.Height, i);
					break;
				case FrameBufferTextureFromat::RED_INTEGER:
					Utils::AttachColorTexture(m_ColorAttachments[i], m_Specification.Samles, GL_R32I, GL_RED_INTEGER, m_Specification.Width, m_Specification.Height, i);
					break;
				default:
					break;
			}
		}
	}

	if (m_DepthAttachmentSpec.TextureFormat != FrameBufferTextureFromat::None) {
		Utils::CreateTextures(mulitsample, &m_DepthAttachment, 1);
		Utils::BindTexture(mulitsample, m_DepthAttachment);
		switch (m_DepthAttachmentSpec.TextureFormat) {
			case FrameBufferTextureFromat::DEPTH24_STENCIL8:
				Utils::AttachDepthTexture(m_DepthAttachment, m_Specification.Samles, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_Specification.Width, m_Specification.Height);
				break;
			
			default:
				break;
		}
	}

	if (m_ColorAttachments.size() > 1) {
		EN_CORE_ASSERT(m_ColorAttachments.size() <= 4);
		GLenum buffers [4] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
		glDrawBuffers(m_ColorAttachments.size(), buffers);
	}
	else if (m_ColorAttachments.empty()) {
		glDrawBuffer(GL_NONE);
	}

	EN_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void OpenGLFrameBuffer::Bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
	glViewport(0, 0, m_Specification.Width, m_Specification.Height);
}
void OpenGLFrameBuffer::Unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLFrameBuffer::Resize(uint32_t width, uint32_t height) {
	if (width <= 0 || height <= 0 || width > s_MaxFrameBufferSize || height > s_MaxFrameBufferSize) {
		EN_CORE_WARN("Attempted to resize frame buffer to ({0}, {1})", width, height);
		return;
	}

	m_Specification.Width = width;
	m_Specification.Height = height;
	
	Invalidate();
}
}