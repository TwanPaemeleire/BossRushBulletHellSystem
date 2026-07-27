#if _DEBUG
#if __has_include(<vld.h>)
#include <vld.h>
#endif
#endif

#include <Bloodforge.h>
#include <WindowUtils.h>
#include <EntityManager.h>
#include <IdCreator.h>
#include <ResourceManager.h>
#include <BloodRenderer.h>
#include <SceneSystemManager.h>
#include <InputHandler.h>

#include <SceneDataComponent.h>
#include <SceneManagingDataComponent.h>
#include <SpriteComponent.h>
#include <SpriteAnimatorComponent.h>

void LoadFunction()
{
	Bloodforge::EntityManager& entityManager = Bloodforge::EntityManager::GetInstance();
	Bloodforge::ResourceManager& resourceManager = Bloodforge::ResourceManager::GetInstance();
	Bloodforge::BloodRenderer& renderer = Bloodforge::BloodRenderer::GetInstance();
}

int main(int, char* [])
{
	Bloodforge::Bloodforge& engine = Bloodforge::Bloodforge::GetInstance();
	engine.SetResourcesDirectory("Resources");

	Bloodforge::WindowUtils::SetWindowAlwaysOnTop(false);
	Bloodforge::WindowUtils::SetWindowBordered(false);
	Bloodforge::WindowUtils::SetWindowFullScreen(false);
	Bloodforge::WindowUtils::SetWindowSize(700, 700);
	Bloodforge::WindowUtils::SetWindowTitle("BossRushBulletHellSystem");

	Bloodforge::EntityManager& entityManager = Bloodforge::EntityManager::GetInstance();
	Bloodforge::SceneSystemManager& sceneSystemManager = Bloodforge::SceneSystemManager::GetInstance();

	Bloodforge::InputHandler::GetInstance().CreateMap(CreateId("MainMap"));
	Bloodforge::InputHandler::GetInstance().SetCurrentMap(CreateId("MainMap"));

	{
		Bloodforge::Entity& sceneDataEntity = entityManager.CreateEntity();
		sceneDataEntity.DontDestroyOnSceneSwitch = true;
		int entityId = sceneDataEntity.Id;
		Bloodforge::SceneDataComponent* sceneData = entityManager.AddComponent<Bloodforge::SceneDataComponent>(entityId);
		sceneData->LoadFunction = LoadFunction;

		Bloodforge::EntityView<Bloodforge::SceneManagingDataComponent> view = entityManager.GetOrCreateFirstEntityWithComponents<Bloodforge::SceneManagingDataComponent>();
		Bloodforge::SceneManagingDataComponent& sceneManagingData = view.GetComponent<Bloodforge::SceneManagingDataComponent>();
		entityManager.GetEntity(view.EntityId).DontDestroyOnSceneSwitch = true;
		sceneManagingData.SceneToLoadDataEntityId = entityId;
		sceneManagingData.ShouldLoadScene = true;
	}

	engine.Run();
	return 0;
}