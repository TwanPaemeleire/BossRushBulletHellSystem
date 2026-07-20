#pragma once
#include "Vector2.h"

struct SDL_Cursor;
struct SDL_Surface;

namespace Bloodforge
{
	class CustomCursor final
	{
	public:
		CustomCursor(SDL_Surface* surface, int hotX, int hotY);

		SDL_Cursor* GetSDLCursor() const { return m_CustomCursor; }
	private:
		Vector2 m_Offset;
		SDL_Cursor* m_CustomCursor{};
	};
}