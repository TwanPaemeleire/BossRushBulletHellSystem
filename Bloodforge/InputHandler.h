#pragma once
#include "Singleton.h"
#include <unordered_map>
#include <memory>
#include <vector>
#include <functional>
#include "Vector2.h"
#include "KeyDefs.h"

namespace Bloodforge 
{
	using MapId = unsigned int;
	using ActionId = unsigned int;
	using ButtonId = unsigned int;
	struct InputActionInfo;
	using ListenerSignature = std::function<void(const InputActionInfo& info)>;
	struct InputActionInfo
	{
		bool started = false; // Pressing a button/key
		bool finished = false; // Releasing a button/key
		bool ongoing = false; // Mainly used for mouse motion
		Vector2 InputActionVectorData; // Used for xy position of mouse
		Vector2 InputActionVectorDeltaData; // Used for xy motion of mouse
	};

	struct InputAction
	{
		std::vector<ListenerSignature> Listeners;
		ButtonId Button = 0;
		bool WasDownLastFrame = false;
	};

	// Notes for using the input handling:
	// Key actions will trigger twice, once when pressing the key, and once when releasing the key
	// Mouse motion actions will trigger every frame the mouse moves
	// Mouse button actions will trigger when pressing and releasing the specified mouse button, but also every frame with mouse motion while the button is held down

	class InputHandler final :public Singleton<InputHandler>
	{
	public:
		void Cleanup();
		void ProcessInput();

		void CreateMap(MapId mapId);
		void CreateAction(ActionId actionId, MapId mapId, ButtonId buttonId);
		void SetCurrentMap(MapId mapId);
		void AddListenerToInputAction(ActionId actionId, MapId mapId, const ListenerSignature& listener);
		void RemoveAllListeners();
	private:
		std::unordered_map<MapId, std::unordered_map<ActionId, std::unique_ptr<InputAction>>> m_InputActionMaps;
		MapId m_CurrentMapId = 0;
	};
}