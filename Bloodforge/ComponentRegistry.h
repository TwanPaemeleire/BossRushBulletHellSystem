#pragma once
#include <unordered_map>
#include "Component.h"

namespace Bloodforge
{
	struct ComponentInfo
	{
		size_t Size;
		size_t Alignment;
		void (*Construct)(void*, int);
		void (*Destruct)(void*);
		void (*MoveConstruct)(void*, void*);
	};

	class ComponentRegistry
	{
	public:
		ComponentRegistry() = default;
		~ComponentRegistry() = default;

		template <typename ComponentType>
		bool TryRegisterComponent();

		template <typename ComponentType>
		const ComponentInfo& GetComponentInfo();
		const ComponentInfo& GetComponentInfo(int componentId);
	private:
		std::unordered_map<int, ComponentInfo> m_ComponentRegistry;
		int m_ComponentIndexTracker = 0;
	};

	template<typename ComponentType>
	inline bool ComponentRegistry::TryRegisterComponent()
	{
		if (Component<ComponentType>::Index != -1) return false;
		Component<ComponentType>::Index = m_ComponentIndexTracker++;

		ComponentInfo info{};
		info.Size = sizeof(ComponentType);
		info.Alignment = alignof(ComponentType);
		info.Construct = [](void* ptr, int entityId)
			{
				ComponentType* comp = static_cast<ComponentType*>(ptr);
				new (ptr) ComponentType{};
				comp->OwnerEntityId = entityId;
			};
		info.Destruct = [](void* ptr)
			{
				static_cast<ComponentType*>(ptr)->~ComponentType();
			};
		info.MoveConstruct = [](void* dst, void* src)
			{
				new (dst) ComponentType(std::move(*static_cast<ComponentType*>(src)));
			};

		m_ComponentRegistry[Component<ComponentType>::Index] = info;
		return true;
	}

	template<typename ComponentType>
	inline const ComponentInfo& ComponentRegistry::GetComponentInfo()
	{
		TryRegisterComponent<ComponentType>();
		return m_ComponentRegistry[Component<ComponentType>::Index];
	}
}