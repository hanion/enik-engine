#include "editor_assets.h"
#include "asset/importer/texture_importer.h"

namespace Enik {

Ref<Texture2D> EditorAssets::Play;
Ref<Texture2D> EditorAssets::Stop;
Ref<Texture2D> EditorAssets::Pause;
Ref<Texture2D> EditorAssets::Step;
Ref<Texture2D> EditorAssets::Select;
Ref<Texture2D> EditorAssets::Move;
Ref<Texture2D> EditorAssets::Scale;
Ref<Texture2D> EditorAssets::Rotate;


Ref<Texture2D> Load(const std::string& path) {
	return TextureImporter::LoadTexture2D(EN_ASSETS_PATH(path));
}

void EditorAssets::LoadEditorAssets() {
	Play  = Load("icons/play_button.png");
	Stop  = Load("icons/stop_button.png");
	Pause = Load("icons/pause_button.png");
	Step  = Load("icons/step_button.png");

	Select = Load("icons/tool_select.png");
	Move   = Load("icons/tool_move.png");
	Scale  = Load("icons/tool_scale.png");
	Rotate = Load("icons/tool_rotate.png");
}

}
