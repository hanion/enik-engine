#pragma once
#include "renderer/texture.h"

namespace Enik {

class EditorAssets {
public:
	static void LoadEditorAssets();

public:
	static Ref<Texture2D> Play, Stop, Pause, Step;
	static Ref<Texture2D> Select, Move, Scale, Rotate;
	static Ref<Texture2D> Banner;
};

}

