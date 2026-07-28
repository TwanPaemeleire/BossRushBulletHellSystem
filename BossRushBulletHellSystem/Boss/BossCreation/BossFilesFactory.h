#pragma once
#include <unordered_map>
#include <any>
#include <Boss/BossConfigCollection.h>
#include <vector>

using BossId = unsigned int;
using BossActionId = unsigned int;
using BlackboardKey = unsigned int;
using BossLevel = unsigned int;
struct Blackboard;

class BossPhaseActionMakerHelper final
{
public:
	BossPhaseActionMakerHelper(int chanceWeight, BossActionId actionId);
	void AddValueToBlackboard(BlackboardKey key, std::any value);

	BossPhaseActionConfig& GetBossPhaseActionConfig() { return m_PhaseActionConfig; }
private:
	BossPhaseActionConfig m_PhaseActionConfig;
};

class BossPhaseMakerHelper final
{
public:
	BossPhaseMakerHelper(float healthPercentageThreshold, bool canExecuteConsecutiveAttacks, bool keepPreviousPhaseActions);
	BossPhaseActionMakerHelper& AddActionToBossPhase(int chanceWeight, BossActionId actionId);

	BossPhaseConfig& GetPhaseConfig();
private:
	BossPhaseConfig m_PhaseConfig;
	std::vector<BossPhaseActionMakerHelper> m_ActionMakerHelpers;
};

class BossMakerHelper final
{
public:
	BossMakerHelper(BossId bossId, const std::string& newBossName, BossLevel level);
	BossPhaseMakerHelper& AddPhaseToBoss(float healthPercentageThreshold, bool canExecuteConsecutiveAttacks = true, bool keepPreviousPhaseActions = false);
	BossConfig& GetBossConfig();
	BossId GetBossId() { return m_BossId; }
	BossLevel GetBossLevel() { return m_BossLevel; }
	const std::string& GetBossName() { return m_BossName; }
private:
	BossConfig m_BossConfig;
	BossId m_BossId;
	BossLevel m_BossLevel;
	std::string m_BossName;
	std::vector<BossPhaseMakerHelper> m_PhaseMakerHelpers;
};

class BossFilesFactory final
{
public:
	BossMakerHelper StartBossCreation(BossId newBossId, const std::string& newBossName, BossLevel level);
	void FinishBossCreation(BossMakerHelper& bossMakerHelper);
	void WriteAllToFiles();
private:
	std::unordered_map<BossId, BossConfigCollection> m_BossesConfigCollectionsMap;
};