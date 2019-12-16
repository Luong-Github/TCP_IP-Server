#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include <sstream>
#pragma comment (lib, "ws2_32.lib")

using namespace std;

// create server

int main() {


	//1. initialize winsock

	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOk = WSAStartup(ver, &wsData);

	if (wsOk != 0) {
		cerr << "Can not initialize winsock ! Quitting " << endl;
		return 0;
	}


	// create a socket

	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);

	if (listening == INVALID_SOCKET) {
		cerr << "Can not create a socket ! Quitting " << endl;
		return 0;
	}

	// binding the socket to an ip address and port

	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(2000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;// could also use inet_pton

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	// tell winsock the socket is for listening
	listen(listening, SOMAXCONN);

	fd_set master;
	FD_ZERO(&master);
	
	FD_SET(listening,&master);

	while (true) {
		fd_set copy = master;

		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);
		// after select will destroy description
		for (int i = 0; i < socketCount; i++) {
			SOCKET sock = copy.fd_array[i];

			if (sock == listening) {
				// accept new connection
				SOCKET client = accept(listening, nullptr, nullptr);
				// add new connection to list of connected clients
				FD_SET(client, &master);
				// send a walcome message to the connected client
				string welcomeMsg = " Welcome to chat server: \r\n";
				send(client, welcomeMsg.c_str(), welcomeMsg.size() + 1, 0);
				// TODO : broadcast we have a new connection
			}
			else {
				char buf[4096];
				ZeroMemory(buf, 4096);
				
				// accept new message
				int bytesIn = recv(sock, buf, 4096, 0);
				if (bytesIn <= 0) {
					closesocket(sock);
					FD_CLR(sock, &master);
				}
				else {
					// Send message to other clients, and definitely not the listening socket
					for (int i = 0; i < master.fd_count; i++) {
						SOCKET outSock = master.fd_array[i];
						if (outSock != listening && outSock != sock) {
							
							ostringstream ss;
							ss << "SOCKET " << sock << ":" << buf << "\r\n";
							string strOut = ss.str();

							send(outSock, buf, bytesIn, 0);
						}
					}
				}
			}
		}
	}

	//
	// close the socket
	//closesocket(ClientSocket);
	// Clean up \ winsock
	WSACleanup();

	system("pause");
}





// wait for a connection
	//sockaddr_in client;
	//int clientsize = sizeof(client);

	//SOCKET ClientSocket = accept(listening, (sockaddr*)&client, &clientsize);

	//char host[NI_MAXHOST];
	//char service[NI_MAXHOST];

	//ZeroMemory(host, NI_MAXHOST); // the same as memset(host, 0, NI_MAXHOST);
	//ZeroMemory(service, NI_MAXSERV);


	//if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	//{
	//	cout << host << " connected on port " << service << endl;
	//}
	//else {
	//	inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
	//	cout << host << " connected on port " << ntohs(client.sin_port) << endl;
	//}
	//// close listening socket

	//closesocket(listening);

	//// while loop : accept and echo message back to client

	//char buf[4096];
	//while (true) {
	//	ZeroMemory(buf, 4096);

	//	// wait for client to send date
	//	int bytesReceived = recv(ClientSocket, buf, 4096, 0);
	//	if (bytesReceived == SOCKET_ERROR) {
	//		cerr << "Error in recv(). Quiting" << endl;
	//		break;
	//	}
	//	if (bytesReceived == 0) {
	//		cout << "Client disconnected" << endl;
	//		break;
	//	}
	//	
	//	cout << string(buf, 0, bytesReceived) << endl;

	//	// echo message back to client
	//	send(ClientSocket, buf, bytesReceived + 1, 0);
	//}