#include "project.h"
#include "project/project_serializer.h"

namespace Enik {

Ref<Project> Project::New() {
	s_ActiveProject = CreateRef<Project>();
	return s_ActiveProject;
}

Ref<Project> Project::Load(const std::filesystem::path& path) {
	Ref<Project> project = CreateRef<Project>();

	ProjectSerializer serializer = ProjectSerializer(project);
	if (serializer.Deserialize(path)) {
		project->m_ProjectDirectory = std::filesystem::canonical(path.parent_path());
		s_ActiveProject = project;
		return s_ActiveProject;
	}

	return nullptr;
}
void Project::Save(const std::filesystem::path& path) {
	ProjectSerializer serializer = ProjectSerializer(s_ActiveProject);
	s_ActiveProject->m_ProjectDirectory = std::filesystem::canonical(path.parent_path());
	serializer.Serialize(path);
}
}