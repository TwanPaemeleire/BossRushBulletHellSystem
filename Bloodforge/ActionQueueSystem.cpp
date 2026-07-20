#include "pch.h"
#include "ActionQueueSystem.h"
#include "ActionQueueComponent.h"
#include "EntityManager.h"
#include "BloodTime.h"

namespace Bloodforge
{
	void ActionQueueSystem::OnUpdate()
	{
		EntityQueryResult<ActionQueueComponent> result = EntityManager::GetInstance().QueryEntities<ActionQueueComponent>();
		for (EntityView<ActionQueueComponent>& entityView : result.EntityViews)
		{
			ActionQueueComponent& actionQueueComponent = entityView.GetComponent<ActionQueueComponent>();
			if (!actionQueueComponent.IsPlaying) continue;
			if (actionQueueComponent.CurrentActionIndex < actionQueueComponent.Actions.size())
			{
				if (actionQueueComponent.IsWaiting)
				{
					actionQueueComponent.WaitCounter -= BloodTime::GetInstance().DeltaTime;
					if (actionQueueComponent.WaitCounter <= 0.0f)
					{
						actionQueueComponent.IsWaiting = false;
						actionQueueComponent.WaitCounter = 0.0f;
					}
					else
					{
						continue;
					}
				}
				std::function<void()>& currentAction = actionQueueComponent.Actions[actionQueueComponent.CurrentActionIndex];
				currentAction();
				++actionQueueComponent.CurrentActionIndex;
			}
			bool reachedEndOfActions = actionQueueComponent.CurrentActionIndex >= actionQueueComponent.Actions.size();
			if (!reachedEndOfActions) continue;
			if (actionQueueComponent.ShouldLoop)
			{
				actionQueueComponent.CurrentActionIndex = 0;
				actionQueueComponent.IsWaiting = false;
				actionQueueComponent.WaitCounter = 0.0f;
			}
			else
			{
				actionQueueComponent.IsPlaying = false;
				if (actionQueueComponent.DestroyEntityOnFinish)
				{
					EntityManager::GetInstance().DestroyEntity(entityView.EntityId);
				}
			}
		}
	}
}