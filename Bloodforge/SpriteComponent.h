#pragma once
#include "Component.h"
#include "Color.h"
#include "Rect.h"

namespace Bloodforge
{
	class Texture2D;
	struct SpriteComponent final : public Component<SpriteComponent>
	{
		SpriteComponent() = default;
		~SpriteComponent() = default;

		Texture2D* GetTexture() const { return Texture; }
		const Rect& GetSourceRect() const { return SourceRect; }

		void SetTexture(Texture2D* texture);
		void SetCustomSourceRect(const Rect& sourceRect);
		void ResetCustomSourceRect();

		Color Color{};
		bool FlipHorizontal = false;
		bool FlipVertical = false;
		int DrawOrder = 0; // Higher values are drawn on top of lower values
	private:
		Rect SourceRect{};
		Texture2D* Texture = nullptr;
		bool HasCustomSourceRect = false;
	};
}