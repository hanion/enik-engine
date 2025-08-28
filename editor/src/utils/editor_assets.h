#pragma once
#include "renderer/texture.h"

namespace Enik {

enum class NewProjectType {
	EXAMPLE_SQUAREUP,
	EXAMPLE_SNAKE,
	EMPTY
};


class EditorAssets {
public:
	static void LoadEditorAssets(const std::filesystem::path& engine_source_path);

public:
	static Ref<Texture2D> Play, Stop, Pause, Step;
	static Ref<Texture2D> Select, Move, Scale, Rotate;
	static Ref<Texture2D> Banner;

	static std::filesystem::path ProjectTemplate;
	static std::filesystem::path ExampleSnakeGame;
	static std::filesystem::path ExampleSquareUp;

};

}

