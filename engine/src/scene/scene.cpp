#include "scene.h"

#include <glm/glm.hpp>

#include "renderer/renderer2D.h"
#include "scene/components.h"
#include "scene/entity.h"
#include "script_system/script_system.h"
#include "scene/scene_serializer.h"
#include "core/application.h"
#include "scene/tween.h"

namespace Enik {

Scene::Scene() {
	ScriptSystem::SetSceneContext(this);
}

// only called from SceneSerializer::Deserialize
void Scene::InstantiateAutoLoads() {
	auto& al = Project::GetActive()->GetConfig().autoloads;
	for (size_t i = 0; i < al.size(); ++i) {
		bool already_autoloaded = false;
		for (size_t j = 0; j < m_autoloaded.size(); ++j) {
			if (al[i] == m_autoloaded[j]) {
				already_autoloaded = true;
				break;
			}
		}
		if (!already_autoloaded) {
			Entity e = InstantiatePrefab(al[i]);
			e.GetOrAdd<Component::SceneControl>().AutoLoaded = true;
			m_autoloaded.push_back(al[i]);
		}
	}
}

Scene::~Scene() {
	m_Physics.Uninitialize();
	DestroyScriptableEntities();
}

Entity Scene::CreateEntity(const std::string& name) {
	return CreateEntityWithUUID(UUID());
}

Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name) {
	Entity entity = Entity(m_Registry.create(), this);
	entity.Add<Component::ID>(uuid);
	entity.Add<Component::Transform>();
	auto& tag = entity.Add<Component::Tag>();
	tag.Text = name.empty() ? "Empty Entity" : name;
	return entity;
}

void Scene::DestroyEntity(Entity entity) {
	m_deferred_destroy.push_back(entity);
}

void Scene::DestroyDeferredEntities() {
	while (m_deferred_destroy.size() > 0) {
		Entity entity = m_deferred_destroy.back();
		m_deferred_destroy.pop_back();
		DestroyEntityImmediatelyInternal(entity);
	}
	m_deferred_destroy.clear();
}

void Scene::DestroyEntityImmediatelyInternal(Entity entity) {
	if (!entity) { return; }

	if (entity.HasFamily()) {
		entity.Reparent({});

		std::vector<Entity> children;
		for (auto& child : entity.GetChildren()) {
			children.push_back(child);
		}
		for (const auto& child : children) {
			DestroyEntityImmediatelyInternal(child);
		}
	}

	if (entity.Has<Component::RigidBody>()) {
		auto& b = entity.Get<Component::RigidBody>();
		m_Physics.RemovePhysicsBody(b.body);
		b.body = nullptr;
	} else if (entity.Has<Component::CollisionBody>()) {
		auto& b = entity.Get<Component::CollisionBody>();
		m_Physics.RemovePhysicsBody(b.body);
		b.body = nullptr;
	}

	if (m_Registry.valid  ((entt::entity)entity)) {
		m_Registry.destroy((entt::entity)entity);
	}
}

Entity Scene::InstantiatePrefab(const std::filesystem::path& path, UUID instance_uuid) {
	std::filesystem::path canonical_path = Project::GetAbsolutePath(path);
	if (canonical_path.empty()) {
		EN_CORE_ERROR("InstantiatePrefab invalid path! {} {}", instance_uuid, path.string());
		return {};
	}
	SceneSerializer serializer = SceneSerializer(this);
	Entity e = serializer.InstantiatePrefab(canonical_path.string(), instance_uuid);
	return e;
}

