#include "pch.h"
#include "SpriteAnimatorComponent.h"
#include "Texture2D.h"
#include "EntityManager.h"
#include "SpriteAnimatorData.h"
#include <utility>

namespace Bloodforge
{
	void SpriteAnimatorComponent::AddAnimation(AnimationId id, const AnimationData& animationData)
	{
		if (!AnimationsData.contains(id))
		{
			AnimationsData.insert({ id, animationData });
		}
		else
		{
			throw std::exception("Trying to add an animation with an id that has already been added.");
		}
	}

	void SpriteAnimatorComponent::PlayAnimation(AnimationId id)
	{
		if (!AnimationsData.contains(id))
		{
			throw std::exception("Trying to play an animation with an id that hasn't been added.");
		}

		AnimationData& data = AnimationsData[id];
		for (AnimationEventData& eventData : data.AnimationEvents)
		{
			eventData.HasBeenTriggered = false;
		}

		IsPlaying = true;
		CurrentFrame = data.StartingFrame;
		FrameTimeCounter = 0.0f;
		CurrentPlayingId = id;
		CurrentFrameWidth = data.Texture->GetWidth() / data.NumberOfFrames;
		CurrentFrameHeight = data.Texture->GetHeight();
		CurrentTexture = data.Texture;
		CurrentSourceRect.X = data.StartingFrame * CurrentFrameWidth;
		CurrentSourceRect.Y = 0.0f;
		CurrentSourceRect.Width = CurrentFrameWidth;
		CurrentSourceRect.Height = CurrentFrameHeight;
	}

	void SpriteAnimatorComponent::SpriteAnimatorComponent::Pause()
	{
		if (!IsPlaying) return;
		IsPaused = true;
	}

	void SpriteAnimatorComponent::Resume()
	{
		if (!IsPlaying) return;
		IsPaused = false;
	}

	void SpriteAnimatorComponent::Stop()
	{
		IsPlaying = false;
		IsPaused = false;
	}

	void SpriteAnimatorComponent::AdvanceToNextFrame()
	{
		const AnimationData& data = AnimationsData[CurrentPlayingId];
		if (CurrentFrame < data.NumberOfFrames - 1) // Go to next frame, end of animation not reached yet
		{
			++CurrentFrame;
			FrameTimeCounter = 0.0f;
		}
		else // Go to next frame, end of animation has been reached
		{
			if (data.ShouldLoop)
			{
				CurrentFrame = data.StartingFrameIndexAfterLoop;
				FrameTimeCounter = 0.0f;

			}
			else
			{
				Pause();
				CurrentFrame = data.NumberOfFrames - 1;
				FrameTimeCounter = 0.0f;
			}
		}

		CurrentSourceRect.X = CurrentFrame * CurrentFrameWidth;
		CurrentSourceRect.Y = 0.0f;
	}

	void SpriteAnimatorComponent::RegressToPreviousFrame()
	{
		if (CurrentFrame > 0)
		{
			--CurrentFrame;
			FrameTimeCounter = 0.0f;
		}
	}

	void SpriteAnimatorComponent::AddAnimationEvent(AnimationId id, std::function<void(SpriteAnimatorComponent&)> callback, int frameToTrigger, float offset)
	{
		if (!AnimationsData.contains(id))
		{
			throw std::exception("Trying to add animation event to an animation with an id that doesn't exist");
		}
		EntityView<SpriteAnimatorData> dataView = EntityManager::GetInstance().GetOrCreateFirstEntityWithComponents<SpriteAnimatorData>();
		SpriteAnimatorData& data = dataView.GetComponent<SpriteAnimatorData>();
		AnimationsData[id].AnimationEvents.push_back({ frameToTrigger, offset, false, data.NextAnimationEventCallbackID });
		data.AnimationEventsCallbacks.insert({ data.NextAnimationEventCallbackID, std::move(callback) });
		data.NextAnimationEventCallbackID++;
	}
}