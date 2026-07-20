#pragma once
#include "System.h"

namespace Bloodforge
{
	class FunctionInvokeSystem final : public System
	{
	public:
		virtual void OnUpdate() override;
	};
}