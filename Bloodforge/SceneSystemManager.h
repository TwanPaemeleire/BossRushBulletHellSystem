#pragma once
#include "Singleton.h"
#include <vector>
#include <unordered_map>
#include <memory>
#include <typeindex>
#include "System.h"

namespace Bloodforge
{
	template <typename SystemType>
	concept RegisterableSystem = std::is_base_of<System, SystemType>::value;

	class SceneSystemManager final : public Singleton<SceneSystemManager>
	{
	public:

		void Cleanup();

		template <RegisterableSystem SystemType>
		bool TryRegisterSystem();
		template <RegisterableSystem SystemType>
		SystemType* GetSystem();

		void Start();
		void Update();
		void FixedUpdate();
		void LateUpdate();
		void Render() const;
		void RenderUI();

		void DisableAllSystems();
		void EnableAllSystems();

	private:
		friend class Singleton<SceneSystemManager>;
		SceneSystemManager();

		void SetCurrentScene();
		bool ShouldSwitchScenes();

		std::vector<std::unique_ptr<System>> m_RegisteredSystems;
		std::unordered_map<std::type_index, int> m_SystemMap;
	};

	template<RegisterableSystem SystemType>
	inline bool SceneSystemManager::TryRegisterSystem()
	{
		std::type_index typeIndex = std::type_index(typeid(SystemType));
		if (!m_SystemMap.contains(typeIndex))
		{
			m_RegisteredSystems.emplace_back(std::make_unique<SystemType>());
			m_SystemMap.insert({ typeIndex, static_cast<int>(m_RegisteredSystems.size()) - 1 });
			return true;
		}
		return false;
	}

	template<RegisterableSystem SystemType>
	inline SystemType* SceneSystemManager::GetSystem()
	{
		std::type_index typeIndex = std::type_index(typeid(SystemType));
		if (m_SystemMap.contains(typeIndex))
		{
			return static_cast<SystemType*>(m_RegisteredSystems[m_SystemMap[typeIndex]].get());
		}
		m_RegisteredSystems.emplace_back(std::make_unique<SystemType>());
		m_SystemMap.insert({ typeIndex, static_cast<int>(m_RegisteredSystems.size()) - 1 });
		return static_cast<SystemType*>(m_RegisteredSystems[m_SystemMap[typeIndex]].get());
	}
}