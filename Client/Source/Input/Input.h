#pragma once
#include <string>
#include <sstream>

struct UserInput
{
	int xDirection;
	bool jump;
	bool attack;
};

std::string PrepareMessage(int clientId, const UserInput& input);
void UpdateInput(UserInput& input);
