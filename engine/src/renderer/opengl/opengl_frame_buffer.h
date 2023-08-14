#pragma once
#include "renderer/frame_buffer.h"

namespace Enik {

class OpenGLFrameBuffer : public FrameBuffer {
public:
	OpenGLFrameBuffer(const FrameBufferSpecification& spec);
	virtual ~OpenGLFrameBuffer();

	virtual void Bind()   override final;
	virtual void Unbind() override final;

	virtual uint32_t GetColorAttachmentRendererID() const override final { return m_ColorAttachment; }

	virtual const FrameBufferSpecification& GetSpecification() const override final { return m_Specification; }

	void Invalidate();

private:
	uint32_t m_RendererID;
	uint32_t m_ColorAttachment, m_DepthAttachment;
	FrameBufferSpecification m_Specification;


};

}