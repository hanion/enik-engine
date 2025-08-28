#include "editor_assets.h"
#include "project/project.h"
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
Ref<Texture2D> EditorAssets::Banner;

std::filesystem::path EditorAssets::ProjectTemplate;
std::filesystem::path EditorAssets::ExampleSnakeGame;
std::filesystem::path EditorAssets::ExampleSquareUp;


static inline Ref<Texture2D> Load(const std::string& path) {
	return TextureImporter::LoadTexture2D(Project::FindAssetPath(path));
}
void EditorAssets::LoadEditorAssets(const std::filesystem::path& engine_source_path) {
	std::filesystem::path assets = engine_source_path / "editor/assets";

	Play   = Load("icons/play_button.png");
	Stop   = Load("icons/stop_button.png");
	Pause  = Load("icons/pause_button.png");
	Step   = Load("icons/step_button.png");

	Select = Load("icons/tool_select.png");
	Move   = Load("icons/tool_move.png");
	Scale  = Load("icons/tool_scale.png");
	Rotate = Load("icons/tool_rotate.png");

	Banner = Load("icons/banner.png");

	std::filesystem::path examples = engine_source_path / "examples";
	ProjectTemplate  = engine_source_path / "project";
	ExampleSnakeGame = examples / "snake-game";
	ExampleSquareUp  = examples / "squareup";
}

}