void Scene::OnUpdateEditor(Timestep ts, OrthographicCameraController& camera) {
	EN_PROFILE_SECTION("Scene::OnUpdateEditor");

	SetGlobalTransforms();

	Renderer2D::BeginScene(camera.GetCamera());

	/* Get Sprites */ {
		EN_PROFILE_SECTION("Get Sprites");

		auto group = m_Registry.group<Component::SpriteRenderer>(entt::get<Component::Transform>);
		for (auto entity : group) {
			Component::Transform& transform   = group.get<Component::Transform>     (entity);
			Component::SpriteRenderer& sprite = group.get<Component::SpriteRenderer>(entity);

			Renderer2D::DrawQuad(transform, sprite, (int32_t)entity);
		}
	}

	{
		EN_PROFILE_SECTION("Render Text");
		auto group = m_Registry.group<Component::Text>(entt::get<Component::Transform>);
		for (auto entity : group) {
			Component::Transform& transform = group.get<Component::Transform>(entity);
			Component::Text& text = group.get<Component::Text>(entity);
			Renderer2D::DrawText2D(transform, text, (int32_t)entity);
		}
	}

	Renderer2D::EndScene();

	if (m_deferred_scene_change) {
		ChangeToDeferredScene();
	}
	DestroyDeferredEntities();
}

void Scene::OnUpdateRuntime(Timestep ts) {
	EN_PROFILE_SECTION("Scene::OnUpdateRuntime");

	/* Update Scripts */
	if (not m_IsPaused or m_StepFrames-- > 0) {
		m_Registry.view<Component::NativeScript>().each([=](auto entity, Component::NativeScript& ns) {
			if (not ns.Instance or ns.Instance == nullptr) {
				if (ns.InstantiateScript and ns.InstantiateScript != nullptr) {
					ns.Instance = ns.InstantiateScript();
					ns.Instance->m_Entity = Entity(entity, this);
					ns.ApplyNativeScriptFieldsToInstance();
					ns.Called_OnCreate = true;
					ns.Instance->OnCreate();
				}
			// NOTE: reason: we need to be able to get script instance
			// right after doing InstantiateScript
			} else if (!ns.Called_OnCreate) {
				EN_ASSERT(ns.Instance);
				ns.Called_OnCreate = true;
				ns.Instance->OnCreate();
			} else {
				ns.Instance->OnUpdate(ts);
			}
		});


		Tween::StepAll(ts);

		// Update animations
		m_Registry.view<Component::AnimationPlayer>().each([=](auto entity, Component::AnimationPlayer& ap) {
			if (ap.BoundEntity == nullptr) {
				ap.BoundEntity = CreateRef<Entity>(entity, this);
			}
			ap.Update(ts);
		});

	}



	// Rendering

	auto primary_camera = GetPrimaryCameraEntity();
	if (not primary_camera) {
		return;
	}

	Renderer2D::BeginScene(
		primary_camera.Get<Component::Camera>().Cam,
		primary_camera.Get<Component::Transform>().GetTransform()
	);

	/* Get Sprites */ {
		EN_PROFILE_SECTION("Get Sprites");

		auto group = m_Registry.group<Component::SpriteRenderer>(entt::get<Component::Transform>);
		for (auto entity : group) {
			Component::Transform& transform   = group.get<Component::Transform>     (entity);
			Component::SpriteRenderer& sprite = group.get<Component::SpriteRenderer>(entity);

			Renderer2D::DrawQuad(transform, sprite, (int32_t)entity);
		}
	}

	{
		EN_PROFILE_SECTION("Render Text");
		auto group = m_Registry.group<Component::Text>(entt::get<Component::Transform>);
		for (auto entity : group) {
			Component::Transform& transform = group.get<Component::Transform>(entity);
			Component::Text& text = group.get<Component::Text>(entity);
			Renderer2D::DrawText2D(transform, text, (int32_t)entity);
		}
	}

	Renderer2D::EndScene();

	if (m_deferred_scene_change) {
		ChangeToDeferredScene();
	}
	DestroyDeferredEntities();
}

void Scene::OnFixedUpdate() {
	SetGlobalTransforms();
	if (not m_IsPaused or m_StepFrames > 0) {
		{ EN_PROFILE_SECTION("Scene::OnFixedUpdate NativeScript calls");
		m_Registry.view<Component::NativeScript>().each([=](auto entity, auto& ns) {
			if (not ns.Instance or ns.Instance == nullptr) {
				if (ns.InstantiateScript and ns.InstantiateScript != nullptr) {
					ns.Instance = ns.InstantiateScript();
					ns.Instance->m_Entity = Entity(entity, this);
					ns.ApplyNativeScriptFieldsToInstance();
					ns.Called_OnCreate = true;
					ns.Instance->OnCreate();
				}
			} else if (!ns.Called_OnCreate) {
				EN_ASSERT(ns.Instance);
				ns.Called_OnCreate = true;
				ns.Instance->OnCreate();
			} else {
				ns.Instance->OnFixedUpdate();
			}
		});
		}

		if (!m_Physics.m_is_initialized) {
			m_Physics.Initialize(m_Registry, this);
			m_Physics.CreatePhysicsWorld();
		}
		m_Physics.UpdatePhysics();
	}
}


