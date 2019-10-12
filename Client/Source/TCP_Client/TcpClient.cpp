#include "TcpClient.h"

int TcpClient::Initialize()
{
	int returnValue;

	//initialize winsock
	WSADATA data;
	WORD version = MAKEWORD(2, 2);
	returnValue = WSAStartup(version, &data);
	if (returnValue != 0) { return returnValue; }

	//create socket
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket == INVALID_SOCKET) { return WSAGetLastError(); }

	return 0;
}

int TcpClient::ConnectToServer(const char * ipAddress, int port)
{
	int returnValue;

	sockaddr_in hint;
	hint.sin_family = AF_INET;
	inet_pton(AF_INET, ipAddress, &hint.sin_addr);
	hint.sin_port = htons(port);

	returnValue = connect(m_socket, (sockaddr*)&hint, sizeof(hint));
	OnConnected(m_socket);

	return returnValue;
}

int TcpClient::DisconnectFromServer()
{
	OnDisconnected(m_socket);
	return shutdown(m_socket, SD_SEND);
}

int TcpClient::Update()
{
	char buffer[MAX_NETWORK_INPUT_BUFFER_SIZE];
	ZeroMemory(buffer, MAX_NETWORK_INPUT_BUFFER_SIZE);

	int bytesReceived = recv(m_socket, buffer, MAX_NETWORK_INPUT_BUFFER_SIZE, 0);
	if (bytesReceived > 0) {
		OnMessageReceived(m_socket, buffer, bytesReceived);
	}

	return 0;
}

int TcpClient::Shutdown()
{
	int returnValue;
	returnValue = closesocket(m_socket);
	WSACleanup();

	return returnValue;
}

void TcpClient::OnConnected(int server)
{
}

void TcpClient::OnDisconnected(int server)
{
}

void TcpClient::OnMessageReceived(int server, const char * message, int messageLength)
{
}

void TcpClient::SendMessageToServer(const char * message, int messageLength)
{
	send(m_socket, message, messageLength, 0);
}
