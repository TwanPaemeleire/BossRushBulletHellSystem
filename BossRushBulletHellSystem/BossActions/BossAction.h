#pragma once
#include <Event.h>
#include <functional>

using BossActionId = unsigned int;

struct BossAction
{
	BossActionId ActionId;
	std::function<void()> OnStartFunction;
	std::function<void()> OnUpdateFunction;
	std::function<void()> OnFinishedFunction;
	std::function<void()> OnStoppedFunction;
};