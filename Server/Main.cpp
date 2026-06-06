// Server
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif 

#include <iostream>
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>

using namespace std;

#pragma comment(lib, "WS2_32.lib")
#define MTU 1500

// 1) Использовать 'FormatLastError.lib' в Сервере
#include "../MyNetworkUtils/FormatUtils.h"
#pragma comment(lib, "MyNetworkUtils.lib")

void main()
{
	setlocale(LC_ALL, "");
	cout << "SERVER" << endl;

	// Инициализация WinSOCK
	WSADATA wsaData;
	INT iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		cout << "WSAStartup failed with error: " << iResult << endl;
		return;
	}

	// Параметры подключения
	addrinfo hints;
	addrinfo* target;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;			// TCP/IPv4
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	iResult = getaddrinfo(NULL, "27015", &hints, &target);
	if (iResult != 0)
	{
		cout << "getaddrinfo() failed with error " << iResult << endl;
		WSACleanup();
		return;
	}

	// Создание серверного сокета
	SOCKET listen_socket = socket(target->ai_family, target->ai_socktype, target->ai_protocol);
	if (listen_socket == INVALID_SOCKET)
	{
		DWORD dwError = WSAGetLastError();
		cout << "Socket failed with error: " << dwError << " - " << FormatLastError(dwError) << endl;
		freeaddrinfo(target);
		WSACleanup();
		return;
	}

	// Привязка сокета
	iResult = bind(listen_socket, target->ai_addr, target->ai_addrlen);
	if (iResult != 0)
	{
		DWORD dwError = WSAGetLastError();
		cout << "bind failed with error: " << dwError << " - " << FormatLastError(dwError) << endl;
		freeaddrinfo(target);
		closesocket(listen_socket);
		WSACleanup();
		return;
	}

	freeaddrinfo(target);

	// Запуск прослушивания
	if (listen(listen_socket, 1) == SOCKET_ERROR)
	{
		DWORD dwError = WSAGetLastError();
		cout << "Listen failed with error: " << dwError << " - " << FormatLastError(dwError) << endl;
		closesocket(listen_socket);
		WSACleanup();
		return;
	}

	cout << "Waiting for client connection..." << endl;

	// 2) При подключении клиента, Сервер должен отображать его IP-адрес и порт
	SOCKADDR_IN client_address;
	INT client_address_len = sizeof(client_address);

	SOCKET client_socket = accept(listen_socket, (SOCKADDR*)&client_address, &client_address_len);
	if (client_socket == INVALID_SOCKET)
	{
		DWORD dwError = WSAGetLastError();
		cout << "Accept failed with error: " << dwError << " - " << FormatLastError(dwError) << endl;
		closesocket(listen_socket);
		WSACleanup();
		return;
	}

	// Преобразуем IP и Port в читаемый вид
	CHAR sz_client_ip[32] = {};
	inet_ntop(AF_INET, &client_address.sin_addr, sz_client_ip, sizeof(sz_client_ip));
	USHORT client_port = ntohs(client_address.sin_port);

	cout << ">> Client connected from IP: " << sz_client_ip << " on Port: " << client_port << " <<" << endl;

	// Обмен данными
	CHAR recv_buffer[MTU] = {};
	CHAR send_buffer[MTU] = "Hello client";
	INT iReceivedBytes = 0;

	do
	{
		iReceivedBytes = recv(client_socket, recv_buffer, MTU, 0);
		if (iReceivedBytes > 0)
		{
			cout << "Received: " << recv_buffer << " (" << iReceivedBytes << " bytes)" << endl;
			send(client_socket, send_buffer, (int)strlen(send_buffer), 0);
		}
		else if (iReceivedBytes == 0)
		{
			cout << "Connection closing..." << endl;
		}
		else
		{
			DWORD dwRecvError = WSAGetLastError();
			cout << "Receive failed with error: " << dwRecvError << " - " << FormatLastError(dwRecvError) << endl;
		}
	} while (iReceivedBytes > 0);

	// Завершение работы
	shutdown(client_socket, SD_BOTH);
	closesocket(client_socket);
	closesocket(listen_socket);
	WSACleanup();
}