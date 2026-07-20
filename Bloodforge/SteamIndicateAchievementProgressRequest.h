#pragma once
#include "Component.h"
#include <string>

// Notes:
// Setting current progress makes no difference if the achievement is linked to a stat in the SteamWorks environment
// 
// Setting max progress will make a difference either way, namely, it makes it so that no progress indicators will trigger if the CurrentProgress (or stat if linked) is higher than the set max progress in the component

namespace Bloodforge
{
	struct SteamIndicateAchievementProgressRequest final : public Component< SteamIndicateAchievementProgressRequest>
	{
		std::string AchievementId;
		int CurrentProgress;
		int MaxProgess;
	};
}