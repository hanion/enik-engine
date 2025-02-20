#pragma once
#include "imgui.h"
#include <typeindex>
#include "renderer/font.h"
#include "utils/editor_colors.h"
#include "asset/asset.h"
#include "audio/audio.h"
#include "scene/animation.h"
#include "renderer/texture.h"

namespace Enik {
namespace FileMetadata {

constexpr ImVec4 default_text = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

struct FileInfo {
	std::string extension;
	std::string placeholder;
	ImVec4 color;
};

static const std::unordered_map<std::type_index, FileInfo> FileInfoMap = {
	{typeid(Texture2D), {".png",    "[Texture2D]", EditorColors::image}},
	{typeid(SoundAsset),{".wav",    "[Sound]",     EditorColors::sound}},
	{typeid(Animation), {".anim",   "[Animation]", EditorColors::anim}},
	{typeid(FontAsset), {".ttf",    "[Font]",      EditorColors::font}},
// 	{typeid(Prefab),    {".prefab", "[Prefab]",    EditorColors::blue}},
// 	{typeid(Scene),     {".escn",   "[Scene]",     EditorColors::green}},
// 	{typeid(Project),   {".enik",   "[Project]",   EditorColors::yellow}},
};

static const std::unordered_map<std::string, ImVec4> ExtensionColorMap = {
	{".png",    EditorColors::image},
	{".wav",    EditorColors::sound},
	{".anim",   EditorColors::anim},
	{".ttf",    EditorColors::font},
	{".prefab", EditorColors::prefab},
	{".escn",   EditorColors::scene},
	{".enik",   EditorColors::project}
};

template <typename T>
std::string GetFileExtension() {
	auto it = FileInfoMap.find(typeid(T));
	return it != FileInfoMap.end() ? it->second.extension : "";
}
template <typename T>
std::string GetPlaceholder() {
	auto it = FileInfoMap.find(typeid(T));
	return it != FileInfoMap.end() ? it->second.placeholder : "";
}


template <typename T>
const ImVec4& GetColor() {
	auto it = FileInfoMap.find(typeid(T));
	if (it != FileInfoMap.end()) {
		return it->second.color;
	} else {
		return default_text;
	}
}

// NOTE: call `ImGui::PopStyleColor();` after calling this !
template <typename T>
void ColorFileText() {
	ImGui::PushStyleColor(ImGuiCol_Text, GetColor<T>());
}

inline ImVec4 GetColorFromExtension(const std::string& extension) {
	auto it = ExtensionColorMap.find(extension);
	return it != ExtensionColorMap.end() ? it->second : default_text;
}

inline void ColorTextByExtension(const std::string& extension) {
	ImVec4 color = GetColorFromExtension(extension);
	ImGui::PushStyleColor(ImGuiCol_Text, color);
}


}
}
