//#include<Windows.h>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<iostream>
#include<string>

using namespace std;

const int PORT = 5000;
const char ADDR[] = "127.0.0.1";
const int BUFFER_SIZE = 2048;

int main(int argc, char* argv[])
{
	WSADATA wsa_data;
	WORD version = MAKEWORD(2, 2);

	if (WSAStartup(version, &wsa_data))
	{
		cerr << "Version is not supported!" << endl;

		return 1;
	}

	SOCKET server = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;

	// TODO: Add handler for `inet_pton` failure
	if (argc == 1)
	{
		server_addr.sin_port = htons((u_short) PORT);
		inet_pton(AF_INET, ADDR, (void*) &(server_addr.sin_addr.s_addr));
	}
	else if (argc == 2)
	{
		server_addr.sin_port = htons((u_short) PORT);
		inet_pton(AF_INET, argv[1], (void*)  &(server_addr.sin_addr.s_addr));
	}
	else
	{
		server_addr.sin_port = htons((u_short) stoi(argv[2]));
		inet_pton(AF_INET, argv[1], (void*) &(server_addr.sin_addr.s_addr));
	}

	if (bind(server, (sockaddr *) &server_addr, sizeof(server_addr)))
	{
		cerr << "Error! Can not bind socket to this address." << endl;
		cerr << "Error code: " << WSAGetLastError() << endl;

		return 1;
	}

	cout << "Server started!" << endl;

	sockaddr_in client_addr;
	char buffer[BUFFER_SIZE];
	int ret;
	int client_addr_len = sizeof(client_addr);

	for (;;)
	{
		ret = recvfrom(server, buffer, BUFFER_SIZE, 0, (sockaddr *) &client_addr, &client_addr_len);

		if (ret == SOCKET_ERROR)
		{
			cerr << "Error: " << WSAGetLastError() << endl;
		}
		else if (strlen(buffer) > 0)
		{
			buffer[ret] = 0;
			char addr[INET_ADDRSTRLEN];

			inet_ntop(AF_INET, &(client_addr.sin_addr), addr, INET_ADDRSTRLEN);

			cout
				<< "Receive from client "
				<< "[" << addr << ":" << ntohs(client_addr.sin_port) << "]: "
				<< buffer
				<< endl;

			ret = sendto(server, buffer, strlen(buffer), 0, (sockaddr *) &client_addr, sizeof(client_addr));

			if (ret == SOCKET_ERROR)
			{
				cerr << "Error: " << WSAGetLastError() << endl;
			}
		}
	}

	closesocket(server);

	WSACleanup();

	return 0;
}