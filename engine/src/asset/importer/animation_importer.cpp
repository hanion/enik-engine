#include "animation_importer.h"
#include "scene/animation.h"
#include <yaml-cpp/yaml.h>

namespace Enik {


Ref<Animation> AnimationImporter::ImportAnimation(AssetHandle handle, const AssetMetadata& metadata) {
	return DeserializeAnimation(metadata.FilePath);
}

Ref<Animation> AnimationImporter::DeserializeAnimation(const std::filesystem::path& path) {
	YAML::Node data;
	try {
		data = YAML::LoadFile(path);
	}
	catch (const YAML::ParserException& e) {
		EN_CORE_ERROR("Failed to load .anim file '{0}'\n	{1}", path, e.what());
		return nullptr;
	}

	if (!data.IsMap() || !data["Animation"]) {
		EN_CORE_ERROR("Invalid .anim file format: {0}", path);
		return nullptr;
	}

	Ref<Animation> animation = CreateRef<Animation>();
	animation->Name = data["Animation"].as<std::string>();

	if (data["Duration"]) {
		animation->Duration = data["Duration"].as<float>();
	}

	if (data["Ease"]) {
		// TODO:
	}

	if (auto tracks = data["Tracks"]) {
		for (std::size_t i = 0; i < tracks.size(); ++i) {
			auto trackNode = tracks[i];

			TrackProperty property = Animation::TrackPropertyFromString(trackNode["TrackProperty"].as<std::string>());

			Track track;
			track.Property = property;

			if (auto keyframes = trackNode["Keyframes"]) {
				for (std::size_t j = 0; j < keyframes.size(); ++j) {
					auto keyframeNode = keyframes[j];

					Keyframe keyframe;
					keyframe.Time = keyframeNode["Time"].as<float>();

					if (keyframeNode["Value"].IsScalar()) {
						keyframe.Value = keyframeNode["Value"].as<float>();
					} else if (keyframeNode["Value"].IsSequence()) {
						auto sequence = keyframeNode["Value"].as<std::vector<float>>();
						if (sequence.size() == 2) {
							keyframe.Value = glm::vec2(sequence[0], sequence[1]);
						} else if (sequence.size() == 3) {
							keyframe.Value = glm::vec3(sequence[0], sequence[1], sequence[2]);
						} else if (sequence.size() == 4) {
							keyframe.Value = glm::vec4(sequence[0], sequence[1], sequence[2], sequence[3]);
						}
					} else {
						EN_CORE_WARN("Unsupported keyframe value type in track {0}", i);
						continue;
					}

					track.Keyframes.emplace_back(keyframe);
				}
			}

			animation->Tracks.emplace_back(track);
		}
	}

	// NOTE: tracks need to be sorted for animations to work correctly
	animation->SortKeyframes();

	return animation;
}


void AnimationImporter::SerializeAnimation(const Ref<Animation>& animation, const std::filesystem::path& path) {
	// NOTE: tracks need to be sorted for animations to work correctly
	animation->SortKeyframes();

	YAML::Emitter out;
	out << YAML::BeginMap;

	out << YAML::Key << "Animation" << YAML::Value << animation->Name;

	out << YAML::Key << "Duration" << YAML::Value << animation->Duration;

	out << YAML::Key << "Tracks" << YAML::Value << YAML::BeginSeq;
	for (const Track& track : animation->Tracks) {
		out << YAML::BeginMap;

		out << YAML::Key << "TrackProperty" << YAML::Value << Animation::TrackPropertyToString(track.Property);

		out << YAML::Key << "Keyframes" << YAML::Value << YAML::BeginSeq;
		for (const auto& keyframe : track.Keyframes) {
			out << YAML::BeginMap;
			out << YAML::Key << "Time" << YAML::Value << keyframe.Time;

			out << YAML::Key << "Value";
			if (std::holds_alternative<float>(keyframe.Value)) {
				out << YAML::Value << std::get<float>(keyframe.Value);
			} else if (std::holds_alternative<glm::vec2>(keyframe.Value)) {
				const auto& vec = std::get<glm::vec2>(keyframe.Value);
				out << YAML::Value << YAML::Flow << YAML::BeginSeq << vec.x << vec.y << YAML::EndSeq;
			} else if (std::holds_alternative<glm::vec3>(keyframe.Value)) {
				const auto& vec = std::get<glm::vec3>(keyframe.Value);
				out << YAML::Value << YAML::Flow << YAML::BeginSeq << vec.x << vec.y << vec.z << YAML::EndSeq;
			} else if (std::holds_alternative<glm::vec4>(keyframe.Value)) {
				const auto& vec = std::get<glm::vec4>(keyframe.Value);
				out << YAML::Value << YAML::Flow << YAML::BeginSeq << vec.x << vec.y << vec.z << vec.w << YAML::EndSeq;
			}

			out << YAML::EndMap;
		}
		out << YAML::EndSeq;

		out << YAML::EndMap;
	}
	out << YAML::EndSeq;

	out << YAML::EndMap;

	std::ofstream fout(path);
	fout << out.c_str();
}

}
