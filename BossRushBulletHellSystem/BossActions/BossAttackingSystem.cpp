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
	if(!currentBossData.BossFightOngoing) return;

	BossesDataBank& dataBank = entityManager.GetFirstEntityWithComponents<BossesDataBank>().value().GetComponent<BossesDataBank>();
	dataBank.BossActionsMap[currentBossData.CurrentBossActionId].OnUpdateFunction();
}