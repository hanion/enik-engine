#pragma once 

#include "editor_layer.h"
#include "editor_tab.h"
#include "renderer/texture.h"


namespace Enik {


class HomeTab : public EditorTab {
public:
	HomeTab();

private:
	virtual void RenderContent() override final;
	virtual void InitializeDockspace() override final;

	void ShowNewProject();

	bool m_StartShowingNewProject = false;
	NewProjectType m_NewProjType = NewProjectType::EXAMPLE_SQUAREUP;

};


}
