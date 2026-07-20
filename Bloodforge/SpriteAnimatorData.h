#pragma once
#include "Component.h"
#include <functional>
#include <unordered_map>

namespace Bloodforge
{
	struct SpriteAnimatorComponent;

	struct SpriteAnimatorData final : public Component<SpriteAnimatorData>
	{
		std::unordered_map<int, std::function<void(SpriteAnimatorComponent&)>> AnimationEventsCallbacks;
		int NextAnimationEventCallbackID = 0;
	};
}