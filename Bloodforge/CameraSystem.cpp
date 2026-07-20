#include "pch.h"
#include "CameraSystem.h"
#include "EntityManager.h"
#include "CameraComponent.h"
#include "TransformComponent.h"
#include "BloodRenderer.h"

void Bloodforge::CameraSystem::OnLateUpdate()
{
	EntityManager& entityManager = EntityManager::GetInstance();
	EntityView<CameraComponent, TransformComponent> view = entityManager.GetOrCreateFirstEntityWithComponents<CameraComponent, TransformComponent>();
	TransformComponent& transformComp = view.GetComponent<TransformComponent>();
	CameraComponent& cameraComp = view.GetComponent<CameraComponent>();

	if (cameraComp.EntityIdToFollow == -1) return;
	Vector2 pos = entityManager.GetComponent<TransformComponent>(cameraComp.EntityIdToFollow)->GetWorldPosition();

	float halfWidth = BloodRenderer::GetInstance().GetWindowWidth() / 2.0f;
	float halfHeight = BloodRenderer::GetInstance().GetWindowHeight() / 2.0f;

	pos -= {halfWidth, halfHeight};

	transformComp.SetLocalPosition(pos);
}