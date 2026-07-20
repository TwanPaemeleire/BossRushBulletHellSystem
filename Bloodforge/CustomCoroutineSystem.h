#pragma once
#include "System.h"

namespace Bloodforge
{
	class CustomCoroutineSystem final : public System
	{
	public:
		virtual void OnUpdate() override;
	};
}
