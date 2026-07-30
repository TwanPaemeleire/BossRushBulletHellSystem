#pragma once
#include <vector>
#include <Utils/Blackboard.h>
#include <json.hpp>

using BossActionId = unsigned int;

struct BossPhaseActionConfig final
{
	int ChanceWeight = 1;
	BossActionId Id = 0;
	Blackboard Blackboard;
};

inline void to_json(nlohmann::json& j, const BossPhaseActionConfig& p)
{
	j = nlohmann::json{
		{ "ChanceWeight", p.ChanceWeight },
		{ "Id", p.Id },
		{"Blackboard", p.Blackboard}
	};
}

inline void from_json(const nlohmann::json& j, BossPhaseActionConfig& p)
{
	j.at("ChanceWeight").get_to(p.ChanceWeight);
	j.at("Id").get_to(p.Id);
	j.at("Blackboard").get_to(p.Blackboard);
}

struct BossPhaseConfig final
{
	std::vector<BossPhaseActionConfig> PossibleActions;
	bool CanExecutiveConsectiveActions = true;
	bool KeepPreviousPhasesActions = false;
	float HealthPercentageThreshold = 1.0f;
	int WeightSum = 0;
};

inline void to_json(nlohmann::json& j, const BossPhaseConfig& p)
{
	j = nlohmann::json{
		{ "PossibleActions", p.PossibleActions },
		{ "CanExecutiveConsectiveActions", p.CanExecutiveConsectiveActions },
		{ "KeepPreviousPhasesActions", p.KeepPreviousPhasesActions },
		{ "HealthPercentageThreshold", p.HealthPercentageThreshold },
		{"WeightSum", p.WeightSum}
	};
}

inline void from_json(const nlohmann::json& j, BossPhaseConfig& p)
{
	j.at("PossibleActions").get_to(p.PossibleActions);
	j.at("CanExecutiveConsectiveActions").get_to(p.CanExecutiveConsectiveActions);
	j.at("KeepPreviousPhasesActions").get_to(p.KeepPreviousPhasesActions);
	j.at("HealthPercentageThreshold").get_to(p.HealthPercentageThreshold);
	j.at("WeightSum").get_to(p.WeightSum);
}

struct BossConfig final
{
	std::vector<BossPhaseConfig> BossPhases;
};

inline void to_json(nlohmann::json& j, const BossConfig& p)
{
	j = nlohmann::json{
		{ "BossPhases", p.BossPhases }
	};
}

inline void from_json(const nlohmann::json& j, BossConfig& p)
{
	j.at("BossPhases").get_to(p.BossPhases);
}