#pragma once
#include "Component.h"

namespace Bloodforge
{
	struct SteamStateComponent final : public Component<SteamStateComponent>
	{
		bool SteamIsInitialized = false;
	};
}