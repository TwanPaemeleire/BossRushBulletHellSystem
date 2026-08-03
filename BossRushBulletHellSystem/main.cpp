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
#include <Projectiles/BasicProjectileSystem.h>
#include <SpriteComponent.h>
#include <SpriteAnimatorComponent.h>
#include <ResourceManager.h>
#include <CircleColliderComponent.h>
#include "Player/PlayerMovementSystem.h"
#include "Player/PlayerMovement.h"
#include "Utils/FpsDisplaySystem.h"

void SaveAllBosses()
{
	BossFilesFactory factory;
	{
		BossMakerHelper bossMakerHelper = factory.StartBossCreation(CreateId("FirstBoss"), "FirstBoss", 0);

		BossPhaseMakerHelper& phase1MakerHelper = bossMakerHelper.AddPhaseToBoss(0.5f, true, false);

		BossPhaseActionMakerHelper& action1Phase1MakerHelper = phase1MakerHelper.AddActionToBossPhase(20, CreateId("RotatingShooters"));
		action1Phase1MakerHelper.AddValueToBlackboard(CreateId("AmountOfShooters"), 8);
		action1Phase1MakerHelper.AddValueToBlackboard(CreateId("AmountOfFullRotations"), 10.0f);
		action1Phase1MakerHelper.AddValueToBlackboard(CreateId("ShootersRotationSpeed"), 360.0f);
		action1Phase1MakerHelper.AddValueToBlackboard(CreateId("ShootersRotationSpeedApplyTime"), 0.5f);
		action1Phase1MakerHelper.AddValueToBlackboard(CreateId("BulletShootDelay"), 0.08f);
		action1Phase1MakerHelper.AddValueToBlackboard(CreateId("BulletSpawnPos"), Bloodforge::Vector2(Bloodforge::WindowUtils::GetWindowSize() / 2.0f));

		factory.FinishBossCreation(bossMakerHelper);
	}

	factory.WriteAllToFiles();
}

void MakeBackgroundPart(int layer)
{
	Bloodforge::EntityManager& entityManager = Bloodforge::EntityManager::GetInstance();
	Bloodforge::Entity& backgroundEntity = entityManager.CreateEntity();
	int backgroundEntityId = backgroundEntity.Id;
	{
		Bloodforge::SpriteComponent* spriteComp = entityManager.AddComponent<Bloodforge::SpriteComponent>(backgroundEntityId);
		spriteComp->DrawOrder = -layer;
		spriteComp->SetTexture(Bloodforge::ResourceManager::GetInstance().LoadTexture("Background/" + std::to_string(layer) + ".png"));

		Bloodforge::TransformComponent* transform = entityManager.GetComponent<Bloodforge::TransformComponent>(backgroundEntityId);
		transform->SetLocalPosition(Bloodforge::WindowUtils::GetWindowSize() / 2.0f);
		transform->SetLocalScale({ 1.0f, 1.0f });
	}
}

