#pragma once
#include "System.h"
#include <isteamuserstats.h>

namespace Bloodforge
{
	class SteamSupportSystem final : public System
	{
	public:
		SteamSupportSystem();

		virtual void OnStart() override;
		virtual void OnUpdate() override;
	private:
		bool IsSteamInitialized();
		void CheckUnlockAchievementRequests();
		void CheckResetAllAchievementsRequests();
		void CheckIndicateAchievementProgressRequests();

		STEAM_CALLBACK(SteamSupportSystem, OnAchievementStored, UserAchievementStored_t, m_CallbackAchievementStored);
		STEAM_CALLBACK(SteamSupportSystem, OnUserStatsStored, UserStatsStored_t, m_CallbackUserStatsStored);
	};
}