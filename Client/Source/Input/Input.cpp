#include "Input.h"
#include <Windows.h>

std::string PrepareMessage(int clientId, const UserInput& input)
{
	std::stringstream outputMessage;

	outputMessage << "CLUPDT ";
	outputMessage << clientId << " ";
	outputMessage << input.xDirection << " ";
	outputMessage << input.jump << " ";
	outputMessage << input.attack;

	return outputMessage.str();
}

void UpdateInput(UserInput& input)
{
	input.xDirection = 0;
	input.jump = false;
	input.attack = false;

	if (GetAsyncKeyState(VK_LEFT)) { input.xDirection -= 1; }
	if (GetAsyncKeyState(VK_RIGHT)) { input.xDirection += 1; }
	if (GetAsyncKeyState(VK_SPACE)) { input.jump = true; }
	if (GetAsyncKeyState(VK_SHIFT)) { input.attack = true; }
}
