#pragma once
#include "Singleton.h"
#include "Color.h"
#include "glm/glm.hpp"

struct SDL_Renderer;
struct SDL_Window;
struct SDL_FPoint;

namespace Bloodforge
{
	struct ColliderRect;
	struct Vector2;
	struct Rect;
	class Texture2D;

	class BloodRenderer : public Singleton<BloodRenderer>
	{
	public:
		BloodRenderer();
		void Cleanup();
		void Render();

		void DrawRectangle(const Vector2& pos, float width, float height, const Color& color) const;
		void DrawRectangle(const ColliderRect& rect, const Color& color) const;
		void RenderTexture(const Texture2D& texture, const Vector2& pos, const Color& color) const;
		void RenderTexture(const Texture2D& texture, const glm::mat4& worldMatrix, const Color& color, bool flippedHorizontal, bool flippedVertical) const;
		void RenderTexture(const Texture2D& texture, const glm::mat4& worldMatrix, const Rect& srcRect, const Color& color, bool flippedHorizontal, bool flippedVertical) const;

		SDL_Renderer* GetSDLRenderer() const;
		const Color& GetBackgroundColor() const { return m_ClearColor; }
		SDL_Window* GetSDLWindow() const { return m_Window; }
		void SetBackgroundColor(const Color& color) { m_ClearColor = color; }

		int GetWindowWidth() const { return m_WindowWidth; }
		int GetWindowHeight() const { return m_WindowHeight; }
		void SetWindowSize(int width, int height);

		Vector2 ScreenToWorldPoint(Vector2 screen) const;
	private:
		SDL_FPoint TransformPoint(const glm::mat4& worldMatrix, float x, float y) const;
		Vector2 GetCameraPosition() const;

		SDL_Renderer* m_Renderer{};
		SDL_Window* m_Window{};
		Color m_ClearColor{0,0,0,255};

		int m_WindowWidth{};
		int m_WindowHeight{};
	};
}