#pragma once
#include <stdint.h>

namespace Bloodforge::SteamUtils
{
	bool SteamIsInitialized();
	bool GetStat(const char* statName, int* statValue);
	bool GetStat(const char* statName, float* statValue);
	bool SetStat(const char* statName, int statValue);
	bool SetStat(const char* statName, float statValue);
	bool HasAchievementBeenUnlocked(const char* statName);
}