#include "pch.h"
#include "EntityManager.h"
#include <iostream>
#include "EntityChunk.h"
namespace Bloodforge
{
	EntityManager::EntityManager()
		:m_ComponentRegistry{ std::make_unique<ComponentRegistry>() }
	{
	}

	void EntityManager::DestroyEntity(int entityId)
	{
		//std::cout << "--------- Destroying Entity ---------" << std::endl;
		Entity& entity = m_Entities[entityId];
		if (entity.MarkedForDestruction) return;
		entity.MarkedForDestruction = true;
		m_EntitiesToDestroy.emplace_back(entityId);
		//std::cout << "-------------------------------------" << std::endl;
	}

	void EntityManager::DestroyEntity(const std::vector<int>& entityIds)
	{
		for (int entityId : entityIds)
		{
			DestroyEntity(entityId);
		}
	}

	void EntityManager::DestroyEntity(Entity& entity)
	{
		DestroyEntity(entity.Id);
	}

	void EntityManager::DestroyAllEntities()
	{
		m_Entities.clear();
		m_EntityChunks.clear();
		m_FreeIndices.clear();
	}

	void EntityManager::DestroyEntitiesOnSceneSwitch()
	{
		// Should find a way to optimize this later down the line
		for (const auto& pair : m_EntityChunks)
		{
			for (const std::unique_ptr<EntityChunk>& chunk : pair.second)
			{
				std::vector<int> entitiesToRemove;
				std::span<int> entityIds = chunk->GetEntityIndices();
				for (int chunkEntityIndex = 0; chunkEntityIndex < chunk->GetEntityIndices().size(); ++chunkEntityIndex)
				{
					if (!m_Entities[entityIds[chunkEntityIndex]].DontDestroyOnSceneSwitch) 
					{
						entitiesToRemove.emplace_back(m_Entities[entityIds[chunkEntityIndex]].Id);
					}
				}
				for (int entityToDelete : entitiesToRemove)
				{
					DestroyEntity(entityToDelete);
				}
			}
		}
		DestroyMarkedForDestructionEntities();
	}

	void EntityManager::DestroyMarkedForDestructionEntities()
	{
		std::for_each(m_EntitiesToDestroy.begin(), m_EntitiesToDestroy.end(), [this](int entityId)
		{
				Entity& entity = GetEntity(entityId);
				entity.IsAlive = false;
				const ArchetypeIdentifierMask& chunkId = entity.CurrentArchetypeId;
				EntityChunk* entityChunk = m_EntityChunks[chunkId][entity.CurrentChunkIndex].get();
				// Destroy components
				for (int componentId : entity.CurrentArchetypeId.GetComponentIndices())
				{
					void* srcArray = entityChunk->GetComponentArray(componentId);
					int indexInArray = entityChunk->GetEntityInChunkIndex(entityId);
					auto& info = m_ComponentRegistry->GetComponentInfo(componentId);

					void* srcPtr = static_cast<char*>(srcArray) + indexInArray * info.Size;
					info.Destruct(srcPtr);
				}

				entityChunk->RemoveEntityAndComponents(entity);
				m_FreeIndices.push_back(entityId);
			});

		for (auto& pair : m_EntityChunks)
		{
			auto& chunks = pair.second;

			bool removedAny = false;
			chunks.erase(std::remove_if(chunks.begin(), chunks.end(), [&removedAny](const std::unique_ptr<EntityChunk>& chunk)
					{
						bool isEmpty = chunk->GetEntityIndices().empty();
						if (isEmpty) removedAny = true;
						return isEmpty;
					}), 
					chunks.end());

			if (removedAny && chunks.size() > 0) // We need to update chunk indices in both the chunk and the entity
			{
				for (int chunkIndex = 0; chunkIndex < chunks.size(); ++chunkIndex)
				{
					chunks[chunkIndex]->SetChunkIndexSelfAndInEntities(chunkIndex);
				}
			}
		}

		m_EntitiesToDestroy.clear();
	}

	EntityChunk* EntityManager::GetFirstAvailableChunk(ArchetypeIdentifierMask& id, int capacity)
	{
		EntityChunk* chunk = nullptr;
		if (m_EntityChunks.find(id) == m_EntityChunks.end()) // Check if any chunk exists
		{
			chunk = CreateNewChunk(id, capacity);
		}
		else // Check if any chunk with space is left
		{
			for (const std::unique_ptr<EntityChunk>& existingChunk : m_EntityChunks[id])
			{
				if (!existingChunk->IsFull())
				{
					chunk = existingChunk.get();
					//std::cout << "Existing chunk with space was found" << std::endl;
					break;
				}
			}
			if (chunk == nullptr) // If all chunks are full, create a new one
			{
				chunk = CreateNewChunk(id, capacity);
			}
		}
		return chunk;
	}

	EntityChunk* EntityManager::CreateNewChunk(ArchetypeIdentifierMask& id, int capacity)
	{
		std::unique_ptr<EntityChunk> newChunk = std::make_unique<EntityChunk>(id, capacity);
		EntityChunk* chunkPtr = newChunk.get();
		m_EntityChunks[id].push_back(std::move(newChunk));
		chunkPtr->SetChunkIndexSelf(static_cast<int>(m_EntityChunks[id].size()) - 1);
		//std::cout << "Created new chunk for entity signature: " << id << std::endl;
		return chunkPtr;
	}
}