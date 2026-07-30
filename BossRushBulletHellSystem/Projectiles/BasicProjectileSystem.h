#pragma once
#include <System.h>

class BasicProjectileSystem final : public Bloodforge::System
{
public:
	virtual void OnUpdate() override;
};