#pragma once
#include "Singleton.h"
#include <unordered_map>
#include <string>
#include <memory>
#include "Font.h"
#include "CustomCursor.h"
#include "Texture2D.h"

using CursorId = unsigned int;
using SoundId = unsigned int;

namespace std
{
	template<>
	struct hash<std::pair<std::string, float>>
	{
		size_t operator()(const std::pair<std::string, float>& p) const noexcept
		{
			size_t h1 = hash<std::string>{}(p.first);
			size_t h2 = hash<float>{}(p.second);
			return h1 ^ (h2 << 1);
		}
	};
}

struct MIX_Audio;
struct MIX_Mixer;
namespace Bloodforge
{
	class Texture2D;
	class ResourceManager final : public Singleton<ResourceManager>
	{
	public:
		void Cleanup();

		Texture2D* LoadTexture(const std::string& file);
		Font* LoadFont(const std::string& file, float size);
		CustomCursor* LoadCustomCursor(CursorId id, const std::string& file, int xOffset, int yOffset);
		MIX_Audio* LoadAudio(SoundId id, const std::string& file);
		MIX_Audio* GetAudio(SoundId id);

		MIX_Mixer* GetMixer() const { return m_Mixer; }

	protected:
		friend class Singleton<ResourceManager>;
		ResourceManager();

	private:
		std::unordered_map<std::string, std::unique_ptr<Texture2D>> m_LoadedTextures;
		std::unordered_map<std::pair<std::string, float>, std::unique_ptr<Font>> m_LoadedFonts;
		std::unordered_map<CursorId, std::unique_ptr<CustomCursor>> m_LoadedCustomCursors;
		std::unordered_map<SoundId, MIX_Audio*> m_LoadedAudio;
		MIX_Mixer* m_Mixer;
	};
}					