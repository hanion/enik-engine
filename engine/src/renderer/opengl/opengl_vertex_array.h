#pragma once
#include "renderer/vertex_array.h"


namespace Enik {


class OpenGLVertexArray : public VertexArray {
public:
	OpenGLVertexArray();
	virtual ~OpenGLVertexArray();

	virtual void Bind() const override;
	virtual void Unbind() const override;

	virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertex_buffer) override;
	virtual void SetIndexBuffer (const Ref<IndexBuffer >& index_buffer ) override;

	virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const override { return m_VertexBuffers; }
	virtual const Ref<IndexBuffer>& GetIndexBuffer() const override { return m_IndexBuffer; }

private:
	uint32_t m_RendererID;
	std::vector<Ref<VertexBuffer>> m_VertexBuffers;
	Ref<IndexBuffer> m_IndexBuffer;

};



}