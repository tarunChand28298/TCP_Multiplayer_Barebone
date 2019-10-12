#include "TCP_Client/TcpClient.h"
#include "Input/Input.h"
#include "Display/Display.h"

struct Player
{
	int xPosition;
	int yPosition;
	int health;
};

int ClientId;
std::vector<Player> players;
UserInput input;

class GameClient : public TcpClient
{	
	void OnMessageReceived(int server, const char* message, int messageLength) override
	{
		std::stringstream incomingMsg;
		incomingMsg << message;

		std::string command;
		incomingMsg >> command;

		if (command == "CLID")	//Server sent us a client id
		{
			incomingMsg >> ClientId;

			Player newPlayer;
			newPlayer.health = 100;
			newPlayer.xPosition = 250;
			newPlayer.yPosition = 250;
			players.push_back(newPlayer);
		}

		if (command == "CONN")	//New player joined
		{
			Player newPlayer;
			newPlayer.health = 100;
			newPlayer.xPosition = 250;
			newPlayer.yPosition = 250;
			players.push_back(newPlayer);
		}

		if (command == "UPDT")	//Server updated one frame
		{
			int numberOfPlayers;

			incomingMsg >> numberOfPlayers;
			if (numberOfPlayers != players.size()) {
				//Out of sync with server. resize the vector.
				players.clear();
				for (int i = 0; i < numberOfPlayers; i++) {
					Player newPlayer;
					newPlayer.health = 100;
					newPlayer.xPosition = 250;
					newPlayer.yPosition = 250;
					players.push_back(newPlayer);
				}
			}

			for (int i = 0; i < players.size(); i++) {
				incomingMsg >> players[i].xPosition;
				incomingMsg >> players[i].yPosition;
				incomingMsg >> players[i].health;
			}
		}
	}
};

class GameDisplay : public Display
{
	void Draw() override 
	{
		DrawRectangle(0, 0, width, 200, 0xff28FF6F, DrawMode::Fill);
		for (int i=0; i<players.size(); i++)
		{
			Player& p = players[i];
			
			if (p.health > 0)
			{
				if (i == ClientId) {
					DrawCircle(p.xPosition, p.yPosition, 10, 0xffff0000, DrawMode::Fill);
				}
				else {
					DrawCircle(p.xPosition, p.yPosition, 10, 0x00000000, DrawMode::Fill);
				}
			}
		}
	}
};

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prevInstance, char* cmdArgs, int cmdShow)
{
	GameClient client;
	client.Initialize();
	client.ConnectToServer("127.0.0.1", 54000);	//read from file if host is on other computer

	GameDisplay display;
	display.Initialize();

	while (display.running)
	{
		UpdateInput(input);

		std::string outboundMsg = PrepareMessage(ClientId, input);
		client.SendMessageToServer(outboundMsg.c_str(), outboundMsg.size() + 1);
		client.Update();

		display.Update();
	}

	return 0;
}
