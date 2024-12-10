#pragma once
#include <base.h>
#include <pch.h>
#include "asset/asset.h"
#include "core/buffer.h"

namespace Enik {

enum class ImageFormat {
	None = 0,
	R8, RGB8, RGBA8, RGBA32F
};

struct TextureSpecification {
	uint32_t Width = 1;
	uint32_t Height = 1;
	ImageFormat Format = ImageFormat::RGBA8;
	bool MagFilterLinear = true;
};

class Texture : public Asset {
   public:
	virtual ~Texture() = default;

	virtual const TextureSpecification& GetSpecification() const = 0;

	virtual uint32_t GetWidth() const = 0;
	virtual uint32_t GetHeight() const = 0;
	virtual uint32_t GetRendererID() const = 0;

	virtual void SetData(Buffer data) = 0;

	virtual void Bind(uint32_t slot = 0) const = 0;

	virtual bool operator==(const Texture& other) const = 0;
	virtual bool equals(const Texture& other) const = 0;
};

class Texture2D : public Texture {
public:
	static Ref<Texture2D> Create(const TextureSpecification& specification, Buffer data = Buffer());

	static AssetType GetStaticType() { return AssetType::Texture2D; }
	virtual AssetType GetType() const override { return GetStaticType(); }
};

}