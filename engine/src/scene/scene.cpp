#include "scene.h"

#include <glm/glm.hpp>
#include "scene/components.h"
#include "renderer/renderer2D.h"
#include "scene/entity.h"

namespace Enik {

Scene::Scene() {
}

Scene::~Scene() {
	/* Destroy Scripts */ {
		m_Registry.view<Component::NativeScript>().each([=](auto entity, auto& ns){
			if (ns.Instance) {
				ns.Instance->OnDestroy();
				ns.DestroyScript(&ns);
			}
		});
	}
}

Entity Scene::CreateEntity(const std::string& name) {
	Entity entity = Entity(m_Registry.create(), this);
	entity.Add<Component::Transform>();
	auto& tag = entity.Add<Component::Tag>();
	tag.Text = name.empty() ? "Empty Entity" : name;
	return entity;
}

void Scene::OnUpdate(Timestep ts) {
	EN_PROFILE_SECTION("Scene::OnUpdate");

	// TODO do this On Editor Play
	/* Update Scripts */ {
		m_Registry.view<Component::NativeScript>().each([=](auto entity, auto& ns){
			if (!ns.Instance) {
				ns.Instance = ns.InstantiateScript();
				ns.Instance->m_Entity = Entity(entity, this);
				ns.Instance->OnCreate();
			}
			ns.Instance->OnUpdate(ts);
		});
	}



	Camera* mainCamera = nullptr;
	glm::mat4 mainCameraTransform;
	/* Get Camera */ {
		EN_PROFILE_SECTION("Get Camera");

		auto view = m_Registry.view<Component::Transform, Component::Camera>();
		for (auto entity : view) {
			Component::Transform& transform = view.get<Component::Transform>  (entity);
			Component::Camera& camera       = view.get<Component::Camera>     (entity);
			
			if (not camera.Primary) { continue; }
			
			mainCamera = &camera.Cam;
			mainCameraTransform = transform.GetTransform();
		}
	}

	if (not mainCamera) { return; }

	Renderer2D::BeginScene(*mainCamera, mainCameraTransform);

	/* Get Sprites */ {
		EN_PROFILE_SECTION("Get Sprites");

		auto group = m_Registry.group<Component::Transform>(entt::get<Component::SpriteRenderer>);
		for (auto entity : group) {
			Component::Transform& transform   = group.get<Component::Transform>     (entity);
			Component::SpriteRenderer& sprite = group.get<Component::SpriteRenderer>(entity);

			Renderer2D::DrawQuad(transform, sprite);

		}
	}

	Renderer2D::EndScene();

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

}