#pragma once
#include "Component.h"
#include <string>
#include <functional>

namespace Bloodforge
{
	struct SceneDataComponent final : public Component<SceneDataComponent>
	{
		std::string SceneName;
		std::function<void()> LoadFunction;
	};
}