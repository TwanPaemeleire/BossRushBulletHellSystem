#include "pch.h"
#include "SpriteAnimatorSystem.h"
#include "EntityManager.h"
#include "SpriteComponent.h"
#include "SpriteAnimatorComponent.h"
#include "BloodTime.h"
#include <utility>
#include "SpriteAnimatorData.h"

namespace Bloodforge
{
	void SpriteAnimatorSystem::OnStart()
	{
		EntityQueryResult<SpriteAnimatorComponent, SpriteComponent> result = EntityManager::GetInstance().QueryEntities<SpriteAnimatorComponent, SpriteComponent>();
		for (EntityView<SpriteAnimatorComponent, SpriteComponent>& view : result.EntityViews)
		{
			SpriteComponent& spriteComp = view.GetComponent<SpriteComponent>();
			SpriteAnimatorComponent& animationComp = view.GetComponent<SpriteAnimatorComponent>();
			spriteComp.SetTexture(animationComp.AnimationsData[animationComp.CurrentPlayingId].Texture);
		}
	}

	void SpriteAnimatorSystem::OnUpdate()
	{
		EntityQueryResult<SpriteAnimatorComponent, SpriteComponent> result = EntityManager::GetInstance().QueryEntities<SpriteAnimatorComponent, SpriteComponent>();
		for (EntityView<SpriteAnimatorComponent, SpriteComponent>& view : result.EntityViews)
		{
			SpriteComponent& spriteComp = view.GetComponent<SpriteComponent>();
			SpriteAnimatorComponent& animationComp = view.GetComponent<SpriteAnimatorComponent>();

			if (animationComp.IsPlaying)
			{
				if (!animationComp.IsPaused)
				{
					animationComp.FrameTimeCounter += BloodTime::GetInstance().DeltaTime;
					HandleAnimationEvents(animationComp);
					const AnimationData& animData = animationComp.AnimationsData[animationComp.CurrentPlayingId];
					if (animationComp.FrameTimeCounter >= animData.FrameTime)
					{
						animationComp.AdvanceToNextFrame();

						if (animationComp.CurrentFrame == 0)
						{
							ResetAllAnimationEvents(animationComp);
						}
					}
				}

				const AnimationData& animData = animationComp.AnimationsData[animationComp.CurrentPlayingId];
				spriteComp.SetCustomSourceRect(animationComp.CurrentSourceRect);
				spriteComp.SetTexture(animData.Texture);
			}
			else if (!animationComp.IsPlaying)
			{
				spriteComp.SetTexture(nullptr);
			}
		}
	}

	void SpriteAnimatorSystem::HandleAnimationEvents(SpriteAnimatorComponent& spriteAnimComp)
	{
		EntityView<SpriteAnimatorData> dataView = EntityManager::GetInstance().GetOrCreateFirstEntityWithComponents<SpriteAnimatorData>();
		SpriteAnimatorData& data = dataView.GetComponent<SpriteAnimatorData>();

		for (AnimationEventData& animEventData : spriteAnimComp.AnimationsData[spriteAnimComp.CurrentPlayingId].AnimationEvents)
		{
			if (!animEventData.HasBeenTriggered && spriteAnimComp.CurrentFrame == animEventData.FrameToTrigger && spriteAnimComp.FrameTimeCounter >= animEventData.Offset)
			{
				data.AnimationEventsCallbacks[animEventData.CallbackIndex](spriteAnimComp);
				animEventData.HasBeenTriggered = true;
			}
		}
	}
	void SpriteAnimatorSystem::ResetAllAnimationEvents(SpriteAnimatorComponent& spriteAnimComp)
	{
		for (AnimationEventData& animEventData : spriteAnimComp.AnimationsData[spriteAnimComp.CurrentPlayingId].AnimationEvents)
		{
			animEventData.HasBeenTriggered = false;
		}
	}
}