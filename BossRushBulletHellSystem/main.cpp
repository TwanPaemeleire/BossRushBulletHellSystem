#if _DEBUG
#if __has_include(<vld.h>)
#include <vld.h>
#endif
#endif

#include <Bloodforge.h>
#include <WindowUtils.h>
#include <EntityManager.h>
#include <IdCreator.h>
#include <SceneSystemManager.h>
#include <InputHandler.h>

#include <SceneDataComponent.h>
#include <SceneManagingDataComponent.h>
#include "Boss/BossCreation/BossFilesFactory.h"

#include <Boss/RuntimeCurrentBossData.h>
#include <FileSaveLoadUtils.h>
#include <Boss/BossesDataBank.h>

#include <BossActions/BossAttackingSystem.h>

void SaveAllBosses()
{
	BossFilesFactory factory;
	{
		BossMakerHelper bossMakerHelper = factory.StartBossCreation(CreateId("FirstBoss"), "FirstBoss", 0);

		BossPhaseMakerHelper& phase1MakerHelper = bossMakerHelper.AddPhaseToBoss(0.5f, true, false);

		BossPhaseActionMakerHelper& action1Phase1MakerHelper = phase1MakerHelper.AddActionToBossPhase(20, CreateId("RotatingShooters"));
		action1Phase1MakerHelper.AddValueToBlackboard(CreateId("AmountOfShooters"), 8);
		action1Phase1MakerHelper.AddValueToBlackboard(CreateId("AmountOfFullRotations"), 2.0f);
		action1Phase1MakerHelper.AddValueToBlackboard(CreateId("ShootersRotationSpeed"), 20.0f);
		action1Phase1MakerHelper.AddValueToBlackboard(CreateId("ShootersRotationSpeedApplyTime"), 0.5f);
		action1Phase1MakerHelper.AddValueToBlackboard(CreateId("BulletShootDelay"), 0.2f);

		factory.FinishBossCreation(bossMakerHelper);
	}

	factory.WriteAllToFiles();
}

void LoadFunction()
{
	Bloodforge::EntityManager& entityManager = Bloodforge::EntityManager::GetInstance();
	Bloodforge::SceneSystemManager& sceneSystemManager = Bloodforge::SceneSystemManager::GetInstance();

	BossesDataBank& dataBank = entityManager.GetOrCreateFirstEntityWithComponents<BossesDataBank>().GetComponent<BossesDataBank>();

	RuntimeCurrentBossData& runtimeBossData = entityManager.GetOrCreateFirstEntityWithComponents<RuntimeCurrentBossData>().GetComponent<RuntimeCurrentBossData>();
	
	BossConfig bossConfig = Bloodforge::FileSaveLoadUtils::LoadFile<BossConfig>("Bosses/FirstBoss/0.json");
	runtimeBossData.CurrentBossConfig = bossConfig;
	runtimeBossData.CurrentBosId = CreateId("FirstBoss");
	runtimeBossData.CurrentBossLevel = 0;
	runtimeBossData.StartBossFightTrigger = true;
}

int main(int, char* [])
{
	Bloodforge::Bloodforge& engine = Bloodforge::Bloodforge::GetInstance();
	engine.SetResourcesDirectory("Resources");

	SaveAllBosses();

	Bloodforge::WindowUtils::SetWindowAlwaysOnTop(false);
	Bloodforge::WindowUtils::SetWindowBordered(false);
	Bloodforge::WindowUtils::SetWindowFullScreen(false);
	Bloodforge::WindowUtils::SetWindowSize(700, 700);
	Bloodforge::WindowUtils::SetWindowTitle("BossRushBulletHellSystem");

	Bloodforge::EntityManager& entityManager = Bloodforge::EntityManager::GetInstance();
	Bloodforge::SceneSystemManager& sceneSystemManager = Bloodforge::SceneSystemManager::GetInstance();

	sceneSystemManager.TryRegisterSystem<BossAttackingSystem>();

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