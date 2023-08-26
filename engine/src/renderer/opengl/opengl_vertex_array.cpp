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


void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertex_buffer) {
	EN_PROFILE_SCOPE;

	EN_CORE_ASSERT(vertex_buffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");

	glBindVertexArray(m_RendererID);
	vertex_buffer->Bind();

	uint32_t index = 0;
	const BufferLayout& layout = vertex_buffer->GetLayout();
	for (const auto& element : layout) {
		switch (element.Type) {
			case ShaderDataType::Float:
			case ShaderDataType::Float2:
			case ShaderDataType::Float3:
			case ShaderDataType::Float4:
			case ShaderDataType::Mat3:
			case ShaderDataType::Mat4: {
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
				break;
			}
			case ShaderDataType::Int:
			case ShaderDataType::Int2:
			case ShaderDataType::Int3:
			case ShaderDataType::Int4:
			case ShaderDataType::Bool: {
				glEnableVertexAttribArray(index);
				glVertexAttribIPointer(
					index,
					element.GetComponentCount(),
					ShaderDataTypeToOpenGLBaseType(element.Type),
					layout.GetStride(),
					(const void*)element.Offset
					);
				index++;
				break;
			}

			default: break;
		}
	}

	m_VertexBuffers.push_back(vertex_buffer);
}

void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& index_buffer) {
	EN_PROFILE_SCOPE;

	glBindVertexArray(m_RendererID);
	index_buffer->Bind();

	m_IndexBuffer = index_buffer;
}



}