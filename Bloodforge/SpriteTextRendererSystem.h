#pragma once
#include "System.h"

namespace Bloodforge
{
	struct SpriteComponent;
	struct TextComponent;
	struct TransformComponent;
	class SpriteTextRendererSystem final : public System
	{
	public:
		virtual void OnRender() override;
	private:
		void RenderSprite(SpriteComponent& spriteComp, TransformComponent& transform);
		void RenderText(TextComponent& textComp, TransformComponent& transform);
	};
}