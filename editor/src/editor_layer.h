#pragma once
#include <Enik.h>

#include "dialogs/dialog_file.h"
#include "dialogs/dialog_confirm.h"
#include "renderer/texture.h"

#include "tabs/editor_tab.h"


using namespace Enik;

class EditorLayer : public Layer {
public:
	EditorLayer();
	virtual ~EditorLayer() = default;

	virtual void OnAttach() override final;
	virtual void OnDetach() override final;

	virtual void OnUpdate(Timestep timestep) override final;
	virtual void OnFixedUpdate() override final;
	virtual void OnEvent(Event &event) override final;
	virtual void OnImGuiRender() override final;

	void RequestOpenAsset(const std::filesystem::path& path);

private:
	void ProcessOpenAssetRequests();
	void OpenAsset(const std::filesystem::path& path);

	bool BeginMainDockspace();
	void RenderContent();
	void InitializeMainDockspace();

	void CreateNewScene();

	void CreateNewProject();
	void LoadProject(const std::filesystem::path &path);
	void SaveProject();
	void ReloadProject();

	bool OnKeyPressed (KeyPressedEvent &event);

	void UpdateWindowTitle();

	void OnOverlayRender();


	void ExitEditor();

private:
	std::vector<Ref<EditorTab>> m_EditorTabs;

	ImGuiID          m_MainDockspaceID;
	ImGuiWindowClass m_MainDockspaceClass;
	bool             m_MainDockspaceInitialized = false;

	std::vector<std::filesystem::path> m_OpenAssetRequests;

};