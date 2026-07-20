#include "pch.h"
#include "AudioSourceSystem.h"
#include "ResourceManager.h"
#include "AudioSourceComponent.h"
#include <SDL3_mixer/SDL_mixer.h>
#include "EntityManager.h"

namespace Bloodforge
{
	void AudioSourceSystem::OnStart()
	{
		m_Mixer = ResourceManager::GetInstance().GetMixer();
	}

	void AudioSourceSystem::OnCleanup()
	{
		for (TrackEntryData& data : m_AllMixTracks)
		{
			MIX_DestroyTrack(data.Track);
		}
	}

	void AudioSourceSystem::SetAudioOfTrack(AudioSourceComponent& audioSource, SoundId id)
	{
		MIX_Audio* audio = ResourceManager::GetInstance().GetAudio(id);
		if (audio == nullptr)
		{
			throw std::exception("Trying to set an audio with an id that hasn't been loaded.");
		}
		audioSource.CurrentSoundId = id;
		MIX_SetTrackAudio(m_AllMixTracks[audioSource.TrackIndex].Track, audio);
		RecalculateTrackVolume(audioSource.TrackIndex);
	}

	void AudioSourceSystem::AddAudioToAudioGroup( AudioSourceComponent& audioSource, AudioGroupId id)
	{
		if (audioSource.CurrentAudioGroupId != 0)
		{
			// Remove from old group
			m_AudioGroupData[audioSource.CurrentAudioGroupId].TrackIndices.erase(std::remove(m_AudioGroupData[audioSource.CurrentAudioGroupId].TrackIndices.begin(), m_AudioGroupData[audioSource.CurrentAudioGroupId].TrackIndices.end(), audioSource.TrackIndex), m_AudioGroupData[audioSource.CurrentAudioGroupId].TrackIndices.end());
		}
		audioSource.CurrentAudioGroupId = id;
		m_AudioGroupData[id].TrackIndices.emplace_back(audioSource.TrackIndex);
		m_AllMixTracks[audioSource.TrackIndex].GroupId = id;
	}

	void AudioSourceSystem::StartPlayingAudioSource(AudioSourceComponent& audioSource)
	{
		audioSource.IsPlaying = true;
		audioSource.IsPaused = false;
		MIX_PlayTrack(m_AllMixTracks[audioSource.TrackIndex].Track, 0);
	}

	void AudioSourceSystem::StopPlayingAudioSource(AudioSourceComponent& audioSource)
	{
		if (!audioSource.IsPlaying) return;
		audioSource.IsPlaying = false;
		audioSource.IsPaused = false;
		MIX_StopTrack(m_AllMixTracks[audioSource.TrackIndex].Track, 0);
	}

	void AudioSourceSystem::PauseAudioSource(AudioSourceComponent& audioSource)
	{
		if (!audioSource.IsPlaying || audioSource.IsPaused) return;
		audioSource.IsPaused = true;
		MIX_PauseTrack(m_AllMixTracks[audioSource.TrackIndex].Track);
	}

	void AudioSourceSystem::ResumeAudioSource(AudioSourceComponent& audioSource)
	{
		MIX_ResumeTrack(m_AllMixTracks[audioSource.TrackIndex].Track);
	}

	void AudioSourceSystem::StopAllAudioSources()
	{
		EntityQueryResult<AudioSourceComponent> result = EntityManager::GetInstance().QueryEntities<AudioSourceComponent>();
		for (EntityView<AudioSourceComponent>& entityView : result.EntityViews)
		{
			AudioSourceComponent& audioSource = entityView.GetComponent<AudioSourceComponent>();
			StopPlayingAudioSource(audioSource);
		}
	}

	void AudioSourceSystem::AddAudioSource(AudioSourceComponent& audioSource)
	{
		audioSource.TrackIndex = GetAvailableMixTrackIndex();
	}

	void AudioSourceSystem::RemoveAudioSource(AudioSourceComponent& audioSource)
	{
		audioSource.Stop();
		m_AudioGroupData[audioSource.CurrentAudioGroupId].TrackIndices.erase(std::remove(m_AudioGroupData[audioSource.CurrentAudioGroupId].TrackIndices.begin(), m_AudioGroupData[audioSource.CurrentAudioGroupId].TrackIndices.end(), audioSource.TrackIndex), m_AudioGroupData[audioSource.CurrentAudioGroupId].TrackIndices.end());
		m_FreeIndices.emplace_back(audioSource.TrackIndex);
	}

	void AudioSourceSystem::SetAudioTrackVolume(AudioSourceComponent& audioSource, float volume)
	{
		audioSource.Volume = volume;
		m_AllMixTracks[audioSource.TrackIndex].Volume = volume;
		RecalculateTrackVolume(audioSource.TrackIndex);
	}

	void AudioSourceSystem::SetMasterVolume(float volume)
	{
		MIX_SetMixerGain(m_Mixer, volume);
		m_MasterVolume = volume;
		for (int index = 0; index < m_AllMixTracks.size(); index++)
		{
			RecalculateTrackVolume(index);
		}
	}

	void AudioSourceSystem::SetAudioGroupVolume(AudioGroupId id, float volume)
	{
		if (!m_AudioGroupData.contains(id))
		{
			throw std::exception("Trying to set the volume of an audio group with an id that doesn't exist.");
		}
		
		m_AudioGroupData[id].Volume = volume;
		for (int index : m_AudioGroupData[id].TrackIndices)
		{
			RecalculateTrackVolume(index);
		}
	}

	int AudioSourceSystem::GetAvailableMixTrackIndex()
	{
		if (m_FreeIndices.size() == 0)
		{
			if(m_Mixer == nullptr) m_Mixer = ResourceManager::GetInstance().GetMixer();
			MIX_Track* newTrack = MIX_CreateTrack(m_Mixer);
			m_AllMixTracks.emplace_back(newTrack);
			return static_cast<int>(m_AllMixTracks.size()) - 1;
		}
		int trackIdx = m_FreeIndices[m_FreeIndices.size() - 1];
		m_FreeIndices.erase(m_FreeIndices.end() - 1, m_FreeIndices.end());
		return trackIdx;
	}

	void AudioSourceSystem::RecalculateTrackVolume(int trackIndex)
	{
		float groupVolume = m_AudioGroupData[m_AllMixTracks[trackIndex].GroupId].Volume;
		MIX_SetTrackGain(m_AllMixTracks[trackIndex].Track, m_MasterVolume * groupVolume * m_AllMixTracks[trackIndex].Volume);
	}
}