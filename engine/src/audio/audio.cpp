#include "audio.h"
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include "asset/asset_manager.h"

namespace Enik {
namespace Audio {

static ma_engine engine;
static bool is_engine_initialized = false;

struct OverlappingSound {
	ma_sound sound;
	bool is_initialized = false;
};
// NOTE: when a sound ends, it is free again
// so for this to fill up, there needs to be more than 16
// overlapped sounds playing simultaneously
static OverlappingSound s_overlapping_sounds[16] = {};

void Init() {
	ma_result result = ma_engine_init(NULL, &engine);
	if (result != MA_SUCCESS) {
		EN_CORE_ERROR("Failed to initialize AudioEngine!");
	}
	ma_engine_set_volume(&engine, 0.5f);
	is_engine_initialized = true;
}
void Shutdown() {
	for (OverlappingSound& entry : s_overlapping_sounds) {
		if (entry.is_initialized) {
			ma_sound_uninit(&entry.sound);
			entry.is_initialized = false;
		}
	}
	ma_engine_uninit(&engine);
	is_engine_initialized = false;
}


Ref<SoundAsset> CreateSound(const char* filepath) {
	if (!is_engine_initialized) {
		return nullptr;
	}
	Ref<SoundAsset> sa = CreateRef<SoundAsset>();
	sa->sound = new ma_sound();
	ma_sound* sound = static_cast<ma_sound*>(sa->sound);
	ma_result result = ma_sound_init_from_file(&engine, filepath, 0, NULL, NULL, sound);

	if (result != MA_SUCCESS) {
		delete sound;
		return nullptr;
	}

	return sa;
}

void UninitSound(void* sound) {
	if (sound) {
		ma_sound* s = static_cast<ma_sound*>(sound);
		if (is_engine_initialized) {
			ma_sound_uninit(s);
		}
		delete s;
	}
}


void Play(AssetHandle sound_handle) {
	Ref<SoundAsset> sound_asset = AssetManager::GetAsset<SoundAsset>(sound_handle);
	ma_sound* sound = static_cast<ma_sound*>(sound_asset->sound);

	if (!ma_sound_is_playing(sound)) {
		ma_sound_start(sound);
		return;
	}

	// if the sound is already playing, we need to clone it
	for (OverlappingSound& entry : s_overlapping_sounds) {
		if (!entry.is_initialized || !ma_sound_is_playing(&entry.sound)) {
			if (entry.is_initialized) {
				ma_sound_uninit(&entry.sound);
				entry.is_initialized = false;
			}

			ma_result result = ma_sound_init_copy(&engine, sound, 0, nullptr, &entry.sound);
			if (result == MA_SUCCESS) {
				entry.is_initialized = true;
				ma_sound_start(&entry.sound);
				return;
			}
		}
	}
}

}
}