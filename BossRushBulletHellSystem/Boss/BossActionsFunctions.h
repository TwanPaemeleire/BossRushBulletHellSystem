#pragma once
#include <EntityManager.h>
#include "RuntimeCurrentBossData.h"

using namespace Bloodforge;

namespace BossActionsFunctions
{
	void TestFunction()
	{
		EntityManager& entityManager = EntityManager::GetInstance();

		RuntimeCurrentBossData& runtimeBossData = entityManager.GetOrCreateFirstEntityWithComponents<RuntimeCurrentBossData>().GetComponent<RuntimeCurrentBossData>();

	}
}