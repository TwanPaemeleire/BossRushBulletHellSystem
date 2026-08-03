#include "FpsDisplaySystem.h"
#include <EntityManager.h>
#include "FPSDisplayer.h"
#include <TextComponent.h>
#include <BloodTime.h>
#include <ResourceManager.h>

using namespace Bloodforge;

void FpsDisplaySystem::OnUpdate()
{
	EntityManager& entityManager = EntityManager::GetInstance();
	EntityView<TransformComponent, FPSDisplayer> view = entityManager.GetOrCreateFirstEntityWithComponents<TransformComponent, FPSDisplayer>();
	if(!entityManager.HasComponent<TextComponent>(view.EntityId))
	{
		entityManager.AddComponent<TextComponent>(view.EntityId)->SetFont(ResourceManager::GetInstance().LoadFont("Font.ttf", 24.0f));
	}
	entityManager.GetComponent<TextComponent>(view.EntityId)->SetText("FPS: " + std::to_string((int)(1.0f / BloodTime::GetInstance().DeltaTime)));
	entityManager.GetComponent<TransformComponent>(view.EntityId)->SetLocalPosition({ 25.0f, 25.0f });
}