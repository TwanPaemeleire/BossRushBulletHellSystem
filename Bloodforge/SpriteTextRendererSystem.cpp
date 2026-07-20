#include "pch.h"
#include "SpriteTextRendererSystem.h"
#include "EntityManager.h"
#include "TextComponent.h"
#include "TransformComponent.h"
#include "SpriteComponent.h"
#include <map>
#include <set>
#include "BloodRenderer.h"

void Bloodforge::SpriteTextRendererSystem::OnRender()
{
	EntityQueryResult<SpriteComponent, TransformComponent> spriteResult = EntityManager::GetInstance().QueryEntities<SpriteComponent, TransformComponent>();
	EntityQueryResult<TextComponent, TransformComponent> textResult = EntityManager::GetInstance().QueryEntities<TextComponent, TransformComponent>();
	std::map<int, std::vector<std::pair<SpriteComponent*, TransformComponent*>>> spriteLayers;
	std::map<int, std::vector<std::pair<TextComponent*, TransformComponent*>>> textLayers;

    std::map<int, std::vector<int>> spriteEntities;
	std::map<int, std::vector<int>> textEntities;

	std::set<int> allDrawOrders;

    // Collect sprites
    for (auto& view : spriteResult.EntityViews)
    {
        auto& spriteComp = view.GetComponent<SpriteComponent>();
        auto& transform = view.GetComponent<TransformComponent>();
		allDrawOrders.insert(spriteComp.DrawOrder);
        spriteLayers[spriteComp.DrawOrder].emplace_back(&spriteComp, &transform);
    }

    // Collect text
    for (auto& view : textResult.EntityViews)
    {
        auto& textComp = view.GetComponent<TextComponent>();
        auto& transform = view.GetComponent<TransformComponent>();
		allDrawOrders.insert(textComp.DrawOrder);
        textLayers[textComp.DrawOrder].emplace_back(&textComp, &transform);
    }

    // Render all sprites and text in order
    for (int drawOrder : allDrawOrders)
    {
        if (spriteLayers.find(drawOrder) != spriteLayers.end())
        {
            for (auto& [spriteComp, transform] : spriteLayers[drawOrder])
            {
                RenderSprite(*spriteComp, *transform);
            }
        }
        if (textLayers.find(drawOrder) != textLayers.end())
        {
            for (auto& [textComp, transform] : textLayers[drawOrder])
            {
                RenderText(*textComp, *transform);
            }
		}
    }
}

void Bloodforge::SpriteTextRendererSystem::RenderSprite(SpriteComponent& spriteComp, TransformComponent& transform)
{
	if (spriteComp.GetTexture() == nullptr) return;
	BloodRenderer::GetInstance().RenderTexture(*spriteComp.GetTexture(), transform.GetWorldMatrix(), spriteComp.GetSourceRect(), spriteComp.Color, spriteComp.FlipHorizontal, spriteComp.FlipVertical);
}

void Bloodforge::SpriteTextRendererSystem::RenderText(TextComponent& textComp, TransformComponent& transform)
{
	if (textComp.GetTexture() == nullptr) return;
	BloodRenderer::GetInstance().RenderTexture(*textComp.GetTexture(), transform.GetWorldMatrix(), textComp.GetColor(), textComp.FlipHorizontal, textComp.FlipVertical);
}