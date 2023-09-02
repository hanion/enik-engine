#pragma once

#include "project/project.h"

namespace Enik {

class ProjectSerializer {
public:
	ProjectSerializer(Ref<Project>& project);

	void Serialize(std::filesystem::path path);
	bool Deserialize(std::filesystem::path path);

private:
	Ref<Project> m_Project;

};

}