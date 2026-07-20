#include "pch.h"
#include "ButtonSystem.h"
#include "InputHandler.h"
#include "IdCreator.h"
#include "EntityManager.h"
#include "ButtonComponent.h"
#include "BloodTime.h"
#include "SpriteComponent.h"
#include "Texture2D.h"
#include "CustomCoroutine.h"
#include "BloodRenderer.h"

namespace Bloodforge
{
	ButtonSystem::ButtonSystem()
	{
		InputHandler& inputHandler = InputHandler::GetInstance();
		inputHandler.CreateAction(CreateId("LeftMouseButtonDown"), CreateId("MainMap"), BLOODFORGE_KEYCODE_MOUSE_LEFT);
		inputHandler.AddListenerToInputAction(CreateId("LeftMouseButtonDown"), CreateId("MainMap"), [this](const InputActionInfo& actionInfo)
			{ 
				OnLeftMouseButtonDown(actionInfo); 
			});
		inputHandler.CreateAction(CreateId("MouseMove"), CreateId("MainMap"), BLOODFORGE_KEYCODE_MOUSE_MOTION);
		inputHandler.AddListenerToInputAction(CreateId("MouseMove"), CreateId("MainMap"), [this](const InputActionInfo& actionInfo)
			{
				OnMouseMove(actionInfo);
			});
	}

	void ButtonSystem::OnUpdate()
	{
		EntityQueryResult<ButtonComponent, TransformComponent, SpriteComponent> result = EntityManager::GetInstance().QueryEntities<ButtonComponent, TransformComponent, SpriteComponent>();
		for (EntityView<ButtonComponent, TransformComponent, SpriteComponent> entityView : result.EntityViews)
		{
			ButtonComponent& buttonComp = entityView.GetComponent<ButtonComponent>();
			SpriteComponent& spriteComp = entityView.GetComponent<SpriteComponent>();

			if (buttonComp.MouseIsOver && buttonComp.MouseIsPressed)
			{
				spriteComp.SetTexture(buttonComp.PressedTexture);
			}
			else if (buttonComp.MouseIsOver)
			{
				spriteComp.SetTexture(buttonComp.HoverTexture);
			}
			else
			{
				spriteComp.SetTexture(buttonComp.NormalTexture);
			}
		}
	}

	Coroutine ButtonSystem::ScalingCoroutine(int entityId, float timeToReach, const Vector2& startScale, const Vector2& targetScale)
	{
		EntityManager& entityManager = EntityManager::GetInstance();
		float timeCounter = 0.0f;
		while (timeCounter < timeToReach)
		{
			timeCounter += BloodTime::GetInstance().DeltaTime;
			TransformComponent* transformComp = entityManager.GetComponent<TransformComponent>(entityId);
			Vector2 newScale = Vector2::SmoothLerp(startScale, targetScale, timeCounter / timeToReach);
			transformComp->SetLocalScale(newScale);
			co_await WaitUntilNextFrame();
		}
		TransformComponent* transformComp = entityManager.GetComponent<TransformComponent>(entityId);
		transformComp->SetLocalScale(targetScale);
		entityManager.GetComponent<ButtonComponent>(entityId)->CurrentCoroutineId = -1;
	}

	void ButtonSystem::OnLeftMouseButtonDown(const InputActionInfo& actionInfo)
	{
		if (actionInfo.ongoing) return;
		EntityQueryResult<ButtonComponent, TransformComponent, SpriteComponent> result = EntityManager::GetInstance().QueryEntities<ButtonComponent, TransformComponent, SpriteComponent>();
		for (EntityView<ButtonComponent, TransformComponent, SpriteComponent> entityView : result.EntityViews)
		{
			ButtonComponent& buttonComp = entityView.GetComponent<ButtonComponent>();
			TransformComponent& transformComp = entityView.GetComponent<TransformComponent>();
			bool mouseIsOverButton = MouseIsOverButton(buttonComp, transformComp, actionInfo.InputActionVectorData);
			if (!mouseIsOverButton) continue;
			if (actionInfo.started)
			{
				buttonComp.MouseIsPressed = true;
			}
			else if (actionInfo.finished && buttonComp.MouseIsPressed)
			{
				buttonComp.MouseIsPressed = false;
				buttonComp.OnClick.Invoke(buttonComp);
			}
		}
	}

	void ButtonSystem::OnMouseMove(const InputActionInfo& actionInfo)
	{
		EntityQueryResult<ButtonComponent, TransformComponent, SpriteComponent> result = EntityManager::GetInstance().QueryEntities<ButtonComponent, TransformComponent, SpriteComponent>();
		for (EntityView<ButtonComponent, TransformComponent, SpriteComponent> entityView : result.EntityViews)
		{
			ButtonComponent& buttonComp = entityView.GetComponent<ButtonComponent>();
			TransformComponent& transformComp = entityView.GetComponent<TransformComponent>();
			bool mouseIsOverButton = MouseIsOverButton(buttonComp, transformComp, actionInfo.InputActionVectorData);
			if (mouseIsOverButton && !buttonComp.MouseIsOver) // Just started hovering
			{
				buttonComp.MouseIsOver = true;
				if (buttonComp.CurrentCoroutineId != -1)
				{
					StopCoroutine(buttonComp.CurrentCoroutineId);
				}
				buttonComp.CurrentCoroutineId = StartCoroutine(ScalingCoroutine(entityView.EntityId, buttonComp.TimeToReachHoverScale, transformComp.GetLocalScale(), buttonComp.HoverScale));
			}
			else if(!mouseIsOverButton && buttonComp.MouseIsOver) // Just stopped hovering
			{
				buttonComp.MouseIsPressed = false;
				buttonComp.MouseIsOver = false;
				if (buttonComp.CurrentCoroutineId != -1)
				{
					StopCoroutine(buttonComp.CurrentCoroutineId);
				}
				buttonComp.CurrentCoroutineId = StartCoroutine(ScalingCoroutine(entityView.EntityId, buttonComp.TimeToReachNormalScale, transformComp.GetLocalScale(), buttonComp.NormalScale));
			}
		}
	}
	bool ButtonSystem::MouseIsOverButton(const ButtonComponent& buttonComp, TransformComponent& transformComp, const Vector2& mousePos)
	{
		Vector2 mouseButtonWorldPos = BloodRenderer::GetInstance().ScreenToWorldPoint(mousePos);
		Vector2 buttonWorldPos = transformComp.GetWorldPosition();
		Vector2 buttonWorldScale = transformComp.GetWorldScale();
		Vector2 textureSize = (buttonComp.MouseIsOver) ? buttonComp.HoverTexture->GetSize() : buttonComp.NormalTexture->GetSize();
		Vector2 scaledTextureSize = Vector2::Multiply(textureSize, buttonWorldScale);
		Vector2 halfSize = scaledTextureSize / 2.0f;

		Vector2 topLeft = buttonWorldPos - halfSize;
		Vector2 bottomRight = buttonWorldPos + halfSize;

		return (mouseButtonWorldPos.X >= topLeft.X && mouseButtonWorldPos.X <= bottomRight.X &&
			mouseButtonWorldPos.Y >= topLeft.Y && mouseButtonWorldPos.Y <= bottomRight.Y);
	}
}