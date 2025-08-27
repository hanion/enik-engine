#pragma once 

#include "editor_tab.h"
#include "renderer/texture.h"

static const std::filesystem::path s_EngineSourcePath = "../..";
static const std::filesystem::path s_TemplatePath = s_EngineSourcePath / "project";
static const std::filesystem::path s_ExamplesPath = s_EngineSourcePath / "examples";
static const std::filesystem::path s_ExampleSnakeGame = s_ExamplesPath / "snake-game";
static const std::filesystem::path s_ExampleSquareUp  = s_ExamplesPath / "squareup";


namespace Enik {


class HomeTab : public EditorTab {
public:
	HomeTab();

private:
	virtual void RenderContent() override final;
	virtual void InitializeDockspace() override final;

	void ShowNewProject();

	bool m_StartShowingNewProject = false;
	enum class NewProjType {
		EXAMPLE_SNAKE,
		EXAMPLE_SQUAREUP,
		EMPTY
	};
	NewProjType m_NewProjType = NewProjType::EXAMPLE_SQUAREUP;

};


}
