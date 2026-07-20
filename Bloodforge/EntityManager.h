#pragma once
#include "Component.h"
#include <vector>
#include <typeindex>
#include <unordered_map>
#include "EntityChunk.h"
#include <string>
#include <algorithm>
#include "Entity.h"
#include <span>
#include "ComponentRegistry.h"
#include "ArchetypeIdentifierMask.h"
#include "TransformComponent.h"
#include "Singleton.h"
#include <optional>

using TagId = unsigned int;

namespace Bloodforge
{
	struct Entity;

	template <typename... Components>
	struct EntityView
	{
		int EntityId;
		std::tuple<Components&...> Components;

		template <typename ComponentType>
		ComponentType& GetComponent()
		{
			return std::get<ComponentType&>(Components);
		}
	};

	template <typename... Components>
	struct EntityQueryResult
	{
		std::vector<EntityView<Components...>> EntityViews;
	};

	class EntityManager : public Singleton<EntityManager>
	{
	public:
		~EntityManager();

		Entity& CreateEntity(int newChunkCapacity = 20);

		void DestroyEntity(int entityId);
		void DestroyEntity(const std::vector<int>& entityIds);
		void DestroyEntity(Entity& entity);

		Entity& GetEntity(int entityId) { return m_Entities.at(entityId); }
		ComponentRegistry* GetComponentRegistry() const { return m_ComponentRegistry.get(); }

		template <typename... Components>
		EntityQueryResult<Components...> QueryEntities(bool includeNonActive = false, bool includeWhenComponentDisabled = false);

		template <typename... Components>
		std::optional<EntityView<Components...>> GetFirstEntityWithComponents(bool includeNonActive = false, bool includeWhenComponentDisabled = false);

		template <typename... Components>
		EntityView<Components...> GetOrCreateFirstEntityWithComponents(bool includeNonActive = false, bool includeWhenComponentDisabled = false);

		template<typename ComponentType>
		ComponentType* AddComponent(Entity& entity);
		template<typename ComponentType>
		ComponentType* AddComponent(int entityId);
		//template<typename ComponentType>
		//void RemoveComponent(Entity& entity);
		//template<typename ComponentType>
		//void RemoveComponent(int entityId);
		template <typename ComponentType>
		ComponentType* GetComponent(Entity& entity);
		template <typename ComponentType>
		ComponentType* GetComponent(int entityId);
		template <typename ComponentType>
		ComponentType* GetComponentInChildren(Entity& entity);
		template <typename ComponentType>
		ComponentType* GetComponentInChildren(int entityId);
		template <typename ComponentType>
		ComponentType* GetComponentInParent(Entity& entity);
		template <typename ComponentType>
		ComponentType* GetComponentInParent(int entityId);
		template <typename ComponentType>
		bool HasComponent(int entityId);
		template <typename ComponentType>
		bool HasComponent(Entity& entity);

		void DestroyAllEntities();
		void DestroyEntitiesOnSceneSwitch();
		void DestroyMarkedForDestructionEntities();
	private:

		template <typename... ComponentTypes>
		ArchetypeIdentifierMask GetArchetypeIds();

		EntityChunk* GetFirstAvailableChunk(ArchetypeIdentifierMask& id, int capacity);
		EntityChunk* CreateNewChunk(ArchetypeIdentifierMask& id, int capacity);

		template <typename T>
		std::type_index ReturnComponentInfo();

		std::vector<int> m_EntitiesToDestroy;

		std::unordered_map<ArchetypeIdentifierMask, std::vector<std::unique_ptr<EntityChunk>>, ArchetypeIdentifierMaskHash> m_EntityChunks;
		std::vector<Entity> m_Entities;
		std::vector<int> m_FreeIndices;
		std::unique_ptr<ComponentRegistry> m_ComponentRegistry;
		int m_CurrentEntityId = 0;

		friend class Singleton<EntityManager>;
		EntityManager();
	};

	inline EntityManager::~EntityManager()
	{
		m_EntityChunks.clear();
	}

