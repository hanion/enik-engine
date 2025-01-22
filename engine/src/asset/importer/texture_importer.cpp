#include "texture_importer.h"
#include "base.h"
#include "core/log.h"
#include "project/project.h"
#include "renderer/texture.h"
#include <stb_image/stb_image.h>

namespace Enik {

Ref<Texture2D> TextureImporter::ImportTexture2D(AssetHandle handle, const AssetMetadata &metadata) {
	return LoadTexture2D(metadata.FilePath);
}

Ref<Texture2D> TextureImporter::LoadTexture2D(const std::filesystem::path& path) {
	EN_PROFILE_SCOPE;

	int width, height, channels;
	stbi_set_flip_vertically_on_load(1);
	Buffer data;

// 	EN_CORE_TRACE("ti lt - {}", path.string());
	std::string path_str = path.string();
	{
		EN_PROFILE_SECTION("stbi_load - TextureImporter::ImportTexture2D");
		data.Data = stbi_load(path_str.c_str(), &width, &height, &channels, 0);
	}

	if (data.Data == nullptr) {
		EN_CORE_ERROR("TextureImporter::ImportTexture2D - could not load from path: {}", path_str.c_str());
		return nullptr;
	}
	data.Size = width * height * channels;

	TextureSpecification spec;
	spec.Width = width;
	spec.Height = height;

	switch (channels) {
		case 3:
			spec.Format = ImageFormat::RGB8;
			break;
		case 4:
			spec.Format = ImageFormat::RGBA8;
			break;
	}
	Ref<Texture2D> texture = Texture2D::Create(spec, data);
	data.Release();
	return texture;
}


}
