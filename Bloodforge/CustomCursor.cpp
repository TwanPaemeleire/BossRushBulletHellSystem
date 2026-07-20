#include "pch.h"
#include "CustomCursor.h"
#include <SDL3/SDL.h>

namespace Bloodforge
{
	CustomCursor::CustomCursor(SDL_Surface* surface, int hotX, int hotY)
	{
		m_CustomCursor = SDL_CreateColorCursor(surface, hotX, hotY);
	}
}