#include "pch.h"
#include "SpriteComponent.h"
#include "Texture2D.h"

namespace Bloodforge
{
	void SpriteComponent::SetTexture(Texture2D* texture)
	{
		Texture = texture;
		if (texture && !HasCustomSourceRect)
		{
			SourceRect.X = 0;
			SourceRect.Y = 0;
			SourceRect.Width = Texture->GetWidth();
			SourceRect.Height = Texture->GetHeight();
		}
	}

	void SpriteComponent::SetCustomSourceRect(const Rect& sourceRect)
	{
		SourceRect = sourceRect;
		HasCustomSourceRect = true;
	}

	void SpriteComponent::ResetCustomSourceRect()
	{
		HasCustomSourceRect = false;
		SourceRect.X = 0;
		SourceRect.Y = 0;
		SourceRect.Width = (Texture) ? Texture->GetWidth() : 0.0f;
		SourceRect.Height = (Texture) ? Texture->GetHeight() : 0.0f;
	}
}