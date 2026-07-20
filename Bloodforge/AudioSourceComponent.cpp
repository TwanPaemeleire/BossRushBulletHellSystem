#include "pch.h"
#include "AudioSourceComponent.h"
#include "AudioSourceSystem.h"
#include "SceneSystemManager.h"
#include <SDL3_mixer/SDL_mixer.h>
#include "ResourceManager.h"

namespace Bloodforge
{
	AudioSourceComponent::AudioSourceComponent()
	{
		SceneSystemManager::GetInstance().GetSystem<AudioSourceSystem>()->AddAudioSource(*this);
	}

	AudioSourceComponent::~AudioSourceComponent()
	{
		SceneSystemManager::GetInstance().GetSystem<AudioSourceSystem>()->RemoveAudioSource(*this);
	}

	void AudioSourceComponent::Play()
	{
		SceneSystemManager::GetInstance().GetSystem<AudioSourceSystem>()->StartPlayingAudioSource(*this);
	}

	void AudioSourceComponent::Pause()
	{
		SceneSystemManager::GetInstance().GetSystem<AudioSourceSystem>()->PauseAudioSource(*this);
	}

	void AudioSourceComponent::Stop()
	{
		SceneSystemManager::GetInstance().GetSystem<AudioSourceSystem>()->StopPlayingAudioSource(*this);
	}

	void AudioSourceComponent::SetAudio(SoundId id)
	{
		SceneSystemManager::GetInstance().GetSystem<AudioSourceSystem>()->SetAudioOfTrack(*this, id);
	}

	void AudioSourceComponent::SetAudioGroup(AudioGroupId id)
	{
		SceneSystemManager::GetInstance().GetSystem<AudioSourceSystem>()->AddAudioToAudioGroup(*this, id);
	}

	void AudioSourceComponent::SetVolume(float volume)
	{
		SceneSystemManager::GetInstance().GetSystem<AudioSourceSystem>()->SetAudioTrackVolume(*this, volume);
	}
}