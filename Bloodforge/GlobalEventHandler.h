#pragma once
#include "Singleton.h"
#include <unordered_map>
#include <vector>
#include <functional>
#include <typeindex>
#include <memory>

namespace Bloodforge
{
	struct IGlobalEventContainer
	{
	};

	template <typename EventType>
	class GlobalEventContainer final : public IGlobalEventContainer
	{
	public:
		int AddListener(const std::function<void(const EventType&)>& listener);
		void RemoveListener(int id);
		void Invoke(const EventType& eventData);

	private:
		struct ListenerEntry
		{
			int Id = -1;
			std::function<void(const EventType&)> Listener;
		};

		std::vector<ListenerEntry> m_Listeners;
		int m_NextListenerId = 0;
	};

	class GlobalEventHandler final : public Singleton<GlobalEventHandler>
	{
	public:
		void Cleanup();
		template <typename EventType>
		int AddListener(std::function<void(const EventType&)> listener);
		template <typename EventType>
		void RemoveListener(int id);
		template <typename EventType>
		void InvokeEvent(const EventType& eventData);
	private:

		template <typename EventType>
		GlobalEventContainer<EventType>& GetOrCreateEvent();
		std::unordered_map<std::type_index, std::unique_ptr<IGlobalEventContainer>> m_Events;
	};

	template<typename EventType>
	inline int GlobalEventHandler::AddListener(std::function<void(const EventType&)> listener)
	{
		auto& typedEvent = GetOrCreateEvent<EventType>();
		return typedEvent.AddListener(std::move(listener));
	}

	template<typename EventType>
	inline void GlobalEventHandler::RemoveListener(int id)
	{
		auto it = m_Events.find(std::type_index(typeid(EventType)));
		if (it == m_Events.end()) return;
		auto* typedEvent = static_cast<GlobalEventContainer<EventType>*>(it->second.get());
		return typedEvent->RemoveListener(id);
	}

	template<typename EventType>
	inline void GlobalEventHandler::InvokeEvent(const EventType& eventData)
	{
		auto it = m_Events.find(std::type_index(typeid(EventType)));
		if (it == m_Events.end()) return;
		auto* typedEvent = static_cast<GlobalEventContainer<EventType>*>(it->second.get());
		typedEvent->Invoke(eventData);
	}

	template<typename EventType>
	inline GlobalEventContainer<EventType>& GlobalEventHandler::GetOrCreateEvent()
	{
		const std::type_index type = std::type_index(typeid(EventType));
		auto it = m_Events.find(type);

		if (it == m_Events.end())
		{
			auto container = std::make_unique<GlobalEventContainer<EventType>>();
			GlobalEventContainer<EventType>* rawPtr = container.get();
			m_Events[type] = std::move(container);
			return *rawPtr;
		}

		return *static_cast<GlobalEventContainer<EventType>*>(it->second.get());
	}

	template<typename EventType>
	inline int GlobalEventContainer<EventType>::AddListener(const std::function<void(const EventType&)>& listener)
	{
		ListenerEntry entry;
		entry.Id = m_NextListenerId; 
		entry.Listener = listener;
		m_Listeners.emplace_back(entry);
		return m_NextListenerId++;
	}

	template<typename EventType>
	inline void GlobalEventContainer<EventType>::RemoveListener(int id)
	{
		m_Listeners.erase(std::remove_if(m_Listeners.begin(), m_Listeners.end(), [id](const ListenerEntry& entry)
			{
				return entry.Id == id;
			}),
			m_Listeners.end());
	}

	template<typename EventType>
	inline void GlobalEventContainer<EventType>::Invoke(const EventType& eventData)
	{
		for (const auto& listener : m_Listeners)
		{
			listener.Listener(eventData);
		}
	}
}