void Scene::OnKeyPressed(const KeyPressedEvent& event) {
	if (not m_IsPaused or m_StepFrames > 0) {
		m_Registry.view<Component::NativeScript>().each([=](auto entity, auto& ns) {
			if (ns.Instance and ns.Instance != nullptr) {
				ns.Instance->OnKeyPressed(event);
			}
		});
	}
}

void Scene::OnKeyReleased(const KeyReleasedEvent& event) {
	if (not m_IsPaused or m_StepFrames > 0) {
		m_Registry.view<Component::NativeScript>().each([=](auto entity, auto& ns) {
			if (ns.Instance and ns.Instance != nullptr) {
				ns.Instance->OnKeyReleased(event);
			}
		});
	}
}

void Scene::OnMouseButtonPressed(const MouseButtonPressedEvent& event) {
	if (not m_IsPaused or m_StepFrames > 0) {
		m_Registry.view<Component::NativeScript>().each([=](auto entity, auto& ns) {
			if (ns.Instance and ns.Instance != nullptr) {
				ns.Instance->OnMouseButtonPressed(event);
			}
		});
	}
}

void Scene::OnMouseButtonReleased(const MouseButtonReleasedEvent& event) {
	if (not m_IsPaused or m_StepFrames > 0) {
		m_Registry.view<Component::NativeScript>().each([=](auto entity, auto& ns) {
			if (ns.Instance and ns.Instance != nullptr) {
				ns.Instance->OnMouseButtonReleased(event);
			}
		});
	}
}

void Scene::OnMouseScrolled(const MouseScrolledEvent& event) {
	if (not m_IsPaused or m_StepFrames > 0) {
		m_Registry.view<Component::NativeScript>().each([=](auto entity, auto& ns) {
			if (ns.Instance and ns.Instance != nullptr) {
				ns.Instance->OnMouseScrolled(event);
			}
		});
	}
}

void Scene::OnViewportResize(uint32_t width, uint32_t height) {
	m_ViewportWidth = width;
	m_ViewportHeight = height;

	auto view = m_Registry.view<Component::Camera>();
	for (auto entity : view) {
		Component::Camera& camera = view.get<Component::Camera>(entity);
		if (not camera.FixedAspectRatio) {
			camera.Cam.SetViewportSize(width, height);
		}
	}
}

void Scene::OnViewportResize(glm::vec2 position, uint32_t width, uint32_t height) {
	m_ViewportWidth = width;
	m_ViewportHeight = height;

	auto view = m_Registry.view<Component::Camera>();
	for (auto entity : view) {
		Component::Camera& camera = view.get<Component::Camera>(entity);
		if (not camera.FixedAspectRatio) {
			camera.Cam.SetViewportSize(position, width, height);
		}
	}
}

void Scene::DestroyScriptableEntities() {
	// FIX: seg fault on ->OnDestroy(), hapens when closing the editor while scene is playing
	// somehow fixed by including tracy ???
	return;
	m_Registry.view<Component::NativeScript>().each([=](entt::entity entity, Component::NativeScript& ns) {
		if (ns.Instance && m_Registry.valid(entity)) {
			ns.Instance->OnDestroy();
			ns.DestroyScript(&ns);
		}
	});
}

void Scene::ClearNativeScripts() {
	m_Registry.view<Component::NativeScript>().each([=](auto entity, auto& ns) {
		ns.InstantiateScript = nullptr;
		ns.DestroyScript(&ns);
	});
}

