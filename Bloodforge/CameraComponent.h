#pragma once
#include "Component.h"

namespace Bloodforge
{
	struct CameraComponent final : public Component<CameraComponent>
	{
		int EntityIdToFollow = -1;
	};
}