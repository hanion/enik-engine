#pragma once

#include <base.h>

namespace Enik {

struct FrameBufferSpecification {
	uint32_t Width = 32;
	uint32_t Height = 32;
	bool SwapChainTarget = false;
};



class FrameBuffer {
public:
	virtual ~FrameBuffer();

	virtual void Bind() = 0;
	virtual void Unbind() = 0;

	virtual void Resize(uint32_t width, uint32_t height) = 0;

	virtual uint32_t GetColorAttachmentRendererID() const = 0;

	virtual const FrameBufferSpecification& GetSpecification() const = 0;

	static Ref<FrameBuffer> Create(const FrameBufferSpecification& spec);


};

}