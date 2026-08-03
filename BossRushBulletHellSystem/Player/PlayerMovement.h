#pragma once
#include <Component.h>
#include <Vector2.h>

struct PlayerMovement final : public Bloodforge::Component<PlayerMovement>
{
	Bloodforge::Vector2 MovementDirection = { 0.0f, 0.0f };
	float CurrentMovementSpeed = 0.0f;
	float MovementSpeed = 300.0f;
	float FocusMovementSpeed = 15.0f;

	float AccelerationTime = 0.2f;
	float DecelerationTime = 0.1f;

	int SpeedChangeCoroutineId = -1;
};
