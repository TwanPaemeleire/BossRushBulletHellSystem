#pragma once
#include "Component.h"
#include <functional>
#include <string>

namespace Bloodforge
{
	struct SteamUnlockAchievementRequest : public Component<SteamUnlockAchievementRequest>
	{
		std::string AchievementId;
	};
}