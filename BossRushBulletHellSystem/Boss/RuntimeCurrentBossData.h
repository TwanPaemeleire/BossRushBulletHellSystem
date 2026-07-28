#pragma once
#include <Component.h>
#include <Utils/Blackboard.h>

using BossActionId = unsigned int;
using BossId = unsigned int;
using BossLevel = unsigned int;

struct RuntimeCurrentBossData final : public Bloodforge::Component<RuntimeCurrentBossData>
{
	Blackboard CurrentBlackboard;
	BossActionId CurrentBossActionId = 0;
	BossActionId PreviousBossActionId = 0;
	BossId CurrentBosId = 0;
	BossLevel CurrentBossLevel = 0;
};