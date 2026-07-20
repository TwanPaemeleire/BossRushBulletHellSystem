#pragma once
#include "ArchetypeIdentifierMask.h"
#include <string>

using TagId = unsigned int;

namespace Bloodforge
{
	struct Entity
	{
		Entity(int id);
		~Entity() = default;

		void SetCurrentChunk(const ArchetypeIdentifierMask& archetypeIds, int chunkIndex);
		void SetActive(bool isActive);

		int Id = -1;
		ArchetypeIdentifierMask CurrentArchetypeId;
		int CurrentChunkIndex = -1;
		TagId Tag = 0;
		std::string Name;
		bool IsActive = true;
		bool MarkedForDestruction = false;
		bool IsAlive = true;
		bool DontDestroyOnSceneSwitch = false;
	};
}