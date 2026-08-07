#include "BossAttackingSystem.h"
#include <EntityManager.h>
#include <Boss/RuntimeCurrentBossData.h>
#include <Boss/BossesDataBank.h>
#include "BossAction.h"
#include <SpriteComponent.h>
#include <SpriteAnimatorComponent.h>
#include <Boss/BossHealth.h>
#include <ResourceManager.h>
#include <IdCreator.h>
#include <WindowUtils.h>

using namespace Bloodforge;

void BossAttackingSystem::OnUpdate()
{
	EntityManager& entityManager = EntityManager::GetInstance();

	RuntimeCurrentBossData& currentBossData = entityManager.GetFirstEntityWithComponents<RuntimeCurrentBossData>().value().GetComponent<RuntimeCurrentBossData>();
	BossesDataBank& dataBank = entityManager.GetFirstEntityWithComponents<BossesDataBank>().value().GetComponent<BossesDataBank>();
	if(!currentBossData.BossFightOngoing && !currentBossData.StartBossFightTrigger) return;

	if (currentBossData.StartBossFightTrigger) // Start boss fight
	{
		currentBossData.BossFightOngoing = true;
		currentBossData.CurrentActionFinishedTrigger = true;
		currentBossData.CurrentActionIndex = 0;
		currentBossData.CurrentBossActionId = 0;
		currentBossData.CurrentPhase = 1;
		currentBossData.StartBossFightTrigger = false;

		currentBossData.CurrentBossParentEntityId = CreateNewBoss();
	}

	if (!currentBossData.CurrentActionFinishedTrigger) // Continue current action
	{
		dataBank.BossActionsMap[currentBossData.CurrentBossActionId].OnUpdateFunction();
	}
	else // Current action finished, select new
	{
		// IN FUTURE, ADD SUPPORT FOR CHECKING FOR REPEATING THE SAME ACTION TWICE IN A ROW, ETC.

		if (currentBossData.CurrentBossActionId != 0)
		{
			dataBank.BossActionsMap[currentBossData.CurrentBossActionId].OnFinishedFunction();
		}
		currentBossData.CurrentActionFinishedTrigger = false;

		int counter = 0;
		int randomNumber = (std::rand() % currentBossData.GetCurrentPhaseConfig().WeightSum) + 1;
		for (int index = 0; index < currentBossData.GetCurrentPhaseConfig().PossibleActions.size(); ++index)
		{
			BossPhaseActionConfig& phaseActionConfig = currentBossData.GetCurrentPhaseConfig().PossibleActions[index];
			counter += phaseActionConfig.ChanceWeight;
			if (randomNumber <= counter) // This action was picked
			{
				currentBossData.PreviousBossActionId = currentBossData.CurrentBossActionId;
				currentBossData.CurrentActionIndex = index;
				currentBossData.CurrentBossActionId = phaseActionConfig.Id;
				dataBank.BossActionsMap[currentBossData.CurrentBossActionId].OnStartFunction();
				dataBank.BossActionsMap[currentBossData.CurrentBossActionId].OnUpdateFunction();
				break;
			}
		}
	}

	
}

int BossAttackingSystem::CreateNewBoss()
{
	EntityManager& entityManager = EntityManager::GetInstance();
	Entity& bossEntity = entityManager.CreateEntity();
	int bossEntityId = bossEntity.Id;
	{
		SpriteComponent* spriteComp = entityManager.AddComponent<SpriteComponent>(bossEntityId);
		spriteComp->DrawOrder = 5;

		SpriteAnimatorComponent* spriteAnimator = entityManager.AddComponent<SpriteAnimatorComponent>(bossEntityId);
		Bloodforge::AnimationData animData;
		animData.Texture = Bloodforge::ResourceManager::GetInstance().LoadTexture("TempFirstBoss.png");
		animData.NumberOfFrames = 12;
		animData.FrameTime = 0.08f;
		spriteAnimator->AddAnimation(CreateId("BossAnim"), animData);
		spriteAnimator->PlayAnimation(CreateId("BossAnim"));

		entityManager.AddComponent<BossHealth>(bossEntityId);

		Bloodforge::TransformComponent* transform = entityManager.GetComponent<Bloodforge::TransformComponent>(bossEntityId);
		transform->SetLocalPosition(Bloodforge::WindowUtils::GetWindowSize() / 2.0f);
		transform->SetLocalScale({ 2.0f, 2.0f });

	}
	return bossEntityId;
}
