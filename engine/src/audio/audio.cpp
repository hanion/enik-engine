#include "audio.h"
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"


namespace Enik {

Scope<Audio> Audio::s_Instance = CreateScope<Audio>();

struct AudioEngine {
	ma_engine engine;
};

void Audio::Init() {
	s_Instance->m_AudioEngine = CreateScope<AudioEngine>();

	ma_result result = ma_engine_init(NULL, &s_Instance->m_AudioEngine->engine);
	if (result != MA_SUCCESS) {
		EN_CORE_ERROR("Failed to initialize AudioEngine!");
	}
	ma_engine_set_volume(&s_Instance->m_AudioEngine->engine, 0.5f);
}

void Audio::Play(const char* filepath) {
	ma_engine_play_sound(&s_Instance->m_AudioEngine->engine, filepath, NULL);
}

void Audio::Play(const std::filesystem::path& filepath) {
	Audio::Play(filepath.string().c_str());
}

Audio::~Audio() {
	ma_engine_uninit(&s_Instance->m_AudioEngine->engine);
}


}