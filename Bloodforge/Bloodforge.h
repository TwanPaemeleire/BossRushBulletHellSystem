#pragma once
#include "Singleton.h"
#include <string>

namespace Bloodforge
{
	class Bloodforge : public Singleton<Bloodforge>
	{
	public:
		~Bloodforge() = default;
		void Run();

		Bloodforge(const Bloodforge& other) = delete;
		Bloodforge(Bloodforge&& other) = delete;
		Bloodforge& operator=(const Bloodforge& other) = delete;
		Bloodforge& operator=(Bloodforge&& other) = delete;

		void RequestStop();
		void SetResourcesDirectory(const std::string& path) const;

	private:
		const float m_FixedTimeStep{ 1.f / 60.f };
		const int m_MsPerFrame{ 1000 / 144 };
		bool m_ShouldContinue = true;

		friend class Singleton<Bloodforge>;
		explicit Bloodforge() = default;
	};
}