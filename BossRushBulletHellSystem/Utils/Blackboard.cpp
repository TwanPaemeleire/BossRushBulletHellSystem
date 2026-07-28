#include "Blackboard.h"

Blackboard::Blackboard()
{
	Register<float>("float");
	Register<int>("int");
	Register<bool>("bool");
}
