#pragma once
#include "Component.h"
#include <string>
#include "Color.h"
#include <memory>
#include "Texture2D.h"
#include "Font.h"

namespace Bloodforge
{
	class Font;
	struct TextComponent final : public Component<TextComponent>
	{
		void SetText(const std::string& text)
		{
			Text = text;
			TextNeedsUpdate = true;
		}

		void SetFontSize(float size)
		{
			FontSize = size;
			FontNeedsUpdate = true;
			TextNeedsUpdate = true;
		}

		void SetColor(const Color& color)
		{
			Color = color;
			TextNeedsUpdate = true;
		}

		void SetFont(Font* font)
		{
			Font = font;
			FontSize = font->GetFontSize();
			TextNeedsUpdate = true;
		}

		void SetTexture(std::unique_ptr<Texture2D> texture)
		{
			Texture = std::move(texture);
			TextNeedsUpdate = false;
		}

		void CompleteUpdate()
		{
			TextNeedsUpdate = false;
			FontNeedsUpdate = false;
		}

		bool TextDoesNeedUpdate() const { return TextNeedsUpdate; }
		bool FontDoesNeedUpdate() const { return FontNeedsUpdate; }
		Texture2D* GetTexture() const { return Texture.get(); }
		Font* GetFont() const { return Font; }
		float GetFontSize() const { return FontSize; }
		const std::string& GetText() const { return Text; }
		const Color& GetColor() const { return Color; }

		bool FlipHorizontal = false;
		bool FlipVertical = false;
		int DrawOrder = 0; // Higher values are drawn on top of lower values

	private:
		std::string Text = "";
		float FontSize = 0.0f;
		Color Color = {};
		Font* Font = nullptr;
		std::unique_ptr<Texture2D> Texture;
		bool TextNeedsUpdate = false;
		bool FontNeedsUpdate = false;
	};
}