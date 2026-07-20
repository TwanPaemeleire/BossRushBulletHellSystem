#pragma once
#include "System.h"

namespace Bloodforge
{
	class ActionQueueSystem final : public System
	{
	public:
		virtual void OnUpdate() override;
	};
}