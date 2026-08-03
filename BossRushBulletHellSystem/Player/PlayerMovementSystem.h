#pragma once
#include <System.h>

namespace Bloodforge
{
	class InputActionInfo;
}

class PlayerMovementSystem final : public Bloodforge::System
{
public:
	virtual void OnStart() override;

private:
	void ProcessPlayerMovementInput(const Bloodforge::InputActionInfo& info);
};