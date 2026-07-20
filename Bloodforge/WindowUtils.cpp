#include "pch.h"
#include "WindowUtils.h"
#include <SDL3/SDL.h>
#include "BloodRenderer.h"
#include "CustomCursor.h"
#include <SDL3/SDL_mouse.h>
#include <SDL3_image/SDL_image.h>
#include <stdexcept>

namespace Bloodforge::WindowUtils
{
	void SetWindowSize(int sizeX, int sizeY)
	{
		BloodRenderer::GetInstance().SetWindowSize(sizeX, sizeY);
	}

	Vector2 GetWindowSize()
	{
		int sizeX;
		int sizeY;
		SDL_GetWindowSize(BloodRenderer::GetInstance().GetSDLWindow(), &sizeX, &sizeY);
		return Vector2(static_cast<float>(sizeX), static_cast<float>(sizeY));
	}

	void SetWindowAlwaysOnTop(bool alwaysOnTop)
	{
		SDL_SetWindowAlwaysOnTop(BloodRenderer::GetInstance().GetSDLWindow(), alwaysOnTop);
	}

	void SetWindowBordered(bool borderless)
	{
		SDL_SetWindowBordered(BloodRenderer::GetInstance().GetSDLWindow(), borderless);
	}

	void SetWindowFullScreen(bool fullscreen)
	{
		SDL_SetWindowFullscreen(BloodRenderer::GetInstance().GetSDLWindow(), fullscreen);
	}

	void SetCustomCursor(CustomCursor* customCursor)
	{
		SDL_SetCursor(customCursor->GetSDLCursor());
	}

	void SetWindowIcon(const std::string& path)
	{
		SDL_Surface* surface = IMG_Load(path.c_str());
		if (!surface)
		{
			throw std::runtime_error(SDL_GetError());
		}

		SDL_SetWindowIcon(BloodRenderer::GetInstance().GetSDLWindow(), surface);
		SDL_DestroySurface(surface);
	}

	void SetWindowTitle(const std::string& title)
	{
		SDL_SetWindowTitle(BloodRenderer::GetInstance().GetSDLWindow(), title.c_str());
		SDL_GetWindowPosition(BloodRenderer::GetInstance().GetSDLWindow(), nullptr, nullptr);
	}

	Vector2 GetWindowPosition()
	{
		int x;
		int y;
		SDL_GetWindowPosition(BloodRenderer::GetInstance().GetSDLWindow(), &x, &y);
		return Bloodforge::Vector2(static_cast<float>(x), static_cast<float>(y));
	}

	void SetWindowPosition(int posX, int posY)
	{
		SDL_SetWindowPosition(BloodRenderer::GetInstance().GetSDLWindow(), posX, posY);
	}
}

