#pragma once
#include "Component.h"
#include <functional>
#include <string>

namespace Bloodforge
{
	struct SteamResetUserDataRequest : public Component<SteamResetUserDataRequest>
	{
		bool AlsoResetAchievements = true;
	};
}