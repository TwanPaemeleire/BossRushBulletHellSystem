#pragma once
#include <Component.h>

struct BossHealth final : public Bloodforge::Component<BossHealth>
{
	float CurrentHealth = 100.0f;
	float MaxHealth = 100.0f;
	int NumberOfPhases = 1;
	int CurrentPhase = 1;

};