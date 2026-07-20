#pragma once
#include "Component.h"
#include <functional>

namespace Bloodforge
{
	struct FunctionInvokeComponent final : public Component<FunctionInvokeComponent>
	{
		std::function<void()> Function;
		float TimeToInvoke = 0.0f;
	};
}
