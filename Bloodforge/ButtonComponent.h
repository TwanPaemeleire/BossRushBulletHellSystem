#pragma once
#include "Component.h"
#include "Vector2.h"
#include "Event.h"

namespace Bloodforge
{
	class Font;
	class Texture2D;
	struct ButtonComponent final : public Component<ButtonComponent>
	{
		Texture2D* NormalTexture;
		Texture2D* HoverTexture;
		Texture2D* PressedTexture;

		bool MouseIsOver = false;
		bool MouseIsPressed = false;

		Vector2 NormalScale;
		Vector2 HoverScale;
		float TimeToReachHoverScale = 0.1f;
		float TimeToReachNormalScale = 0.1f;

		Event<ButtonComponent&> OnClick;

		int CurrentCoroutineId = -1;
	};
}