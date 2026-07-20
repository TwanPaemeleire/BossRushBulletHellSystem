#include "pch.h"
#include "TextSystem.h"
#include "EntityManager.h"
#include "TextComponent.h"
#include "TransformComponent.h"
#include "BloodRenderer.h"
#include <SDL3_ttf/SDL_ttf.h>
#include "ResourceManager.h"

namespace Bloodforge
{
	void TextSystem::OnUpdate()
	{
		EntityQueryResult<TextComponent, TransformComponent> result = EntityManager::GetInstance().QueryEntities<TextComponent, TransformComponent>();
		for (EntityView<TextComponent, TransformComponent>& view : result.EntityViews)
		{
			TextComponent& textComp = view.GetComponent<TextComponent>();
			if (textComp.TextDoesNeedUpdate())
			{
				const Color& color = textComp.GetColor();
				SDL_Color sdlColor = {color.r, color.g, color.b, color.a};

				if (textComp.FontDoesNeedUpdate())
				{
					textComp.SetFont(ResourceManager::GetInstance().LoadFont(textComp.GetFont()->GetFontPath(), textComp.GetFontSize()));
				}

				const auto surf = TTF_RenderText_Blended(textComp.GetFont()->GetFontPointer(), textComp.GetText().c_str(), 0, sdlColor);
				if (surf == nullptr)
				{
					throw std::runtime_error(std::string("Render text failed: ") + SDL_GetError());
				}
				const auto texture = SDL_CreateTextureFromSurface(BloodRenderer::GetInstance().GetSDLRenderer(), surf);
				if (texture == nullptr)
				{
					throw std::runtime_error(std::string("Create text texture from surface failed: ") + SDL_GetError());
				}
				SDL_DestroySurface(surf);
				textComp.SetTexture(std::make_unique<Texture2D>(texture));
				textComp.CompleteUpdate();
			}
		}
	}
}