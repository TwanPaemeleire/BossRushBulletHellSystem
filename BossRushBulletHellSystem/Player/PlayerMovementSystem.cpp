#include "PlayerMovementSystem.h"
#include <InputHandler.h>
#include <IdCreator.h>
#include <KeyDefs.h>
#include "PlayerMovement.h"
#include <CustomCoroutine.h>
#include <EntityManager.h>
#include <TransformComponent.h>
#include <BloodTime.h>
#include <WindowUtils.h>
#include <SpriteComponent.h>
#include <MathUtils.h>

using namespace Bloodforge;

void PlayerMovementSystem::OnStart()
{
	InputHandler& inputHandler = InputHandler::GetInstance();

	inputHandler.CreateAction(CreateId("MoveUp"), CreateId("MainMap"), BLOODFORGE_KEYCODE_W);
	inputHandler.CreateAction(CreateId("MoveDown"), CreateId("MainMap"), BLOODFORGE_KEYCODE_S);
	inputHandler.CreateAction(CreateId("MoveLeft"), CreateId("MainMap"), BLOODFORGE_KEYCODE_A);
	inputHandler.CreateAction(CreateId("MoveRight"), CreateId("MainMap"), BLOODFORGE_KEYCODE_D);

	inputHandler.AddListenerToInputAction(CreateId("MoveUp"), CreateId("MainMap"), [this](const InputActionInfo& info) { ProcessPlayerMovementInput(info); });
	inputHandler.AddListenerToInputAction(CreateId("MoveDown"), CreateId("MainMap"), [this](const InputActionInfo& info) { ProcessPlayerMovementInput(info); });
	inputHandler.AddListenerToInputAction(CreateId("MoveLeft"), CreateId("MainMap"), [this](const InputActionInfo& info) { ProcessPlayerMovementInput(info); });
	inputHandler.AddListenerToInputAction(CreateId("MoveRight"), CreateId("MainMap"), [this](const InputActionInfo& info) { ProcessPlayerMovementInput(info); });
}

void PlayerMovementSystem::OnUpdate()
{
	auto result = EntityManager::GetInstance().GetFirstEntityWithComponents<PlayerMovement, TransformComponent, SpriteComponent>().value();
	PlayerMovement& playerMovement = result.GetComponent<PlayerMovement>();
	if (playerMovement.CurrentMovementSpeed == 0.0f) return;
	TransformComponent& transform = result.GetComponent<TransformComponent>();
	Vector2 newPos = transform.GetLocalPosition() + playerMovement.MovementDirection * playerMovement.CurrentMovementSpeed * BloodTime::GetInstance().DeltaTime;
	
	SpriteComponent& sprite = result.GetComponent<SpriteComponent>();
	Vector2 frameSize = { sprite.GetSourceRect().Width, sprite.GetSourceRect().Height };

	Vector2 windowSize = WindowUtils::GetWindowSize();

	// Check if within window bounds, position is center of sprite
	if (newPos.X - frameSize.X / 2 < 0) newPos.X = frameSize.X / 2; // Left bound
	if (newPos.X + frameSize.X / 2 > windowSize.X) newPos.X = windowSize.X - frameSize.X / 2; // Right bound
	if (newPos.Y - frameSize.Y / 2 < 0) newPos.Y = frameSize.Y / 2; // Top bound
	if (newPos.Y + frameSize.Y / 2 > windowSize.Y) newPos.Y = windowSize.Y - frameSize.Y / 2; // Bottom bound

	transform.SetLocalPosition(newPos);
}

void PlayerMovementSystem::ProcessPlayerMovementInput(const InputActionInfo& info)
{
	if (!info.started && !info.finished) return;
	PlayerMovement& playerMovement = EntityManager::GetInstance().GetFirstEntityWithComponents<PlayerMovement>().value().GetComponent<PlayerMovement>();
	Vector2 newMovementDirection = playerMovement.MovementDirection;
	switch (info.Button)
	{
	case BLOODFORGE_KEYCODE_W:
		newMovementDirection.Y = (info.started) ? -1.0f : 0.0f;
		break;
	case BLOODFORGE_KEYCODE_S:
		newMovementDirection.Y = (info.started) ? 1.0f : 0.0f;
		break;
	case BLOODFORGE_KEYCODE_A:
		newMovementDirection.X = (info.started) ? -1.0f : 0.0f;
		break;
	case BLOODFORGE_KEYCODE_D:
		newMovementDirection.X = (info.started) ? 1.0f : 0.0f;
		break;
	}

	bool isMovementStart = playerMovement.MovementDirection == Vector2(0.0f, 0.0f) && info.started;
	bool isMovementStop = playerMovement.MovementDirection != Vector2(0.0f, 0.0f) && newMovementDirection == Vector2(0.0f, 0.0f);
	if (isMovementStart || isMovementStop)
	{
		if (playerMovement.SpeedChangeCoroutineId != -1) StopCoroutine(playerMovement.SpeedChangeCoroutineId);
	}
	if (isMovementStart) playerMovement.SpeedChangeCoroutineId = StartCoroutine(ChangeSpeedGradually(true));
	else if (isMovementStop) playerMovement.SpeedChangeCoroutineId = StartCoroutine(ChangeSpeedGradually(false));

	playerMovement.MovementDirection = newMovementDirection;
}

Coroutine PlayerMovementSystem::ChangeSpeedGradually(bool accelerate)
{
	PlayerMovement& playerMovement = EntityManager::GetInstance().GetFirstEntityWithComponents<PlayerMovement>().value().GetComponent<PlayerMovement>();

	float timer = 0.0f;
	float currentSpeed = playerMovement.CurrentMovementSpeed;
	float targetSpeed = (accelerate) ? playerMovement.MovementSpeed : 0.0f;
	while (timer < ((accelerate) ? playerMovement.AccelerationTime : playerMovement.DecelerationTime))
	{
		timer += BloodTime::GetInstance().DeltaTime;
		playerMovement.CurrentMovementSpeed = MathUtils::SmoothLerp(currentSpeed, targetSpeed, timer / ((accelerate) ? playerMovement.AccelerationTime : playerMovement.DecelerationTime));
		co_await WaitUntilNextFrame();
	}
	PlayerMovement& playerMovementUpdated = EntityManager::GetInstance().GetFirstEntityWithComponents<PlayerMovement>().value().GetComponent<PlayerMovement>();
	playerMovementUpdated.SpeedChangeCoroutineId = -1;
}
