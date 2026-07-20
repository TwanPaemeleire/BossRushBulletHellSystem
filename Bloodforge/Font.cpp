#include "pch.h"
#include "Font.h"
#include <SDL3_ttf/SDL_ttf.h>
#include <stdexcept>	

namespace Bloodforge
{
	Bloodforge::Font::Font(const std::string& path, float fontSize)
	{
		m_FontPath = path;
		m_Fontsize = fontSize;
		m_Font = TTF_OpenFont(path.c_str(), fontSize);
		if (m_Font == nullptr)
		{
			throw std::runtime_error(std::string("Loading font failed: ") + SDL_GetError());
		}
	}

	Bloodforge::Font::~Font()
	{
		TTF_CloseFont(m_Font);
	}
}