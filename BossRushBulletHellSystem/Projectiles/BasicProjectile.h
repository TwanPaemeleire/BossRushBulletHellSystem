#pragma once
#include <Component.h>
#include <Vector2.h>

struct BasicProjectile final : public Bloodforge::Component<BasicProjectile>
{
	float Speed = 5.0f;
};
