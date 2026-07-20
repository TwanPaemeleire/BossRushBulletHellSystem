#include "pch.h"
#include "InputHandler.h"
#include <SDL3/SDL.h>
#include "Bloodforge.h"
#include <stdexcept>
#include <iostream>

void Bloodforge::InputHandler::Cleanup()
{
	m_InputActionMaps.clear();
}

void Bloodforge::InputHandler::ProcessInput()
{
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_EVENT_QUIT)
		{
			Bloodforge::GetInstance().RequestStop();
		}
		bool keyEvent = e.type == SDL_EVENT_KEY_UP || e.type == SDL_EVENT_KEY_DOWN;
		bool mouseEvent = e.type == SDL_EVENT_MOUSE_BUTTON_UP || e.type == SDL_EVENT_MOUSE_BUTTON_DOWN;
		bool mouseMotionEvent = e.type == SDL_EVENT_MOUSE_MOTION;
		if (keyEvent || mouseEvent || mouseMotionEvent)
		{
			for (auto& [key, value] : m_InputActionMaps[m_CurrentMapId])
			{
				InputActionInfo info;
				if (keyEvent) 
				{
					if (value->Button != e.key.key) continue;
					info.started = (e.type == SDL_EVENT_KEY_DOWN) && (!value->WasDownLastFrame);
					value->WasDownLastFrame = e.type == SDL_EVENT_KEY_DOWN;
					info.ongoing = e.type == SDL_EVENT_KEY_DOWN;
					info.finished = e.type == SDL_EVENT_KEY_UP;
				}
				if (mouseEvent)
				{
					if (value->Button != e.button.button) continue;
					info.InputActionVectorData = { e.button.x, e.button.y };
					info.started = e.type == SDL_EVENT_MOUSE_BUTTON_DOWN && (!value->WasDownLastFrame);
					value->WasDownLastFrame = e.type == SDL_EVENT_MOUSE_BUTTON_DOWN;
					info.finished = e.type == SDL_EVENT_MOUSE_BUTTON_UP;
				}
				if (mouseMotionEvent)
				{
					if (value->Button != BLOODFORGE_KEYCODE_MOUSE_MOTION && !(e.motion.state & SDL_BUTTON_MASK(value->Button))) continue;
					info.InputActionVectorData = { e.motion.x, e.motion.y };
					info.InputActionVectorDeltaData = { e.motion.xrel, e.motion.yrel };
					info.ongoing = true;
				}
				for (const auto& listener : value->Listeners)
				{
					listener(info);
				}
			}
		}
	}
}

void Bloodforge::InputHandler::CreateMap(MapId mapId)
{
	if (m_InputActionMaps.contains(mapId)) return;
	m_InputActionMaps.emplace(mapId, std::unordered_map<ActionId, std::unique_ptr<InputAction>>{});
}

void Bloodforge::InputHandler::CreateAction(ActionId actionId, MapId mapId, ButtonId buttonId)
{
	if (!m_InputActionMaps.contains(mapId)) CreateMap(mapId);
	if (m_InputActionMaps[mapId].contains(actionId)) throw std::runtime_error("Trying to create an action in an input action map that already has an action with this id.");
	std::unique_ptr<InputAction> inputAction = std::make_unique<InputAction>();
	inputAction->Button = buttonId;
	m_InputActionMaps[mapId][actionId] = std::move(inputAction);
}

void Bloodforge::InputHandler::SetCurrentMap(MapId mapId)
{
	if (!m_InputActionMaps.contains(mapId)) throw std::runtime_error("Trying to set an input action map that doesn't exist.");
	m_CurrentMapId = mapId;
}

void Bloodforge::InputHandler::AddListenerToInputAction(ActionId actionId, MapId mapId, const ListenerSignature& listener)
{
	if (!m_InputActionMaps.contains(mapId)) throw std::runtime_error("Trying to access an input action map that doesn't exist.");
	if (!m_InputActionMaps[mapId].contains(actionId)) throw std::runtime_error("Trying to access an action that doesn't exist in this input action map.");

	m_InputActionMaps[mapId][actionId]->Listeners.emplace_back(listener);
}

void Bloodforge::InputHandler::RemoveAllListeners()
{
	for (auto& [key, value] : m_InputActionMaps[m_CurrentMapId])
	{
		value->Listeners.clear();
	}
}