void LoadFunction()
{
	Bloodforge::EntityManager& entityManager = Bloodforge::EntityManager::GetInstance();

	entityManager.GetOrCreateFirstEntityWithComponents<BossesDataBank>().GetComponent<BossesDataBank>();

	RuntimeCurrentBossData& runtimeBossData = entityManager.GetOrCreateFirstEntityWithComponents<RuntimeCurrentBossData>().GetComponent<RuntimeCurrentBossData>();
	
	BossConfig bossConfig = Bloodforge::FileSaveLoadUtils::LoadFile<BossConfig>("Bosses/FirstBoss/0.json");
	runtimeBossData.CurrentBossConfig = bossConfig;
	runtimeBossData.CurrentBosId = CreateId("FirstBoss");
	runtimeBossData.CurrentBossLevel = 0;
	runtimeBossData.StartBossFightTrigger = true;


	Bloodforge::Entity& inconsistencyEntity = entityManager.CreateEntity();
	int inconsistencyEntityId = inconsistencyEntity.Id;
	{
		Bloodforge::SpriteComponent* spriteComponent = entityManager.AddComponent<Bloodforge::SpriteComponent>(inconsistencyEntityId);
		spriteComponent->Color = { 255, 0,0, 255 };
		Bloodforge::SpriteAnimatorComponent* spriteAnimator = entityManager.AddComponent<Bloodforge::SpriteAnimatorComponent>(inconsistencyEntityId);

		Bloodforge::AnimationData animData;
		animData.NumberOfFrames = 4;
		animData.FrameTime = 0.08f;
		animData.Texture = Bloodforge::ResourceManager::GetInstance().LoadTexture("InconsistencyArea.png");

		spriteAnimator->AddAnimation(CreateId("InconsistencyArea"), animData);
		spriteAnimator->PlayAnimation(CreateId("InconsistencyArea"));

		Bloodforge::TransformComponent* transform = entityManager.GetComponent<Bloodforge::TransformComponent>(inconsistencyEntityId);
		transform->SetLocalPosition(400.0f, 400.0f);

		Bloodforge::CircleColliderComponent* circleCollider = entityManager.AddComponent<Bloodforge::CircleColliderComponent>(inconsistencyEntityId);
		circleCollider->Radius = 80.0f;

		circleCollider->OnCollisionEnterEvent.AddListener([inconsistencyEntityId](int, int secondEntityId)
			{
				Bloodforge::Entity& otherEntity = Bloodforge::EntityManager::GetInstance().GetEntity(secondEntityId);
				if (otherEntity.Tag == CreateId("BossProjectile"))
				{
					Bloodforge::EntityManager::GetInstance().DestroyEntity(secondEntityId);
				}
			});
	}

	Bloodforge::Entity& playerEntity = entityManager.CreateEntity();
	int playerEntityId = playerEntity.Id;
	{
		entityManager.AddComponent<Bloodforge::SpriteComponent>(playerEntityId);
		Bloodforge::SpriteAnimatorComponent* spriteAnimator = entityManager.AddComponent<Bloodforge::SpriteAnimatorComponent>(playerEntityId);
		Bloodforge::AnimationData animData;
		animData.Texture = Bloodforge::ResourceManager::GetInstance().LoadTexture("TempPlayer.png");
		animData.NumberOfFrames = 10;
		animData.FrameTime = 0.08f;
		spriteAnimator->AddAnimation(CreateId("PlayerAnim"), animData);
		spriteAnimator->PlayAnimation(CreateId("PlayerAnim"));

		Bloodforge::TransformComponent* transform = entityManager.GetComponent<Bloodforge::TransformComponent>(playerEntityId);
		transform->SetLocalPosition(400.0f, 400.0f);

		entityManager.AddComponent<PlayerMovement>(playerEntityId);
	}

	Bloodforge::Entity& bossEntity = entityManager.CreateEntity();
	int bossEntityId = bossEntity.Id;
	{
		Bloodforge::SpriteComponent* spriteComp = entityManager.AddComponent<Bloodforge::SpriteComponent>(bossEntityId);
		spriteComp->DrawOrder = 5;

		Bloodforge::SpriteAnimatorComponent* spriteAnimator = entityManager.AddComponent<Bloodforge::SpriteAnimatorComponent>(bossEntityId);
		Bloodforge::AnimationData animData;
		animData.Texture = Bloodforge::ResourceManager::GetInstance().LoadTexture("TempFirstBoss.png");
		animData.NumberOfFrames = 12;
		animData.FrameTime = 0.08f;
		spriteAnimator->AddAnimation(CreateId("BossAnim"), animData);
		spriteAnimator->PlayAnimation(CreateId("BossAnim"));

		Bloodforge::TransformComponent* transform = entityManager.GetComponent<Bloodforge::TransformComponent>(bossEntityId);
		transform->SetLocalPosition(Bloodforge::WindowUtils::GetWindowSize() / 2.0f);
		transform->SetLocalScale({ 2.0f, 2.0f });
	}

	// MakeBackgroundPart(1);
	// MakeBackgroundPart(2);
	// MakeBackgroundPart(3);
	// MakeBackgroundPart(4);
	// MakeBackgroundPart(5);
}

int main(int, char* [])
{
	Bloodforge::Bloodforge& engine = Bloodforge::Bloodforge::GetInstance();
	engine.SetResourcesDirectory("Resources");

	Bloodforge::WindowUtils::SetWindowAlwaysOnTop(false);
	Bloodforge::WindowUtils::SetWindowFullScreen(true);
	Bloodforge::WindowUtils::SetWindowBordered(true);
	Bloodforge::WindowUtils::SetWindowSize(1920, 1080);
	Bloodforge::WindowUtils::SetWindowTitle("BossRushBulletHellSystem");

	SaveAllBosses();

	Bloodforge::EntityManager& entityManager = Bloodforge::EntityManager::GetInstance();
	Bloodforge::SceneSystemManager& sceneSystemManager = Bloodforge::SceneSystemManager::GetInstance();

	sceneSystemManager.TryRegisterSystem<BossAttackingSystem>();
	sceneSystemManager.TryRegisterSystem<BasicProjectileSystem>();
	sceneSystemManager.TryRegisterSystem<PlayerMovementSystem>();
	sceneSystemManager.TryRegisterSystem<FpsDisplaySystem>();

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