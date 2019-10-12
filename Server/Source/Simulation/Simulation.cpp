#include "Simulation.h"
#include <string>
#include <iostream>

void UpdateSimulation(std::vector<PlayerState>& players, std::vector<ClientInput>& inputs)
{
	for (int i = 0; i < players.size(); i++)
	{
		//Horizontal velocity:
		players[i].xVelocity = inputs[i].xDirection * HorizontalSpeedMultiplier;

		//Vertical velocity:
		if (inputs[i].jumping) { players[i].yVelocity += JumpForceMultiplier; }
		if (players[i].yPosition > FloorHeight) { players[i].yVelocity -= Gravity; }

		//Attacking:
		if (inputs[i].attacking)
		{
			//Critical section start
			for (int j = 0; j < players.size(); j++) {
				if (i == j) { continue; }
				if (abs(players[i].xPosition - players[j].xPosition) < AttackDistance)
				{
					players[j].health -= DamagePerTick;
				}
			}
			//Critical section end
		}

		//Euler integration:
		players[i].xPosition += players[i].xVelocity;
		players[i].yPosition += players[i].yVelocity;

		//Overshoot correction:
		if (players[i].yPosition < FloorHeight) { players[i].yVelocity = 0; players[i].yPosition = FloorHeight; }
	}

	//std::cout << "Updated " << players.size() << " players" << std::endl;
}

std::string PrepareBroadcastMessage(std::vector<PlayerState> players)
{
	std::stringstream messageStream;

	messageStream << "UPDT ";
	messageStream << players.size() << " ";
	for (int i = 0; i < players.size(); i++) {
		messageStream << players[i].xPosition << " ";
		messageStream << players[i].yPosition << " ";
		messageStream << players[i].health << " ";
	}

	return messageStream.str();
}
