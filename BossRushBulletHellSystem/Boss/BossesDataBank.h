#pragma once
#include <Component.h>
#include <unordered_map>
#include "BossConfigCollection.h"

using BossId = unsigned int;
struct BossAction;

struct BossesDataBank final : public Bloodforge::Component<BossesDataBank>
{
	std::unordered_map<BossId, BossConfigCollection> BossesConfigCollectionsMap;
	std::unordered_map<BossActionId, BossAction> BossActionsMap;
};