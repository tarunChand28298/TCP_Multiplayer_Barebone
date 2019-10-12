#include "TCP_Listner/TcpListner.h"
#include "Simulation/Simulation.h"

bool quit;
int currentMaxClientID;
int numberOfPlayersConnected;
std::vector<ClientInput> inputs;
std::vector<PlayerState> players;

class GameServer : public TcpListner
{
	void OnClientConnected(int client) override
	{
		currentMaxClientID++;
		numberOfPlayersConnected++;

		//Push back new player state to players vector:
		PlayerState newPlayer;
		newPlayer.health = 100;
		newPlayer.xPosition = 250;
		newPlayer.yPosition = 250;
		players.push_back(newPlayer);

		//push back new entry into input vector
		ClientInput newInputSlot;
		newInputSlot.attacking = false;
		newInputSlot.jumping = false;
		newInputSlot.xDirection = 0;
		inputs.push_back(newInputSlot);

		//Return clientId to new client:
		std::string pvtMsg = "CLID " + std::to_string(currentMaxClientID - 1) + " ";
		SendMessageToClient(client, pvtMsg.c_str(), pvtMsg.size() + 1);

		//broadcast the new client id to all other clients
		std::string bcastMsg = "CONN " + std::to_string(currentMaxClientID) + " ";
		BroadcastToConnectedClients(client, bcastMsg.c_str(), bcastMsg.size() + 1);
	}

	void OnClientDisconnected(int client) override
	{
		numberOfPlayersConnected--;
		if (numberOfPlayersConnected < 1) { quit = true; }
	}

	void OnMessageReceived(int client, const char* message, int messageLength) override
	{
		std::stringstream receivedMessage;
		receivedMessage << message;

		std::string clientHeader;
		receivedMessage >> clientHeader;

		if (clientHeader == "CLUPDT")
		{
			//extract client id
			int clientId;
			receivedMessage >> clientId;

			//parse input message into clientinput format
			receivedMessage >> inputs[clientId].xDirection;
			receivedMessage >> inputs[clientId].jumping;
			receivedMessage >> inputs[clientId].attacking;
		}
	}
};

int main()
{
	GameServer server;
	server.Initialize("0.0.0.0", 54000);
	
	while (!quit)
	{
		server.Update();

		UpdateSimulation(players, inputs);
		std::string updateBroadcast = PrepareBroadcastMessage(players);

		server.BroadcastToConnectedClients(updateBroadcast.c_str(), updateBroadcast.size() + 1);
	}

	server.Shutdown();
	return 0;
}
