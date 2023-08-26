#include "opengl_buffer.h"

#include <glad/glad.h>

namespace Enik {

OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size) {
	EN_PROFILE_SCOPE;

	glCreateBuffers(1, &m_RendererID);
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
}

OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size) {
	EN_PROFILE_SCOPE;

	glCreateBuffers(1, &m_RendererID);
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

OpenGLVertexBuffer::~OpenGLVertexBuffer() {
	EN_PROFILE_SCOPE;

	glDeleteBuffers(1, &m_RendererID);
}

void OpenGLVertexBuffer::Bind() const {
	EN_PROFILE_SCOPE;

	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
}

void OpenGLVertexBuffer::Unbind() const {
	EN_PROFILE_SCOPE;

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OpenGLVertexBuffer::SetData(const void* data, uint32_t size) {
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count)
	: m_Count(count) {
	EN_PROFILE_SCOPE;

	glCreateBuffers(1, &m_RendererID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
}

OpenGLIndexBuffer::~OpenGLIndexBuffer() {
	EN_PROFILE_SCOPE;

	glDeleteBuffers(1, &m_RendererID);
}

void OpenGLIndexBuffer::Bind() const {
	EN_PROFILE_SCOPE;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
}

void OpenGLIndexBuffer::Unbind() const {
	EN_PROFILE_SCOPE;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

}