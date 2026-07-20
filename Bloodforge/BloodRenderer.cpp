#include "pch.h"
#include "BloodRenderer.h"
#include <SDL3/SDL_render.h>
#include <stdexcept>
#include "Texture2D.h"
#include "SceneSystemManager.h"
#include "Vector2.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_pixels.h>
#include "glm/glm.hpp"
#include "RectColliderComponent.h"
#include "Rect.h"
#include "EntityManager.h"
#include "CameraComponent.h"

namespace Bloodforge
{
	BloodRenderer::BloodRenderer()
	{
		if (!SDL_Init(SDL_INIT_VIDEO))
		{
			throw std::runtime_error(std::string("SDL_Init Error: ") + SDL_GetError());
		}

		m_Window = SDL_CreateWindow("Bloodforge", 800, 800, SDL_WINDOW_OPENGL);
		if (m_Window == nullptr)
		{
			throw std::runtime_error(std::string("SDL_CreateWindow Error: ") + SDL_GetError());
		}

		m_Renderer = SDL_CreateRenderer(m_Window, NULL);
		if (m_Renderer == nullptr)
		{
			throw std::runtime_error(std::string("SDL_CreateRenderer Error: ") + SDL_GetError());
		}
		SDL_GetWindowSizeInPixels(m_Window, &m_WindowWidth, &m_WindowHeight);
	}

	void BloodRenderer::Cleanup()
	{
		SDL_QuitSubSystem(SDL_INIT_VIDEO);
		SDL_DestroyRenderer(m_Renderer);
		SDL_DestroyWindow(m_Window);
		SDL_Quit();
	}

	void BloodRenderer::Render()
	{
		const Color& color = GetBackgroundColor();
		SDL_SetRenderDrawColor(m_Renderer, color.r, color.g, color.b, color.a);
		SDL_RenderClear(m_Renderer);

		SceneSystemManager::GetInstance().Render();
		SceneSystemManager::GetInstance().RenderUI();

		SDL_RenderPresent(m_Renderer);
	}

	void BloodRenderer::DrawRectangle(const Vector2& pos, float width, float height, const Color& color) const
	{
		SDL_SetRenderDrawColor(m_Renderer, color.r, color.g, color.b, color.a);
		Vector2 screenPos = pos - GetCameraPosition();
		SDL_FRect rect{};
		rect.x = screenPos.X;
		rect.y = screenPos.Y;
		rect.w = width;
		rect.h = height;

		SDL_RenderRect(m_Renderer, &rect);
	}

	void BloodRenderer::DrawRectangle(const ColliderRect& rect, const Color& color) const
	{
		Vector2 camPos = GetCameraPosition();
		Vector2 topLeft = rect.TopLeft - camPos;
		Vector2 topRight = rect.TopRight - camPos;
		Vector2 bottomLeft = rect.BottomLeft - camPos;
		Vector2 bottomRight = rect.BottomRight - camPos;

		SDL_FPoint linePoints[5] = 
		{
			{ topLeft.X,topLeft.Y },
			{ topRight.X, topRight.Y },
			{ bottomRight.X, bottomRight.Y },
			{ bottomLeft.X, bottomLeft.Y },
			{ topLeft.X, topLeft.Y }
		};

		SDL_SetRenderDrawColor(m_Renderer, color.r, color.g, color.b, color.a);
		SDL_RenderLines(GetSDLRenderer(), linePoints, 5);
	}

	void BloodRenderer::RenderTexture(const Texture2D& texture, const Vector2& pos, const Color& color) const
	{
		Vector2 screenPos = pos - GetCameraPosition();
		SDL_FRect dstRect{};
		dstRect.x = screenPos.X;
		dstRect.y = screenPos.Y;
		dstRect.w = texture.GetWidth();
		dstRect.h = texture.GetHeight();
		SDL_SetTextureColorMod(texture.GetSDLTexture(), color.r, color.g, color.b);
		SDL_SetTextureAlphaMod(texture.GetSDLTexture(), color.a);
		SDL_RenderTexture(GetSDLRenderer(), texture.GetSDLTexture(), nullptr, &dstRect);
	}

