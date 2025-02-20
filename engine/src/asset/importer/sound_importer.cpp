#include "sound_importer.h"
#include "audio/audio.h"

namespace Enik {
namespace SoundImporter {

Ref<SoundAsset> ImportSound(AssetHandle handle, const AssetMetadata& metadata) {
	return Audio::CreateSound(metadata.FilePath.string().c_str());
}

}
}