	inline Entity& EntityManager::CreateEntity(int newChunkCapacity)
	{
		//std::cout << "---------- Creating Entity ----------" << std::endl;

		ArchetypeIdentifierMask identifier = GetArchetypeIds<TransformComponent>();
		EntityChunk* chunk = GetFirstAvailableChunk(identifier, newChunkCapacity);

		int id;

		if (!m_FreeIndices.empty())
		{
			id = m_FreeIndices.back();
			m_FreeIndices.pop_back();
			m_Entities[id] = Entity(id);
		}
		else
		{
			id = static_cast<int>(m_Entities.size());
			m_Entities.emplace_back(Entity(id));
		}

		m_Entities[id].SetCurrentChunk(identifier, chunk->GetChunkIndex());
		chunk->AddEntity(id);

		// Construct transform component
		m_ComponentRegistry->TryRegisterComponent<TransformComponent>();
		int compId = Component<TransformComponent>::Index;
		void* array = chunk->GetComponentArray(compId);
		auto& info = m_ComponentRegistry->GetComponentInfo(compId);

		void* elementPtr = static_cast<char*>(array) + chunk->GetEntityInChunkIndex(id) * info.Size;
		info.Construct(elementPtr, id);

		//std::cout << "-------------------------------------" << std::endl;
		return m_Entities[id];
	}

	template<typename ComponentType>
	inline ComponentType* EntityManager::AddComponent(Entity& entity)
	{
		if (!entity.CurrentArchetypeId.HasComponent(Component<ComponentType>::Index))
		{
			m_ComponentRegistry->TryRegisterComponent<ComponentType>();

			// Get old chunk and archetype
			auto* oldChunk = m_EntityChunks[entity.CurrentArchetypeId][entity.CurrentChunkIndex].get();
			ArchetypeIdentifierMask oldArchetypeMask = entity.CurrentArchetypeId;

			// Create new archetype and chunk
			ArchetypeIdentifierMask newArchetypeMask = oldArchetypeMask;
			newArchetypeMask.AddComponent(Component<ComponentType>::Index);
			EntityChunk* newChunk = GetFirstAvailableChunk(newArchetypeMask, 20);

			// Add entity to new chunk
			int newIndex = newChunk->AddEntity(entity.Id);

			// Copy old components
			for (int componentId : oldArchetypeMask.GetComponentIndices())
			{
				void* srcArray = oldChunk->GetComponentArray(componentId);
				void* dstArray = newChunk->GetComponentArray(componentId);
				int oldIndex = oldChunk->GetEntityInChunkIndex(entity.Id);
				auto& info = m_ComponentRegistry->GetComponentInfo(componentId);

				void* srcPtr = static_cast<char*>(srcArray) + oldIndex * info.Size;
				void* dstPtr = static_cast<char*>(dstArray) + newIndex * info.Size;
				info.MoveConstruct(dstPtr, srcPtr);
				info.Destruct(srcPtr);
			}

			// Construct new component
			int compId = Component<ComponentType>::Index;
			void* array = newChunk->GetComponentArray(compId);
			auto& info = m_ComponentRegistry->GetComponentInfo(compId);

			void* elementPtr = static_cast<char*>(array) + newIndex * info.Size;
			info.Construct(elementPtr, entity.Id);

			// Remove from old chunk
			oldChunk->RemoveEntityAndComponents(entity);

			// Update entity data
			entity.CurrentArchetypeId = newArchetypeMask;
			entity.CurrentChunkIndex = newChunk->GetChunkIndex();
		}
		return GetComponent<ComponentType>(entity);
	}

	template<typename ComponentType>
	inline ComponentType* EntityManager::AddComponent(int entityId)
	{
		return AddComponent<ComponentType>(GetEntity(entityId));
	}

