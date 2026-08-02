#pragma once
#include "Component.h"
#include "Event.h"
#include <unordered_set>
#include "Vector2.h"

using TagId = unsigned int;

namespace Bloodforge
{
	struct CircleColliderComponent final : public Component<CircleColliderComponent>
	{
		float Radius = 0.0f;

		Event<int, int> OnCollisionEnterEvent;
		Event<int, int> OnCollisionEvent;
		Event<int, int> OnCollisionExitEvent;

		std::unordered_set<TagId> IgnoreTags;
	};
}