#include "PlayerMovementSystem.h"
#include <InputHandler.h>
#include <IdCreator.h>
#include <KeyDefs.h>

using namespace Bloodforge;

void PlayerMovementSystem::OnStart()
{
	InputHandler& inputHandler = InputHandler::GetInstance();

	inputHandler.CreateAction(CreateId("MoveUp"), CreateId("MainMap"), BLOODFORGE_KEYCODE_W);
	
}

void PlayerMovementSystem::ProcessPlayerMovementInput(const InputActionInfo& info)
{
	//if(info.sta)
}