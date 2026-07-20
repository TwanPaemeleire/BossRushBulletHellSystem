#pragma once
#include "Component.h"
#include <unordered_map>
#include <string>
#include "Event.h"

namespace Bloodforge
{
	struct SceneManagingDataComponent final : public Component<SceneManagingDataComponent>
	{
		int CurrentSceneDataEntityId = -1;
		int SceneToLoadDataEntityId = -1;
		bool ShouldLoadScene = false;
		// std::unordered_map<std::string, int> SceneNameToSceneDataEntityId;
		Event<int, int> OnSceneSwitch; // Arguments are old scene data entity id and new scene data entity id
	};
}