	//template<typename ComponentType>
	//inline void EntityManager::RemoveComponent(Entity& entity)
	//{
	//	if (!entity.CurrentArchetypeId.HasComponent(Component<ComponentType>::Index))
	//	{
	//		throw std::exception("Trying to remove a component from an entity that doesn't have it.");
	//	}
	//	m_ComponentRegistry->TryRegisterComponent<ComponentType>();
	//
	//	// Get old chunk and archetype
	//	auto* oldChunk = m_EntityChunks[entity.CurrentArchetypeId][entity.CurrentChunkIndex].get();
	//	ArchetypeIdentifierMask oldArchetypeMask = entity.CurrentArchetypeId;
	//
	//	// Create new archetype and chunk
	//	ArchetypeIdentifierMask newArchetypeMask = oldArchetypeMask;
	//	newArchetypeMask.RemoveComponent(Component<ComponentType>::Index);
	//	EntityChunk* newChunk = GetFirstAvailableChunk(newArchetypeMask, 20);
	//
	//	// Add entity to new chunk
	//	int newIndex = newChunk->AddEntity(entity.Id);
	//
	//	// Copy old components
	//	for (int componentId : newArchetypeMask.GetComponentIndices())
	//	{
	//		void* srcArray = oldChunk->GetComponentArray(componentId);
	//		void* dstArray = newChunk->GetComponentArray(componentId);
	//		int oldIndex = oldChunk->GetEntityInChunkIndex(entity.Id);
	//		auto& info = m_ComponentRegistry->GetComponentInfo(componentId);
	//
	//		void* srcPtr = static_cast<char*>(srcArray) + oldIndex * info.Size;
	//		void* dstPtr = static_cast<char*>(dstArray) + newIndex * info.Size;
	//		info.MoveConstruct(dstPtr, srcPtr);
	//	}
	//
	//	// Destruct old component
	//	int compId = Component<ComponentType>::Index;
	//	void* array = oldChunk->GetComponentArray(compId);
	//	auto& info = m_ComponentRegistry->GetComponentInfo(compId);
	//
	//	int oldIndex = oldChunk->GetEntityInChunkIndex(entity.Id);
	//	void* elementPtr = static_cast<char*>(array) + oldIndex * info.Size;
	//	info.Destruct(elementPtr);
	//
	//	// Remove from old chunk
	//	oldChunk->RemoveEntityAndComponents(entity);
	//
	//	// Update entity data
	//	entity.CurrentArchetypeId = newArchetypeMask;
	//	entity.CurrentChunkIndex = newChunk->GetChunkIndex();
	//}
	//
	//template<typename ComponentType>
	//inline void EntityManager::RemoveComponent(int entityId)
	//{
	//	RemoveComponent(GetEntity(entityId));
	//}

	template<typename ComponentType>
	inline ComponentType* EntityManager::GetComponent(Entity& entity)
	{
		auto* chunk = m_EntityChunks[entity.CurrentArchetypeId][entity.CurrentChunkIndex].get();
		void* array = chunk->GetComponentArray(Component<ComponentType>::Index);
		if (!array) return nullptr;
		int index = chunk->GetEntityInChunkIndex(entity.Id);
		return &static_cast<ComponentType*>(array)[index];
	}

	template<typename ComponentType>
	inline ComponentType* EntityManager::GetComponent(int entityId)
	{
		return GetComponent<ComponentType>(GetEntity(entityId));
	}

	template<typename ComponentType>
	inline ComponentType* EntityManager::GetComponentInChildren(Entity& entity)
	{
		for (int childId : GetComponent<TransformComponent>(entity)->GetChildEntityIds())
		{
			ComponentType* component = GetComponent<ComponentType>(childId);
			if (component) return component;
			component = GetComponentInChildren<ComponentType>(childId);
			if (component) return component;
		}
		return nullptr;
	}

	template<typename ComponentType>
	inline ComponentType* EntityManager::GetComponentInChildren(int entityId)
	{
		return GetComponentInChildren<ComponentType>(GetEntity(entityId));
	}

	template<typename ComponentType>
	inline ComponentType* EntityManager::GetComponentInParent(Entity& entity)
	{
		TransformComponent* transform = GetComponent<TransformComponent>(entity);
		GetComponent<ComponentType>(transform->GetParentEntityId());
		return nullptr;
	}

	template<typename ComponentType>
	inline ComponentType* EntityManager::GetComponentInParent(int entityId)
	{
		return GetComponentInParent(GetEntity(entityId));
	}

