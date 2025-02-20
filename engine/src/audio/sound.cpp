#include "sound.h"
#include "audio/audio.h"

namespace Enik {

SoundAsset::~SoundAsset() {
	Audio::UninitSound(sound);
}

}
