#pragma once
#include <unordered_map>
#include "BossConfig.h"

using BossLevel = unsigned int;

struct BossConfigCollection
{
	std::unordered_map<BossLevel, Bossconfig> BossConfigsMap;
};