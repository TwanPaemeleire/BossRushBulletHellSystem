#include "pch.h"
#include "EntityChunk.h"
#include "EntityManager.h"
#include <ranges>

namespace Bloodforge
{
	EntityChunk::EntityChunk(ArchetypeIdentifierMask& identifier, size_t capacity)
		: m_Capacity{ capacity }, m_EntityIds{ std::make_unique<int[]>(capacity) }
	{
		int index = 0;
		for (int id : identifier.GetComponentIndices())
		{
			m_ComponentIdToArrayIndex[id] = index++;
			ConstructComponentArray(id);
		}

		m_ComponentIndices = identifier.GetComponentIndices();

		// std::cout << "EntityChunk created with capacity: " << capacity << std::endl;
		// std::cout << "With following component types: ";
		// ((std::cout << typeid(Components).name() << " "), ...);
		// std::cout << std::endl;
	}

	EntityChunk::~EntityChunk()
	{
		for (auto& [id, array] : m_ComponentArrays)
		{
			const ComponentInfo& info = EntityManager::GetInstance().GetComponentRegistry()->GetComponentInfo(id);
			for (int i = 0; i < m_EntityCount; ++i)
			{
				void* ptr = static_cast<char*>(array.Data) + i * info.Size;
				info.Destruct(ptr);
			}
			std::free(array.Data);
		}
	}

	int EntityChunk::AddEntity(int entityId)
	{
		if (IsFull())
		{
			throw std::runtime_error("Chunk is full. Cannot add more entities.");
		}

		int entityIndexInChunk = m_EntityCount;

		// std::cout << "Adding entity with ID: " << entityId << " to chunk with components:";
		// ((std::cout << " " << typeid(Components).name()), ...);
		// std::cout << std::endl << "Chunk fill progress: " << m_EntityCount + 1 << "/" << m_Capacity << std::endl;

		m_EntityIds[entityIndexInChunk] = entityId;
		m_EntityIdToChunkIndex[entityId] = entityIndexInChunk;

		++m_EntityCount;
		m_IsFull = (m_EntityCount) >= m_Capacity;
		return entityIndexInChunk; // Return the index of the newly added entity
	}

	void EntityChunk::RemoveEntityAndComponents(Entity& entity)
	{
		int lastIndex = m_EntityCount - 1;
		int entityIndexInChunk = m_EntityIdToChunkIndex[entity.Id];

		if (entityIndexInChunk != lastIndex)
		{
			int movedEntity = m_EntityIds[lastIndex];

			m_EntityIds[entityIndexInChunk] = movedEntity;

			MoveComponents(entityIndexInChunk, lastIndex, m_ComponentIndices);

			m_EntityIdToChunkIndex[movedEntity] = entityIndexInChunk;

			Entity& movedEntityRef = EntityManager::GetInstance().GetEntity(movedEntity);
			movedEntityRef.CurrentChunkIndex = GetChunkIndex();
		}

		m_EntityIdToChunkIndex.erase(entity.Id);
		--m_EntityCount;
		m_IsFull = false;
	}

	void EntityChunk::SetChunkIndexSelfAndInEntities(int index)
	{
		m_ChunkIndex = index;
		for (int entityId : GetEntityIndices())
		{
			Entity& entity = EntityManager::GetInstance().GetEntity(entityId);
			entity.CurrentChunkIndex = index;
		}
	}

	bool EntityChunk::ContainsComponent(int componentIndex) const
	{
		return m_ComponentIdToArrayIndex.contains(componentIndex);
	}

	bool EntityChunk::HasEntity(int entityId) const
	{
		return m_EntityIdToChunkIndex.contains(entityId);
	}

	std::span<int> EntityChunk::GetEntityIndices() const
	{
		return { m_EntityIds.get(), static_cast<size_t>(m_EntityCount) };
	}

	void* EntityChunk::GetComponentArray(int componentId) const
	{
		auto it = m_ComponentArrays.find(componentId);
		if (it == m_ComponentArrays.end()) return nullptr;
		return it->second.Data;
	}

	void EntityChunk::ConstructComponentArray(int componentIndex)
	{
		auto& info = EntityManager::GetInstance().GetComponentRegistry()->GetComponentInfo(componentIndex);
		void* arrayMemory = std::malloc(info.Size * m_Capacity);

		m_ComponentArrays[componentIndex] =
		{
			arrayMemory,
			info.Size
		};
	}

	void EntityChunk::MoveComponents(int target, int source, const std::vector<int>& componentIndices)
	{
		for (int id : componentIndices)
		{
			auto& array = m_ComponentArrays[id];

			char* data = static_cast<char*>(array.Data);

			void* srcPtr = data + source * array.ElementSize;
			void* dstPtr = data + target * array.ElementSize;

			const ComponentInfo& info = EntityManager::GetInstance().GetComponentRegistry()->GetComponentInfo(id);

			info.MoveConstruct(dstPtr, srcPtr);
			info.Destruct(srcPtr);
		}
	}
}