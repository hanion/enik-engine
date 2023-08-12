#pragma once
#include "renderer/buffer.h"
#include <pch.h>

namespace Enik {

class OpenGLVertexBuffer : public VertexBuffer {
public:
	OpenGLVertexBuffer(uint32_t size);
	OpenGLVertexBuffer(float* vertices, uint32_t size);
	virtual ~OpenGLVertexBuffer();


	virtual void Bind() const override;
	virtual void Unbind() const override;

	virtual void SetData(const void* data, uint32_t size) override;

	virtual void SetLayout(const BufferLayout& layout) override { m_BufferLayout = layout; }
	virtual const BufferLayout& GetLayout() const override { return m_BufferLayout; }

private:
	uint32_t m_RendererID;
	BufferLayout m_BufferLayout;

};


class OpenGLIndexBuffer : public IndexBuffer {
public:
	OpenGLIndexBuffer(uint32_t* indices, uint32_t size);
	virtual ~OpenGLIndexBuffer();


	virtual void Bind() const override;
	virtual void Unbind() const override;

	virtual uint32_t GetCount() const override { return m_Count; }

private:
	uint32_t m_RendererID;
	uint32_t m_Count;

};


}