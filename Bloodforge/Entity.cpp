#include "pch.h"
#include "Entity.h"
#include "EntityManager.h"

namespace Bloodforge
{
	Entity::Entity(int id)
		: Id(id)
	{
	}

	void Entity::SetCurrentChunk(const ArchetypeIdentifierMask& chunkId, int chunkIndex)
	{
		CurrentArchetypeId = chunkId;
		CurrentChunkIndex = chunkIndex;
	}

	void Entity::SetActive(bool isActive)
	{
		IsActive = isActive;
		for (int entityId : EntityManager::GetInstance().GetComponent<TransformComponent>(Id)->GetChildEntityIds())
		{
			EntityManager::GetInstance().GetEntity(entityId).SetActive(isActive);
		}
	}
}