	void BloodRenderer::RenderTexture(const Texture2D& texture, const glm::mat4& worldMatrix, const Color& color, bool flipHorizontal, bool flipVertical) const
	{
		const float width = texture.GetWidth();
		const float height = texture.GetHeight();
		float halfWidth = width * 0.5f;
		float halfHeight = height * 0.5f;

		(flipHorizontal) ? halfWidth *= -1 : halfWidth;
		(flipVertical) ? halfHeight *= -1 : halfHeight;

		SDL_Vertex vertices[4]{};
		SDL_FColor sdlColor = 
		{ 
			color.r / 255.f, 
			color.g / 255.f, 
			color.b / 255.f, 
			color.a / 255.f 
		};

		vertices[0].position = TransformPoint(worldMatrix, -halfWidth, -halfHeight);
		vertices[0].tex_coord = SDL_FPoint{ 0.0f, 0.0f };
		vertices[0].color = sdlColor;

		vertices[1].position = TransformPoint(worldMatrix, halfWidth, -halfHeight);
		vertices[1].tex_coord = SDL_FPoint{ 1.0f, 0.0f };
		vertices[1].color = sdlColor;

		vertices[2].position = TransformPoint(worldMatrix, halfWidth, halfHeight);
		vertices[2].tex_coord = SDL_FPoint{ 1.0f, 1.0f };
		vertices[2].color = sdlColor;

		vertices[3].position = TransformPoint(worldMatrix, -halfWidth, halfHeight);
		vertices[3].tex_coord = SDL_FPoint{ 0.0f, 1.0f };
		vertices[3].color = sdlColor;

		int indices[6] = { 0, 1, 2, 0, 2, 3 };

		SDL_RenderGeometry(GetSDLRenderer(), texture.GetSDLTexture(), vertices, 4, indices, 6);
	}

	void BloodRenderer::RenderTexture(const Texture2D& texture, const glm::mat4& worldMatrix, const Rect& srcRect, const Color& color, bool flipHorizontal, bool flipVertical) const
	{
		const float textureWidth = texture.GetWidth();
		const float textureHeight = texture.GetHeight();

		const float width = srcRect.Width;
		const float height = srcRect.Height;
		float halfWidth = width * 0.5f;
		float halfHeight = height * 0.5f;

		(flipHorizontal) ? halfWidth *= -1 : halfWidth;
		(flipVertical) ? halfHeight *= -1 : halfHeight;

		const float u0 = srcRect.X / textureWidth;
		const float v0 = srcRect.Y / textureHeight;
		const float u1 = (srcRect.X + srcRect.Width) / textureWidth;
		const float v1 = (srcRect.Y + srcRect.Height) / textureHeight;

		SDL_Vertex vertices[4]{};
		SDL_FColor sdlColor =
		{
			color.r / 255.f,
			color.g / 255.f,
			color.b / 255.f,
			color.a / 255.f
		};

		vertices[0].position = TransformPoint(worldMatrix, -halfWidth, -halfHeight);
		vertices[0].tex_coord = SDL_FPoint{ u0, v0 };
		vertices[0].color = sdlColor;

		vertices[1].position = TransformPoint(worldMatrix, halfWidth, -halfHeight);
		vertices[1].tex_coord = SDL_FPoint{ u1, v0 };
		vertices[1].color = sdlColor;

		vertices[2].position = TransformPoint(worldMatrix, halfWidth, halfHeight);
		vertices[2].tex_coord = SDL_FPoint{ u1, v1 };
		vertices[2].color = sdlColor;

		vertices[3].position = TransformPoint(worldMatrix, -halfWidth, halfHeight);
		vertices[3].tex_coord = SDL_FPoint{ u0, v1 };
		vertices[3].color = sdlColor;

		int indices[6] = { 0, 1, 2, 0, 2, 3 };

		SDL_RenderGeometry(GetSDLRenderer(), texture.GetSDLTexture(), vertices, 4, indices, 6);
	}

	SDL_Renderer* BloodRenderer::GetSDLRenderer() const
	{
		if (!m_Renderer) throw std::runtime_error("Renderer not initialized yet!");
		return m_Renderer;
	}

	void BloodRenderer::SetWindowSize(int width, int height)
	{
		m_WindowWidth = width;
		m_WindowHeight = height;
		SDL_SetWindowSize(BloodRenderer::GetInstance().GetSDLWindow(), width, height);
	}

	Vector2 BloodRenderer::ScreenToWorldPoint(Vector2 screen) const
	{
		Vector2 cameraPos = GetCameraPosition();
		return Vector2
		{
			screen.X + cameraPos.X,
			screen.Y + cameraPos.Y
		};
	}

	SDL_FPoint BloodRenderer::TransformPoint(const glm::mat4& worldMatrix, float x, float y) const
	{
		glm::vec4 point = worldMatrix * glm::vec4(x, y, 0.0f, 1.0f);
		Vector2 cameraPos = GetCameraPosition();
		return SDL_FPoint{ point.x - cameraPos.X, point.y - cameraPos.Y };
	}

	Vector2 BloodRenderer::GetCameraPosition() const
	{
		EntityView<CameraComponent, TransformComponent> view = EntityManager::GetInstance().GetOrCreateFirstEntityWithComponents<CameraComponent, TransformComponent>();
		TransformComponent& transformComp = view.GetComponent<TransformComponent>();
		return transformComp.GetWorldPosition();
	}
}