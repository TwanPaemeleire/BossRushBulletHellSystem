#pragma once
#include "System.h"
#include "Vector2.h"

namespace Bloodforge
{
	struct InputActionInfo;
	struct ButtonComponent;
	struct TransformComponent;
	struct Coroutine;

	class ButtonSystem final : public System
	{
	public:
		ButtonSystem();
		virtual void OnUpdate() override;

	private:
		Coroutine ScalingCoroutine(int entityId, float timeToReach, const Vector2& startScale, const Vector2& targetScale);

		void OnLeftMouseButtonDown(const InputActionInfo& actionInfo);
		void OnMouseMove(const InputActionInfo& actionInfo);

		bool MouseIsOverButton(const ButtonComponent& buttonComp, TransformComponent& transformComp, const Vector2& mousePos);
	};
}