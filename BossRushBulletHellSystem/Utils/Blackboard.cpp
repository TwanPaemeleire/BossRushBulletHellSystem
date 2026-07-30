#include "Blackboard.h"
#include <Vector2.h>

using namespace Bloodforge;

Blackboard::Blackboard()
{
	Register<float>("float");
	Register<int>("int");
	Register<bool>("bool");
	Register<Vector2>("vec2");
}
