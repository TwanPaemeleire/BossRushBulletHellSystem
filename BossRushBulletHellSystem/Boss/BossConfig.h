#pragma once
#include <vector>
#include <Utils/Blackboard.h>

using BossActionId = unsigned int;

struct BossPhaseActionConfig final
{
	int ChanceWeight = 1;
	BossActionId Id;
	Blackboard Blackboard;
};

struct Bossconfig final
{
	std::vector<std::vector<BossPhaseActionConfig>> PossibleActionsPerPhaseIds;
	std::vector<bool> CanExecuteConsecutivePerPhase; // List of bools indicating whether the boss can execute the same action twice in a row in each phase
	std::vector<bool> KeepPreviousPhasesActionsPerPhase; // List of bools indicating whether the boss can keep actions from previous phases when entering a new phase
	std::vector<float> HealthPercentageThresholdPerPhase; // List of health percentage thresholds for each phase (amount of phases = amount of thresholds + 1)
};