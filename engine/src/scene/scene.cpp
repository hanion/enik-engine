#include "scene.h"

#include <glm.hpp>
#include "scene/components.h"
#include "renderer/renderer2D.h"
#include "scene/entity.h"

namespace Enik {

Scene::Scene() {
	
}

Scene::~Scene() {

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

	Camera* mainCamera;
	glm::mat4 mainCameraTransform;
	/* Get Camera */ {
		EN_PROFILE_SECTION("Get Camera");

		auto view = m_Registry.view<Component::Transform, Component::Camera>();
		for (auto entity : view) {
			Component::Transform& transform = view.get<Component::Transform>  (entity);
			Component::Camera& camera       = view.get<Component::Camera>     (entity);
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
}