#pragma once
#include "System.h"
#include <vector>
#include <unordered_map>

using AudioGroupId = unsigned int;
using SoundId = unsigned int;

struct MIX_Mixer;
struct MIX_Track;
namespace Bloodforge
{
	struct TrackEntryData
	{
		MIX_Track* Track;
		float Volume = 1.0f;
		AudioGroupId GroupId;
	};

	struct AudioGroupData
	{
		std::vector<int> TrackIndices;
		float Volume = 1.0f;
	};

	struct AudioSourceComponent;
	class AudioSourceSystem final : public System
	{
	public:
		virtual void OnStart() override;
		virtual void OnCleanup() override;

		void SetAudioOfTrack(AudioSourceComponent& audioSource, SoundId id);
		void AddAudioToAudioGroup(AudioSourceComponent& audioSource, AudioGroupId id);
		void StartPlayingAudioSource(AudioSourceComponent& audioSource);
		void StopPlayingAudioSource(AudioSourceComponent& audioSource);
		void PauseAudioSource(AudioSourceComponent& audioSource);
		void ResumeAudioSource(AudioSourceComponent& audioSource);

		void StopAllAudioSources();

		void AddAudioSource(AudioSourceComponent& audioSource);
		void RemoveAudioSource(AudioSourceComponent& audioSource);

		void SetAudioTrackVolume(AudioSourceComponent& audioSource, float volume);
		void SetMasterVolume(float volume);
		void SetAudioGroupVolume(AudioGroupId id, float volume);
	private:
		int GetAvailableMixTrackIndex();
		void RecalculateTrackVolume(int trackIndex);

		MIX_Mixer* m_Mixer = nullptr;
		std::vector<TrackEntryData> m_AllMixTracks;
		std::vector<int> m_FreeIndices;
		std::unordered_map<AudioGroupId, AudioGroupData> m_AudioGroupData;

		float m_MasterVolume = 1.0f;
	};
}