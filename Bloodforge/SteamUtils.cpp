#include "pch.h"
#include "SteamUtils.h"
#include <isteamuserstats.h>
#include "EntityManager.h"
#include "SteamStateComponent.h"

bool Bloodforge::SteamUtils::SteamIsInitialized()
{
    std::optional<EntityView<SteamStateComponent>> view = EntityManager::GetInstance().GetFirstEntityWithComponents<SteamStateComponent>();
    if (view)
    {
        return view->GetComponent<SteamStateComponent>().SteamIsInitialized;
    }
    return false;
}

bool Bloodforge::SteamUtils::GetStat(const char* statName, int* statValue)
{
    if (!SteamIsInitialized()) return false;
    return SteamUserStats()->GetStat(statName, statValue);
}

bool Bloodforge::SteamUtils::GetStat(const char* statName, float* statValue)
{
    if (!SteamIsInitialized()) return false;
    return SteamUserStats()->GetStat(statName, statValue);
}

bool Bloodforge::SteamUtils::SetStat(const char* statName, int statValue)
{
    if (!SteamIsInitialized()) return false;
    return SteamUserStats()->SetStat(statName, statValue);
}

bool Bloodforge::SteamUtils::SetStat(const char* statName, float statValue)
{
    if (!SteamIsInitialized()) return false;
    return SteamUserStats()->SetStat(statName, statValue);
}

bool Bloodforge::SteamUtils::HasAchievementBeenUnlocked(const char* statName)
{
    if (!SteamIsInitialized()) return false;
    bool achieved = false;
    SteamUserStats()->GetAchievement(statName, &achieved);
    return achieved;
}
