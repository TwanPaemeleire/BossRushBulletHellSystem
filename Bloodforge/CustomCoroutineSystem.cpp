#include "pch.h"
#include "CustomCoroutineSystem.h"
#include "CustomCoroutineDataComponent.h"
#include "EntityManager.h"
#include "BloodTime.h"

void Bloodforge::CustomCoroutineSystem::OnUpdate()
{
	EntityQueryResult<CustomCoroutineDataComponent> result = EntityManager::GetInstance().QueryEntities<CustomCoroutineDataComponent>();
	for (EntityView<CustomCoroutineDataComponent> view : result.EntityViews)
	{
		CustomCoroutineDataComponent& coroutineDataComp = view.GetComponent<CustomCoroutineDataComponent>();
		if (coroutineDataComp.WaitTimeCounter > 0.0f)
		{
			coroutineDataComp.WaitTimeCounter -= BloodTime::GetInstance().DeltaTime;
			if (coroutineDataComp.WaitTimeCounter <= 0.0f)
			{
				coroutineDataComp.WaitTimeCounter = 0.0f;
				coroutineDataComp.CoroutineHandle.handle.resume();
			}
		}
		else if (coroutineDataComp.IsWaitingForNextFrame && !coroutineDataComp.CoroutineHandle.handle.done())
		{
			coroutineDataComp.IsWaitingForNextFrame = false;
			coroutineDataComp.CoroutineHandle.handle.resume();
		}
	}
}