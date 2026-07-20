#include "pch.h"
#include "FunctionInvokeSystem.h"
#include "EntityManager.h"
#include "FunctionInvokeComponent.h"
#include "BloodTime.h"
#include <vector>

namespace Bloodforge
{
	void FunctionInvokeSystem::OnUpdate()
	{
		std::vector<int> entitiesToDestroy;
		EntityQueryResult<FunctionInvokeComponent> result = EntityManager::GetInstance().QueryEntities<FunctionInvokeComponent>();
		for (EntityView<FunctionInvokeComponent>& entityView : result.EntityViews)
		{
			FunctionInvokeComponent& invokeComp = entityView.GetComponent<FunctionInvokeComponent>();
			invokeComp.TimeToInvoke -= BloodTime::GetInstance().DeltaTime;
			if (invokeComp.TimeToInvoke <= 0.0f)
			{
				invokeComp.Function();
				entitiesToDestroy.emplace_back(invokeComp.OwnerEntityId);
			}
		}

		EntityManager::GetInstance().DestroyEntity(entitiesToDestroy);
	}
}