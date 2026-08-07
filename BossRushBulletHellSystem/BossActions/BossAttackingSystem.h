#pragma once
#include <System.h>

class BossAttackingSystem final : public Bloodforge::System
{
public:
	virtual void OnUpdate() override;
private:
	int CreateNewBoss();
};