Entity Scene::GetPrimaryCameraEntity() {
	EN_PROFILE_SECTION("GetPrimaryCameraEntity");

	auto view = m_Registry.view<Component::Transform, Component::Camera>();
	for (auto entity : view) {
		Component::Camera& camera = view.get<Component::Camera>(entity);
		if (camera.Primary) {
			return Entity(entity, this);
		}
	}

	return Entity();
}

Entity Scene::FindEntityByUUID(UUID uuid) {
	auto view = m_Registry.view<Component::ID>();
	for (auto entity_handle : view) {
		const Component::ID& id = view.get<Component::ID>(entity_handle);
		if (id.uuid == uuid) {
			return Entity(entity_handle, this);
		}
	}
	return Entity();
}

Entity Scene::FindEntityByName(const std::string& name) {
	auto view = m_Registry.view<Component::Tag>();
	for (auto entity_handle : view) {
		const Component::Tag& tag = view.get<Component::Tag>(entity_handle);
		if (tag.Text == name) {
			return Entity(entity_handle, this);
		}
	}
	return Entity();
}

void Scene::SetGlobalTransforms() {
	EN_PROFILE_SCOPE;

	// reset all global transforms
	m_Registry.view<Component::Transform>().each(
		[=](auto entity, auto& transform) {
			transform.GlobalPosition = transform.LocalPosition;
			transform.GlobalRotation = transform.LocalRotation;
			transform.GlobalScale    = transform.LocalScale;
		}
	);

	auto group = m_Registry.group<Component::Family>(entt::get<Component::Transform>);
	for (auto entity : group) {
		Component::Transform& transform = group.get<Component::Transform>(entity);
		Component::Family&    family    = group.get<Component::Family>   (entity);
		if (family.HasParent()) {
			continue;
		}
		family.SetChildrenGlobalTransformRecursive(transform);
	}
}

void Scene::CloseApplication() {
#ifdef EN_DEBUG
	SetPaused(true);
#else
	Application::Get().Close();
#endif
}

void Scene::ChangeScene(const std::string& path) {
	if (m_deferred_scene_change) {
		return;
	}

	const std::filesystem::path absolute = Project::GetAbsolutePath(path);
	if (!std::filesystem::exists(absolute)) {
		EN_CORE_ERROR("ChangeScene: scene not found {}", path.c_str());
		return;
	}
	m_deferred_scene_path = absolute.string();
	m_deferred_scene_change = true;
}


void Scene::ChangeToDeferredScene() {
	if (!m_deferred_scene_change || m_deferred_scene_path.empty()) {
		return;
	}


	std::unordered_set<entt::entity> entities_to_keep;
	m_Registry.each([&](entt::entity entity) {
		if (m_Registry.any_of<Component::SceneControl>(entity)) {
			const auto& sc = m_Registry.get<Component::SceneControl>(entity);
			if (sc.Persistent) {
				std::function<void(entt::entity)> collect_children = [&](entt::entity parent) {
					entities_to_keep.insert(parent);
					if (m_Registry.any_of<Component::Family>(parent)) {
						const auto& family = m_Registry.get<Component::Family>(parent);
						for (const auto& child : family.Children) {
							collect_children(child.m_Handle);
						}
					}
				};
				collect_children(entity);
			}
		}
	});

	// destroy every entity except for Persistent ones
	m_Registry.each([&](entt::entity entity) {
		if (entities_to_keep.find(entity) == entities_to_keep.end()) {
			DestroyEntityImmediatelyInternal(Entity(entity, this));
		}
	});

	SceneSerializer serializer = SceneSerializer(this);
	if (serializer.Deserialize(m_deferred_scene_path)) {
		ScriptSystem::SetSceneContext(this);
		NeedViewportResize = true;
		//EN_CORE_INFO("Changed Scene to '{}'", m_deferred_scene_path.c_str());

		for (auto entity : entities_to_keep) {
			Component::NativeScript* ns = m_Registry.try_get<Component::NativeScript>(entity);
			Component::SceneControl* sc = m_Registry.try_get<Component::SceneControl>(entity);
			if (sc && ns) {
				if (sc->Persistent) {
					ns->Instance->OnSceneChanged(m_deferred_scene_path);
				}
			}
		}
	}

	m_deferred_scene_path.clear();
	m_deferred_scene_change = false;
}

}