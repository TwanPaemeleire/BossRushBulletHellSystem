#include "pch.h"
#include "Texture2D.h"
#include <SDL3_image/SDL_image.h>
#include "BloodRenderer.h"
#include <stdexcept>

namespace Bloodforge
{
	Texture2D::Texture2D(SDL_Texture* texture)
		: m_Texture{texture}
	{
		SDL_GetTextureSize(m_Texture, &m_Size.X, &m_Size.Y);
	}

	Texture2D::Texture2D(const std::string& fullPath)
	{
		m_Texture = IMG_LoadTexture(BloodRenderer::GetInstance().GetSDLRenderer(), fullPath.c_str());
		if (m_Texture == nullptr) throw std::runtime_error(std::string("Failed to load texture: ") + SDL_GetError());
		SDL_GetTextureSize(m_Texture, &m_Size.X, &m_Size.Y);
	}

	Texture2D::~Texture2D()
	{
		SDL_DestroyTexture(m_Texture);
	}

	SDL_Texture* Texture2D::GetSDLTexture() const
	{
		return m_Texture;
	}

	float Texture2D::GetWidth() const
	{
		return m_Size.X;
	}

	float Texture2D::GetHeight() const
	{
		return m_Size.Y;
	}

	const Vector2& Texture2D::GetSize() const
	{
		return m_Size;
	}
}