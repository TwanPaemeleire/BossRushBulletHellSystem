#include "BossAttackingSystem.h"
#include <EntityManager.h>
#include <Boss/RuntimeCurrentBossData.h>
#include <Boss/BossesDataBank.h>
#include "BossAction.h"

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
	}

	if (!currentBossData.CurrentActionFinishedTrigger) // Continue current action
	{
		dataBank.BossActionsMap[currentBossData.CurrentBossActionId].OnUpdateFunction();
	}
	else // Current action finished, select new
	{
		// IN FUTURE, ADD SUPPORT HERE FOR CHECKING FOR REPEATING THE SAME ACTION TWICE IN A ROW, ETC.
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
			}
		}
	}

	
}