#pragma once
#include <glad/glad.h>

#include "renderer/texture.h"

namespace Enik {

class OpenGLTexture2D : public Texture2D {
public:
	OpenGLTexture2D(const TextureSpecification& specification, Buffer data = Buffer());
	virtual ~OpenGLTexture2D();

	virtual void SetData(Buffer data) override final;

	virtual const TextureSpecification& GetSpecification() const override { return m_Specification; }

	virtual uint32_t GetWidth() const override { return m_Width; };
	virtual uint32_t GetHeight() const override { return m_Height; };
	virtual uint32_t GetRendererID() const override { return m_RendererID; };

	virtual void Bind(uint32_t slot = 0) const override;

	virtual bool operator==(const Texture& other) const override final {
		return (m_RendererID == ((OpenGLTexture2D&)other).m_RendererID);
	}

	virtual bool equals(const Texture& other) const override final {
		return (m_RendererID == ((OpenGLTexture2D&)other).m_RendererID);
	}

private:
	TextureSpecification m_Specification;

	uint32_t m_Width;
	uint32_t m_Height;
	uint32_t m_RendererID;

	GLenum m_InternalFormat = 0;
	GLenum m_DataFormat = 0;
};


}