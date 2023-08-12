#include <pch.h>
#include "opengl_vertex_array.h"

#include <glad/glad.h>

namespace Enik {


static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type) {
	switch (type) {
		case ShaderDataType::Float:     return GL_FLOAT;
		case ShaderDataType::Float2:    return GL_FLOAT;
		case ShaderDataType::Float3:    return GL_FLOAT;
		case ShaderDataType::Float4:    return GL_FLOAT;
		case ShaderDataType::Mat3:      return GL_FLOAT;
		case ShaderDataType::Mat4:      return GL_FLOAT;
		case ShaderDataType::Int:       return GL_INT;
		case ShaderDataType::Int2:      return GL_INT;
		case ShaderDataType::Int3:      return GL_INT;
		case ShaderDataType::Int4:      return GL_INT;
		case ShaderDataType::Bool:      return GL_BOOL;
		default: break;
	}
	EN_CORE_ASSERT(false, "Unknown ShaderDataType!");
	return 0;
}





OpenGLVertexArray::OpenGLVertexArray() {
	EN_PROFILE_SCOPE;

	glCreateVertexArrays(1, &m_RendererID);
}

OpenGLVertexArray::~OpenGLVertexArray() {
	EN_PROFILE_SCOPE;

	glDeleteVertexArrays(1, &m_RendererID);
}



void OpenGLVertexArray::Bind() const {
	EN_PROFILE_SCOPE;

	glBindVertexArray(m_RendererID);
}


void OpenGLVertexArray::Unbind() const {
	EN_PROFILE_SCOPE;

	glBindVertexArray(0);
}


void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) {
	EN_PROFILE_SCOPE;

	EN_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");

	glBindVertexArray(m_RendererID);
	vertexBuffer->Bind();

	uint32_t index = 0;
	const BufferLayout& layout = vertexBuffer->GetLayout();
	for (const auto& element : layout) {
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(
			index, 
			element.GetComponentCount(), 
			ShaderDataTypeToOpenGLBaseType(element.Type), 
			element.Normalized ? GL_TRUE : GL_FALSE, 
			layout.GetStride(), 
			(const void*)element.Offset
			);
		index++;
	}
	
	m_VertexBuffers.push_back(vertexBuffer);
}

void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) {
	EN_PROFILE_SCOPE;

	glBindVertexArray(m_RendererID);
	indexBuffer->Bind();

	m_IndexBuffer = indexBuffer;
}



}