	template<typename ComponentType>
	inline bool EntityManager::HasComponent(int entityId)
	{
		return GetEntity(entityId).CurrentArchetypeId.HasComponent(Component<ComponentType>::Index);
	}

	template<typename ComponentType>
	inline bool EntityManager::HasComponent(Entity& entity)
	{
		return HasComponent<ComponentType>(entity.Id);
	}

	template<typename ...Components>
	inline EntityQueryResult<Components...> EntityManager::QueryEntities(bool includeNonActive, bool includeWhenComponentDisabled)
	{
		std::vector<EntityView<Components...>> entityViews;

		for (const auto& pair : m_EntityChunks)
		{
			if ((pair.first.HasComponent(Component<Components>::Index) && ...))
			{
				for (const std::unique_ptr<EntityChunk>& chunk : pair.second)
				{
					for (int chunkEntityIndex = 0; chunkEntityIndex < chunk->GetEntityIndices().size(); ++chunkEntityIndex)
					{
						std::span<int> entityIds = chunk->GetEntityIndices();
						if (!m_Entities[entityIds[chunkEntityIndex]].IsActive && !includeNonActive) continue;
						int indexInChunk = chunk->GetEntityInChunkIndex(entityIds[chunkEntityIndex]);

						auto components = std::tie(static_cast<Components*>(chunk->GetComponentArray(Component<Components>::Index))[indexInChunk]...);

						if (!includeWhenComponentDisabled && !(std::get<Components&>(components).IsEnabled && ...)) continue;

						entityViews.push_back(EntityView<Components...>
						{
							entityIds[chunkEntityIndex],
							components
						});
					}
				}
			}
		}

		return EntityQueryResult<Components...>{ entityViews };
	}

	template<typename ...Components>
	inline std::optional<EntityView<Components...>> EntityManager::GetFirstEntityWithComponents(bool includeNonActive, bool includeWhenComponentDisabled)
	{
		for (const auto& pair : m_EntityChunks)
		{
			if ((pair.first.HasComponent(Component<Components>::Index) && ...))
			{
				for (const std::unique_ptr<EntityChunk>& chunk : pair.second)
				{
					for (int chunkEntityIndex = 0; chunkEntityIndex < chunk->GetEntityIndices().size(); ++chunkEntityIndex)
					{
						std::span<int> entityIds = chunk->GetEntityIndices();
						if (!m_Entities[entityIds[chunkEntityIndex]].IsActive && !includeNonActive) continue;
						int indexInChunk = chunk->GetEntityInChunkIndex(entityIds[chunkEntityIndex]);

						auto components = std::tie(static_cast<Components*>(chunk->GetComponentArray(Component<Components>::Index))[indexInChunk]...);

						if (!includeWhenComponentDisabled && !(std::get<Components&>(components).IsEnabled && ...)) continue;

						return EntityView<Components...>
						{
							entityIds[chunkEntityIndex],
							components
						};
					}
				}
			}
		}

		return std::nullopt;
	}

	template<typename ...Components>
	inline EntityView<Components...> EntityManager::GetOrCreateFirstEntityWithComponents(bool includeNonActive, bool includeWhenComponentDisabled)
	{
		std::optional<EntityView<Components...>> view = GetFirstEntityWithComponents<Components...>(includeNonActive, includeWhenComponentDisabled);
		if (view.has_value()) return view.value();
		Entity& entity = CreateEntity();
		int entityId = entity.Id;
		((AddComponent<Components>(entityId)), ...);
		view = GetFirstEntityWithComponents<Components...>(includeNonActive, includeWhenComponentDisabled);
		return view.value();
	}

	template<typename T>
	inline std::type_index EntityManager::ReturnComponentInfo()
	{
		return std::type_index(typeid(T));
	}

	template<typename ...ComponentTypes>
	inline ArchetypeIdentifierMask EntityManager::GetArchetypeIds()
	{
		((m_ComponentRegistry->TryRegisterComponent<ComponentTypes>()), ...);
		ArchetypeIdentifierMask identifierMask;
		((identifierMask.AddComponent(Component<ComponentTypes>::Index)), ...);
		return identifierMask;
	}
}