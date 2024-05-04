#define MINIAUDIO_IMPLEMENTATION
#include "audio.h"


namespace Enik {

Scope<Audio> Audio::s_Instance = CreateScope<Audio>();

void Audio::Init() {
	ma_result result = ma_engine_init(NULL, &s_Instance->m_AudioEngine);
	if (result != MA_SUCCESS) {
		EN_CORE_ERROR("Failed to initialize AudioEngine!");
	}
}

void Audio::Play(const char* filepath) {
	ma_engine_play_sound(&s_Instance->m_AudioEngine, filepath, NULL);
}

void Audio::Play(const std::filesystem::path& filepath) {
	Audio::Play(filepath.c_str());
}

Audio::~Audio() {
	ma_engine_uninit(&s_Instance->m_AudioEngine);
}


}