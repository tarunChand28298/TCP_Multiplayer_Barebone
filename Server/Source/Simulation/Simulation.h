#pragma once
#include <vector>
#include <string>
#include <sstream>

#define HorizontalSpeedMultiplier 1
#define JumpForceMultiplier 1
#define Gravity 1
#define FloorHeight 200
#define AttackDistance 50
#define DamagePerTick 5

struct ClientInput
{
	int xDirection;
	bool jumping;
	bool attacking;
};

struct PlayerState
{
	int xPosition;
	int yPosition;
	int xVelocity;
	int yVelocity;
	int health;
};

void UpdateSimulation(std::vector<PlayerState>& players, std::vector<ClientInput>& inputs);
std::string PrepareBroadcastMessage(std::vector<PlayerState> players);