#pragma once
#include "Component.h"
#include <unordered_map>

namespace Bloodforge
{
	struct CustomCoroutinesMapHolderComponent final : public Component<CustomCoroutinesMapHolderComponent>
	{
		std::unordered_map<void*, int> AddressToEntityId;
	};
}