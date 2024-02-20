#pragma once
#include <base.h>
#include <pch.h>

namespace Enik {

class EN_API Texture {
   public:
	virtual ~Texture() = default;

	virtual uint32_t GetWidth() const = 0;
	virtual uint32_t GetHeight() const = 0;
	virtual uint32_t GetRendererID() const = 0;

	virtual void SetData(void* data, uint32_t size) = 0;

	virtual void Bind(uint32_t slot = 0) const = 0;

	virtual bool operator==(const Texture& other) const = 0;
	virtual bool equals(const Texture& other) const = 0;
};

class EN_API Texture2D : public Texture {
public:
	static Ref<Texture2D> Create(uint32_t width, uint32_t height);
	static Ref<Texture2D> Create(const std::string& path, bool mag_filter_linear = true);
};

}