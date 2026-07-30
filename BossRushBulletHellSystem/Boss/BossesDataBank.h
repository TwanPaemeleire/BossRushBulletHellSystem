#pragma once
#include <Component.h>
#include <unordered_map>
#include "BossConfigCollection.h"
#include <BossActions/BossAction.h>

using BossId = unsigned int;

struct BossesDataBank final : public Bloodforge::Component<BossesDataBank>
{
	BossesDataBank();
	std::unordered_map<BossId, BossConfigCollection> BossesConfigCollectionsMap;
	std::unordered_map<BossActionId, BossAction> BossActionsMap;
};