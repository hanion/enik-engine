#pragma once
#include <memory>
#include "buffer.h"

namespace Enik {


class VertexArray {
public:
	virtual ~VertexArray() {}

	virtual void Bind() const = 0;
	virtual void Unbind() const = 0;

	virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertex_buffer) = 0;
	virtual void SetIndexBuffer (const Ref<IndexBuffer >& index_buffer ) = 0;

	virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const = 0;
	virtual const Ref<IndexBuffer>& GetIndexBuffer() const = 0;

	static Ref<VertexArray> Create();

};


}