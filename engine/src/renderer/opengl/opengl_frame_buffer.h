#pragma once
#include "renderer/frame_buffer.h"

namespace Enik {

class OpenGLFrameBuffer : public FrameBuffer {
public:
	OpenGLFrameBuffer(const FrameBufferSpecification& spec);
	virtual ~OpenGLFrameBuffer();

	virtual void Bind()   override final;
	virtual void Unbind() override final;

	virtual void Resize(uint32_t width, uint32_t height) override final;
	virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) override final;

	virtual void ClearAttachment(uint32_t attachmentIndex, int value) override final;


	virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override final { 
		EN_CORE_ASSERT(index < m_ColorAttachments.size());
		return m_ColorAttachments[index];
	}

	virtual const FrameBufferSpecification& GetSpecification() const override final { return m_Specification; }

	void Invalidate();

private:
	uint32_t m_RendererID;
	FrameBufferSpecification m_Specification;

	std::vector<FrameBufferTextureSpecification> m_ColorAttachmentSpecs;
	FrameBufferTextureSpecification m_DepthAttachmentSpec;

	std::vector<uint32_t> m_ColorAttachments;
	uint32_t m_DepthAttachment;

};

}