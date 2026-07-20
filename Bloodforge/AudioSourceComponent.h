#pragma once
#include "Component.h"

using SoundId = unsigned int;
using AudioGroupId = unsigned int;

namespace Bloodforge
{
	struct AudioSourceComponent final : Component<AudioSourceComponent>
	{
		AudioSourceComponent();
		~AudioSourceComponent();
		void Play();
		void Pause();
		void Stop();
		void SetAudio(SoundId id);
		void SetAudioGroup(AudioGroupId id);
		void SetVolume(float volume);

		SoundId CurrentSoundId = 0;
		AudioGroupId CurrentAudioGroupId = 0;
		bool IsPlaying = false;
		bool IsPaused = false;
		bool ShouldLoop = false;
		float Volume = 1.0f;
		int TrackIndex = -1;
	};
}