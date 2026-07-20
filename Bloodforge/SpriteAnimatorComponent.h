#pragma once
#include "Component.h"
#include "Rect.h"
#include <functional>

using AnimationId = unsigned int;

namespace Bloodforge
{
	class Texture2D;

	struct AnimationEventData
	{
		int FrameToTrigger = 0;
		float Offset = 0.0f;
		bool HasBeenTriggered = false;
		int CallbackIndex = 0;
	};

	struct AnimationData
	{
		Texture2D* Texture = nullptr;
		float FrameTime = 0.2f;
		int NumberOfFrames = 0;
		bool ShouldLoop = true;
		int StartingFrameIndexAfterLoop = 0;
		int StartingFrame = 0;
		std::vector<AnimationEventData> AnimationEvents;
	};

	struct SpriteAnimatorComponent final : public Component<SpriteAnimatorComponent>
	{
		void AddAnimation(AnimationId id, const AnimationData& animationData);

		void PlayAnimation(AnimationId id);
		void Pause();
		void Resume();
		void Stop();

		void AdvanceToNextFrame();
		void RegressToPreviousFrame();

		void AddAnimationEvent(AnimationId id, std::function<void(SpriteAnimatorComponent&)> callback, int indexOfFrameToTriggerOn, float offset = 0.0f);

		std::unordered_map<AnimationId, AnimationData> AnimationsData;

		AnimationId CurrentPlayingId = 0;
		Rect CurrentSourceRect;
		float CurrentFrameWidth = 0.0f;
		float CurrentFrameHeight = 0.0f;
		Texture2D* CurrentTexture;

		bool IsPlaying = false;
		bool IsPaused = false;
		int CurrentFrame = 0;
		float FrameTimeCounter = 0.0f;
	};
}