#pragma once

#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include <vector>
#include <string>

#define MAX_NETWORK_INPUT_BUFFER_SIZE 4096

class TcpClient {
public:
	TcpClient() = default;
	~TcpClient() = default;

	int Initialize();
	int ConnectToServer(const char* ipAddress, int port);
	int DisconnectFromServer();
	int Update();
	int Shutdown();

	void SendMessageToServer(const char* message, int messageLength);

protected:
	virtual void OnConnected(int server);
	virtual void OnDisconnected(int server);
	virtual void OnMessageReceived(int server, const char* message, int messageLength);

private:
	const char* m_ipAddress;
	int m_port;
	int m_socket;
};
