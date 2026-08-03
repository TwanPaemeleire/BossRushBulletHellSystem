#pragma once
#include <System.h>

namespace Bloodforge
{
	struct InputActionInfo;
	struct Coroutine;
}

class PlayerMovementSystem final : public Bloodforge::System
{
public:
	virtual void OnStart() override;
	virtual void OnUpdate() override;

private:
	void ProcessPlayerMovementInput(const Bloodforge::InputActionInfo& info);

	Bloodforge::Coroutine ChangeSpeedGradually(bool accelerate);
};