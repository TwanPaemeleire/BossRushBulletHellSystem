#pragma once
#include "System.h"
#include <functional>

namespace Bloodforge
{
	struct SpriteAnimatorComponent;

	class SpriteAnimatorSystem final : public System
	{
	public:
		virtual void OnStart() override;
		virtual void OnUpdate() override;

	private:
		void HandleAnimationEvents(SpriteAnimatorComponent& spriteAnimComp);
		void ResetAllAnimationEvents(SpriteAnimatorComponent& spriteAnimComp);
	};
}