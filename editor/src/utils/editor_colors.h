#pragma once
#include <imgui/imgui.h>

namespace Enik {
namespace EditorColors {

constexpr ImVec4 alpha  = ImVec4(1.0f, 1.0f, 1.0f, 0.6f);
constexpr ImVec4 dim    = ImVec4(0.9f, 0.9f, 0.9f, 0.9f);

// script
constexpr ImVec4 orange = ImVec4(0.9f, 0.5f, 0.1f, 1.0f);
// prefab
constexpr ImVec4 blue   = ImVec4(0.3f, 0.6f, 0.9f, 1.0f);
constexpr ImVec4 blue_a = ImVec4(0.3f, 0.6f, 0.9f, 0.6f);
// scene
constexpr ImVec4 green  = ImVec4(0.3f, 0.9f, 0.3f, 1.0f);
// project.enik
constexpr ImVec4 yellow = ImVec4(0.9f, 0.9f, 0.0f, 1.0f);
// .png
constexpr ImVec4 cyan   = ImVec4(0.0f, 0.9f, 0.9f, 1.0f);

// text editor background
constexpr ImVec4 te_bg  = ImVec4(0.08f, 0.08f, 0.08f, 1.0f);
// .wav
constexpr ImVec4 teal   = ImVec4(0.2f, 0.8f, 0.6f, 1.0f);
// .anim
constexpr ImVec4 purple = ImVec4(0.75f, 0.5f, 1.0f, 1.0f);
// .ttf
constexpr ImVec4 pale_pink = ImVec4(1.0f, 0.75f, 0.75f, 1.0f);

constexpr ImVec4 magenta = ImVec4(0.75f, 0.3f, 0.7f, 1.0f);


constexpr ImVec4 project = yellow;
constexpr ImVec4 scene = green;
constexpr ImVec4 persistent = green;
constexpr ImVec4 prefab = blue;
constexpr ImVec4 prefab_child = blue_a;
constexpr ImVec4 image = cyan;
constexpr ImVec4 script = orange;
constexpr ImVec4 sound = teal;
constexpr ImVec4 anim = purple;
constexpr ImVec4 font = pale_pink;
constexpr ImVec4 rigidbody = magenta;


constexpr float color_dark  = 19.0f / 255.0f;
constexpr float color_light = 26.0f / 255.0f;
constexpr ImVec4 bg_dark  = ImVec4(color_dark, color_dark, color_dark, 1.0f);
constexpr ImVec4 bg_light = ImVec4(color_light, color_light, color_light, 1.0f);

}

namespace EditorVars {
constexpr ImVec2 EditorTabPadding = ImVec2(16,8);
constexpr ImVec2 PanelTabPadding  = ImVec2(8 ,4);
}


}
