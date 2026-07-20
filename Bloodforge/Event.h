#pragma once
#include <functional>
#include <vector>

namespace Bloodforge
{
	template <typename... Arguments>
	class Event
	{
	public:
		int AddListener(std::function<void(Arguments...)> listener);
		void RemoveListener(int id);
		void Invoke(Arguments... arguments);
	private:
		struct ListenerEntry
		{
			int Id = -1;
			std::function<void(Arguments...)> Listener;
		};
		std::vector<ListenerEntry> m_Listeners;
		int m_NextListenerId = 0;
	};

	template<typename ...Arguments>
	inline int Event<Arguments...>::AddListener(std::function<void(Arguments...)> listener)
	{
		ListenerEntry entry;
		entry.Id = m_NextListenerId;
		entry.Listener = listener;
		m_Listeners.emplace_back(entry);
		return m_NextListenerId++;
	}

	template<typename ...Arguments>
	inline void Event<Arguments...>::RemoveListener(int id)
	{
		m_Listeners.erase(std::remove_if(m_Listeners.begin(), m_Listeners.end(), [id](const ListenerEntry& entry)
				{
					return entry.Id == id;
				}),
				m_Listeners.end());
	}

	template<typename ...Arguments>
	inline void Event<Arguments...>::Invoke(Arguments... arguments)
	{
		for (const auto& listener : m_Listeners)
		{
			listener.Listener(arguments...);
		}
	}
}