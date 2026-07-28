#pragma once
#include <unordered_map>
#include <string>
#include "BossConfig.h"

using BossLevel = unsigned int;

struct BossConfigCollection
{
	std::unordered_map<BossLevel, BossConfig> BossConfigsMap;
	std::string BossName;
};

inline void to_json(nlohmann::json& j, const BossConfigCollection& p)
{
    j = nlohmann::json{
        { "BossConfigsMap", p.BossConfigsMap },
        { "BossName", p.BossName }
    };
}

inline void from_json(const nlohmann::json& j, BossConfigCollection& p)
{
    j.at("BossConfigsMap").get_to(p.BossConfigsMap);
    j.at("BossName").get_to(p.BossName);
}