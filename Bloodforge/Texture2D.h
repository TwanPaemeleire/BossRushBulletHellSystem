#pragma once
#include <string>
#include "Vector2.h"
struct SDL_Texture;

namespace Bloodforge
{
	class Texture2D final
	{
	public:
		explicit Texture2D(SDL_Texture* texture);
		explicit Texture2D(const std::string& fullPath);
		~Texture2D();
		Texture2D(const Texture2D&) = delete;
		Texture2D(Texture2D&&) = delete;
		Texture2D& operator= (const Texture2D&) = delete;
		Texture2D& operator= (const Texture2D&&) = delete;

		SDL_Texture* GetSDLTexture() const;

		float GetWidth() const;
		float GetHeight() const;
		const Vector2& GetSize() const;
	private:
		SDL_Texture* m_Texture;
		Vector2 m_Size;
	};
}