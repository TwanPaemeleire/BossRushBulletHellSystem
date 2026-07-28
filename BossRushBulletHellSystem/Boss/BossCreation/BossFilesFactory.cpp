#include "BossFilesFactory.h"
#include <filesystem>
#include <FileSaveLoadUtils.h>

BossMakerHelper BossFilesFactory::StartBossCreation(BossId newBossId, const std::string& newBossName, BossLevel level)
{
    return BossMakerHelper(newBossId, newBossName, level);
}

void BossFilesFactory::FinishBossCreation(BossMakerHelper& bossMakerHelper)
{
    BossId bossId = bossMakerHelper.GetBossId();
    BossLevel bossLevel = bossMakerHelper.GetBossLevel();

    if (!m_BossesConfigCollectionsMap.contains(bossId))
    {
        m_BossesConfigCollectionsMap.insert({ bossId, {} });
    }
    m_BossesConfigCollectionsMap[bossId].BossConfigsMap.insert({ bossLevel, bossMakerHelper.GetBossConfig() });
    m_BossesConfigCollectionsMap[bossId].BossName = bossMakerHelper.GetBossName();
}

void BossFilesFactory::WriteAllToFiles()
{
    for (auto& [id, configCollection] : m_BossesConfigCollectionsMap)
    {
        std::filesystem::create_directories("Bosses/" + configCollection.BossName);
        for (auto& [level, config] : configCollection.BossConfigsMap)
        {
            Bloodforge::FileSaveLoadUtils::SaveFile<BossConfig>(config, "Bosses/" + configCollection.BossName + "/" + std::to_string(level) + ".json");
        }
    }
}

BossMakerHelper::BossMakerHelper(BossId bossId, const std::string& newBossName, BossLevel level)
{
    m_BossId = bossId;
    m_BossLevel = level;
    m_BossName = newBossName;
}

BossPhaseMakerHelper& BossMakerHelper::AddPhaseToBoss(float healthPercentageThreshold, bool canExecuteConsecutiveAttacks, bool keepPreviousPhaseActions)
{
    m_PhaseMakerHelpers.emplace_back(BossPhaseMakerHelper{healthPercentageThreshold, canExecuteConsecutiveAttacks, keepPreviousPhaseActions});
    return m_PhaseMakerHelpers[m_PhaseMakerHelpers.size() - 1];
}

BossConfig& BossMakerHelper::GetBossConfig()
{
    for (BossPhaseMakerHelper& phaseMakerHelper : m_PhaseMakerHelpers)
    {
        m_BossConfig.BossPhases.emplace_back(phaseMakerHelper.GetPhaseConfig());
    }
    return m_BossConfig;
}

BossPhaseActionMakerHelper::BossPhaseActionMakerHelper(int chanceWeight, BossActionId actionId)
{
    m_PhaseActionConfig.ChanceWeight = chanceWeight;
    m_PhaseActionConfig.Id = actionId;
}

void BossPhaseActionMakerHelper::AddValueToBlackboard(BlackboardKey key, std::any value)
{
    m_PhaseActionConfig.Blackboard.Set(key, value);
}

BossPhaseActionMakerHelper& BossPhaseMakerHelper::AddActionToBossPhase(int chanceWeight, BossActionId actionId)
{
    m_ActionMakerHelpers.emplace_back(BossPhaseActionMakerHelper(chanceWeight, actionId));
    return m_ActionMakerHelpers[m_ActionMakerHelpers.size() - 1];
}

BossPhaseConfig& BossPhaseMakerHelper::GetPhaseConfig()
{
    m_PhaseConfig.PossibleActions.clear();
    for (BossPhaseActionMakerHelper& actionMakerHelper : m_ActionMakerHelpers)
    {
        m_PhaseConfig.PossibleActions.emplace_back(actionMakerHelper.GetBossPhaseActionConfig());
    }
    return m_PhaseConfig;
}

BossPhaseMakerHelper::BossPhaseMakerHelper(float healthPercentageThreshold, bool canExecuteConsecutiveAttacks, bool keepPreviousPhaseActions)
{
    m_PhaseConfig.HealthPercentageThreshold = healthPercentageThreshold;
    m_PhaseConfig.CanExecutiveConsectiveActions = canExecuteConsecutiveAttacks;
    m_PhaseConfig.KeepPreviousPhasesActions = keepPreviousPhaseActions;
}