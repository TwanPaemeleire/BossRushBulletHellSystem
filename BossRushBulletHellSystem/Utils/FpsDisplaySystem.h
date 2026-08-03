#pragma once
#include <System.h>
class FpsDisplaySystem final : public Bloodforge::System
{
public:
	virtual void OnUpdate() override;
};