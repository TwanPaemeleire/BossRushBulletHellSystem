#pragma once
#include "Singleton.h"

namespace Bloodforge
{
	class BloodTime final : public Singleton<BloodTime>
	{
	public:
		float DeltaTime{};
		float UnscaledDeltaTime{};
		float TimeScale{ 1.0f };

		void SetDeltaTime(float deltaTime)
		{
			DeltaTime = deltaTime * TimeScale;
			UnscaledDeltaTime = deltaTime;
		}

	private:
		friend class Singleton<BloodTime>;
		BloodTime() = default;
	};
}