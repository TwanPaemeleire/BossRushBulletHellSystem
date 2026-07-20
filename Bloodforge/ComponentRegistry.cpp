#include "pch.h"
#include "ComponentRegistry.h"

namespace Bloodforge
{
    const ComponentInfo& ComponentRegistry::GetComponentInfo(int componentId)
    {
        return m_ComponentRegistry[componentId];
    }
}