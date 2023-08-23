#pragma once

#include <base.h>

namespace Enik {

enum class FrameBufferTextureFormat {
	None = 0,
	RGBA8,
	RED_INTEGER,
	DEPTH24_STENCIL8,
	Depth = DEPTH24_STENCIL8
};

struct FrameBufferTextureSpecification {
	FrameBufferTextureSpecification() = default;
	FrameBufferTextureSpecification(FrameBufferTextureFormat format)
		: TextureFormat(format) { 	
	}

	FrameBufferTextureFormat TextureFormat = FrameBufferTextureFormat::None;
};

struct FrameBufferAttachmentSpecification {
	FrameBufferAttachmentSpecification() = default;
	FrameBufferAttachmentSpecification(std::initializer_list<FrameBufferTextureSpecification> attachments)
		: Attachments(attachments) {
	}

	std::vector<FrameBufferTextureSpecification> Attachments;
};




struct FrameBufferSpecification {
	uint32_t Width = 32;
	uint32_t Height = 32;

	FrameBufferAttachmentSpecification Attachments;

	uint32_t Samles = 1;
	bool SwapChainTarget = false;
};



class FrameBuffer {
public:
	virtual ~FrameBuffer() = default;

	virtual void Bind() = 0;
	virtual void Unbind() = 0;

	virtual void Resize(uint32_t width, uint32_t height) = 0;
	virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) = 0;

	virtual void ClearAttachment(uint32_t attachmentIndex, int value) = 0;

	virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const = 0;

	virtual const FrameBufferSpecification& GetSpecification() const = 0;

	static Ref<FrameBuffer> Create(const FrameBufferSpecification& spec);


};

}