#include "opengl_texture.h"
#include <cstdint>
#include <pch.h>


namespace Enik {
static GLenum ImageFormatToGLDataFormat(ImageFormat format) {
	switch (format) {
		case ImageFormat::RGB8:  return GL_RGB;
		case ImageFormat::RGBA8: return GL_RGBA;
		case ImageFormat::R8:    return GL_RED;
		default: {
			EN_CORE_ASSERT(false);
			return 0;
		}
	}
}

static GLenum ImageFormatToGLInternalFormat(ImageFormat format) {
	switch (format) {
		case ImageFormat::RGB8:  return GL_RGB8;
		case ImageFormat::RGBA8: return GL_RGBA8;
		case ImageFormat::R8:    return GL_R8;
		default: {
			EN_CORE_ASSERT(false);
			return 0;
		}
	}
}

OpenGLTexture2D::OpenGLTexture2D(const TextureSpecification& specification, Buffer data)
	: m_Width(specification.Width), m_Height(specification.Height) {
	EN_PROFILE_SCOPE;

	m_InternalFormat = ImageFormatToGLInternalFormat(specification.Format);
	m_DataFormat = ImageFormatToGLDataFormat(specification.Format);

	glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
	glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

	glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, specification.MagFilterLinear ? GL_LINEAR : GL_NEAREST);
	glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

	if (data) {
		SetData(data);
	}
}

OpenGLTexture2D::~OpenGLTexture2D() {
	EN_PROFILE_SCOPE;

	glDeleteTextures(1, &m_RendererID);
}

uint32_t get_bpp(GLenum format) {
	switch (format) {
		case GL_RGBA: return 4;
		case GL_RGB: return 3;
		case GL_R: return 1;
		default: return 1;
	}
}

void OpenGLTexture2D::SetData(Buffer data) {
	EN_PROFILE_SCOPE;

#ifdef EN_ENABLE_ASSERTS
	uint32_t bpp = get_bpp(m_DataFormat);
	EN_CORE_ASSERT(data.Size == m_Width * m_Height * bpp, "Data must be entire texture!");
#endif
	glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data.Data);
}

void OpenGLTexture2D::Bind(uint32_t slot) const {
	EN_PROFILE_SCOPE;

	glBindTextureUnit(slot, m_RendererID);
}

}