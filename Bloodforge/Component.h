#pragma once

namespace Bloodforge
{
	struct Entity;
	template <typename T>
	struct Component
	{
		inline static int Index = -1;
		int OwnerEntityId = -1;
		bool IsEnabled = true;
		Component() = default;
	};
}