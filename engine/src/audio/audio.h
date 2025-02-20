#pragma once
#include <base.h>
#include "audio/sound.h"


namespace Enik {
namespace Audio {

void Init();
void Shutdown();

Ref<SoundAsset> CreateSound(const char* filepath);
void UninitSound(void* sound);

void Play(AssetHandle sound_handle);

}
}