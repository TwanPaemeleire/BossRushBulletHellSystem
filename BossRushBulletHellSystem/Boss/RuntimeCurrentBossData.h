#pragma once
#include <Component.h>

struct RuntimeCurrentBossData final : public Bloodforge::Component<RuntimeCurrentBossData>
{
	// Runtime events
	Blackboard CurrentBlackboard;
	BossActionId CurrentBossActionId;
	BossActionId PreviousBossActionId = -1;
	BossId CurrentBosId;
	BossLevel CurrentBossLevel;
};