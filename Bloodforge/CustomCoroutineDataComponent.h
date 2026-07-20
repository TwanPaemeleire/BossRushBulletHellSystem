#pragma once
#include "Component.h"
#include "CustomCoroutine.h"

namespace Bloodforge
{
	struct CustomCoroutineDataComponent final : Component<CustomCoroutineDataComponent>
	{
		Coroutine CoroutineHandle;

		bool IsWaitingForNextFrame = false;
		float WaitTimeCounter = 0.0f;
	};
}