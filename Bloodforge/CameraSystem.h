#pragma once
#include "System.h"
namespace Bloodforge
{
	class CameraSystem final : public System
	{
	public:
		virtual void OnLateUpdate() override;
	};
}