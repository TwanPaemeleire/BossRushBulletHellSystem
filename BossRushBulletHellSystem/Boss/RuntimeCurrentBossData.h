#pragma once
#include <Component.h>
#include "BossConfig.h"

using BossActionId = unsigned int;
using BossId = unsigned int;
using BossLevel = unsigned int;

struct RuntimeCurrentBossData final : public Bloodforge::Component<RuntimeCurrentBossData>
{
	BossConfig CurrentBossConfig;
	BossActionId CurrentBossActionId = 0;
	BossActionId PreviousBossActionId = 0;
	BossId CurrentBosId = 0;
	BossLevel CurrentBossLevel = 0;
	bool BossFightOngoing = false;
	int CurrentBossParentEntityId = -1;
	int CurrentPhase = 1;
	int CurrentActionIndex = -1;
	bool CurrentActionFinishedTrigger = false;
	Blackboard& GetCurrentBlackboard() { return CurrentBossConfig.BossPhases[CurrentPhase - 1].PossibleActions[CurrentActionIndex].Blackboard; }
};