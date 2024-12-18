#include "font_importer.h"
#include "renderer/font.h"
#include "stb_truetype/stb_truetype.h"
#include "stb_image/stb_image.h"
#include "renderer/texture.h"

namespace Enik {

const uint32_t code_point_of_first_char = 32;       // ASCII of ' '(Space)
const uint32_t chars_to_include_in_font_atlas = 95; // Number of characters to include
const uint32_t atlas_width = 1000;
const uint32_t atlas_height = 1000;
const float font_atlas_width = atlas_width;
const float font_atlas_height = atlas_height;
const float font_size = 200.0f;

Ref<Texture2D> CreateTexture2D(int width, int height, uint8_t* data) {
	uint8_t* rgba_data = new uint8_t[width * height * 4];

	for (int i = 0; i < width * height; i++) {
		uint8_t r = data[i];
		rgba_data[i * 4 + 0] = r;
		rgba_data[i * 4 + 1] = r;
		rgba_data[i * 4 + 2] = r;
		rgba_data[i * 4 + 3] = r;
	}

	TextureSpecification spec;
	spec.Width = width;
	spec.Height = height;
	spec.Format = ImageFormat::RGBA8;
	spec.MagFilterLinear = false;

	Buffer buffer;
	buffer.Data = rgba_data;
	buffer.Size = width * height * 4;

	Ref<Texture2D> texture = Texture2D::Create(spec, buffer);

	delete[] rgba_data;

	return texture;
}

Ref<FontAsset> FontImporter::ImportFont(AssetHandle handle, const AssetMetadata& metadata) {
	std::ifstream input_stream(metadata.FilePath.string().c_str(), std::ios::binary);
	input_stream.seekg(0, std::ios::end);
	auto&& fontFileSize = input_stream.tellg();
	input_stream.seekg(0, std::ios::beg);

	uint8_t* font_data_buffer = new uint8_t[fontFileSize];
	input_stream.read((char*)font_data_buffer, fontFileSize);


    stbtt_fontinfo font_info = {};
	if (!stbtt_InitFont(&font_info, font_data_buffer, 0)) {
		EN_CORE_ERROR("FontImporter::ImportFont - stbtt_InitFont() Failed!");
		return nullptr;
	}

	uint8_t* font_atlas_texture_data = new uint8_t[atlas_width * atlas_height];
	memset(font_atlas_texture_data, 1, atlas_width * atlas_height);

	stbtt_pack_context ctx;
	stbtt_PackBegin(
		&ctx,
		(unsigned char*)font_atlas_texture_data,
		atlas_width,
		atlas_height,
		0, 1, nullptr
	);

	if (ctx.pack_info == nullptr) {
		EN_CORE_ERROR("Failed to initialize font pack context!");
		return nullptr;
	}


	stbtt_packedchar packed_chars[chars_to_include_in_font_atlas];
	stbtt_aligned_quad aligned_quads[chars_to_include_in_font_atlas];

	stbtt_PackFontRange(
		&ctx,
		font_data_buffer,
		0,
		font_size,
		code_point_of_first_char,
		chars_to_include_in_font_atlas,
		packed_chars
	);

	stbtt_PackEnd(&ctx);

	for (uint32_t i = 0; i < chars_to_include_in_font_atlas; i++) {
		float unusedX, unusedY;

		stbtt_GetPackedQuad(
			packed_chars,
			atlas_width,
			atlas_height,
			i,
			&unusedX, &unusedY,
			&aligned_quads[i],
			0
		);
	}


	std::vector<Glyph> glyphs(chars_to_include_in_font_atlas);
	for (uint32_t i = 0; i < chars_to_include_in_font_atlas; i++) {
		const auto& pc = packed_chars[i];
		Glyph& g = glyphs[i];

		g.tex_coords[0] = {pc.x0 / font_atlas_width, pc.y0 / font_atlas_height};
		g.tex_coords[1] = {pc.x1 / font_atlas_width, pc.y0 / font_atlas_height};
		g.tex_coords[2] = {pc.x1 / font_atlas_width, pc.y1 / font_atlas_height};
		g.tex_coords[3] = {pc.x0 / font_atlas_width, pc.y1 / font_atlas_height};

		g.Size = glm::vec2(pc.x1 - pc.x0, pc.y1 - pc.y0);
		g.Advance = pc.xadvance;
// 		g.Bearing = glm::vec2(pc.xoff, pc.yoff);

		g.positions[0] = {pc.xoff           , pc.yoff};
		g.positions[1] = {pc.xoff + g.Size.x, pc.yoff};
		g.positions[2] = {pc.xoff + g.Size.x, pc.yoff + g.Size.y};
		g.positions[3] = {pc.xoff           , pc.yoff + g.Size.y};
	}


	Ref<Texture2D> atlas_texture = CreateTexture2D(atlas_width, atlas_height, font_atlas_texture_data);

	Ref<FontAsset> font_asset = CreateRef<FontAsset>();
	font_asset->AtlasTexture = atlas_texture;
	font_asset->Glyphs = std::move(glyphs);
	font_asset->TextHeight = static_cast<uint32_t>(font_size);

    delete[] font_data_buffer;

	return font_asset;
}

}
