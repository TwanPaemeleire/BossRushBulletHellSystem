#pragma once
#include "Component.h"
#include <functional>
#include <vector>

namespace Bloodforge
{
	struct WaitAction
	{
		float Duration = 0.0f;
	};

	struct FunctionAction
	{
		std::function<void()> Function;
	};

	struct ActionQueueComponent final : public Component<ActionQueueComponent>
	{
		std::vector<std::function<void()>> Actions;
		int CurrentActionIndex = 0;
		bool IsPlaying = false;
		bool ShouldLoop = false;
		bool DestroyEntityOnFinish = false;
		
		void AddWaitAction(float duration)
		{
			Actions.push_back([this, duration]()
				{
					WaitCounter += duration;
				});
		}

		void AddFunctionAction(std::function<void()> function)
		{
			Actions.push_back(function);
		}

		void Restart()
		{
			CurrentActionIndex = 0;
			WaitCounter = 0.0f;
			IsWaiting = false;
			IsPlaying = true;
		}

		// Internal usage
		float WaitCounter = 0.0f;
		bool IsWaiting = false;
	};
}