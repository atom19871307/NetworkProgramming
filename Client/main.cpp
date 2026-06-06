#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif 

#include <iostream>
#include <string>
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>

using namespace std;

#pragma comment(lib, "WS2_32.lib")
#define MTU 1500

#include "../MyNetworkUtils/FormatUtils.h"
#pragma comment(lib, "MyNetworkUtils.lib")

void main()
{
	setlocale(LC_ALL, "");
	cout << "CLIENT" << endl;

	// 3) У клиента должна быть возможность ввести IP-адрес и порт Сервера с клавиатуры
	string server_ip;
	string server_port;

	cout << "Enter Server IP (e.g. 127.0.0.1): ";
	cin >> server_ip;
	cout << "Enter Server Port (e.g. 27015): ";
	cin >> server_port;

	// Инициализация WinSOCK
	WSAData wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		cout << "WinSOCK init failed with code: " << iResult << endl;
		return;
	}

	// Определяем параметры подключения (используем введенные данные)
	addrinfo hints;
	addrinfo* target;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	iResult = getaddrinfo(server_ip.c_str(), server_port.c_str(), &hints, &target);
	if (iResult != 0)
	{
		cout << "getaddrinfo() failed with code " << iResult << endl;
		WSACleanup();
		return;
	}

	// Создаем сокет
	SOCKET connect_socket = socket(target->ai_family, target->ai_socktype, target->ai_protocol);
	if (connect_socket == INVALID_SOCKET)
	{
		cout << "SOCKET creation failed with error: \t" << WSAGetLastError() << endl;
		freeaddrinfo(target);
		WSACleanup();
		return;
	}

	// Подключаемся к узлу
	iResult = connect(connect_socket, target->ai_addr, target->ai_addrlen);
	DWORD dwError = WSAGetLastError();
	freeaddrinfo(target);

	if (iResult == SOCKET_ERROR)
	{
		cout << "Error " << dwError << ":\t" << FormatLastError(dwError) << endl;
		cout << "Unable to connect to server" << endl;
		closesocket(connect_socket);
		WSACleanup();
		return;
	}

	// Отправка данных
	CHAR send_buffer[MTU] = "Hello Server";
	iResult = send(connect_socket, send_buffer, (int)strlen(send_buffer), 0);
	if (iResult == SOCKET_ERROR)
	{
		cout << "Send failed with error: " << WSAGetLastError() << endl;
		closesocket(connect_socket);
		WSACleanup();
		return;
	}

	// Получение данных
	CHAR recv_buffer[MTU] = {};
	do
	{
		iResult = recv(connect_socket, recv_buffer, MTU, 0);
		if (iResult > 0)
		{
			cout << "Bytes received: " << iResult << " Message: " << recv_buffer << endl;
		}
		else if (iResult == 0)
		{
			cout << "Connection closed" << endl;
		}
		else
		{
			DWORD dwRecvError = WSAGetLastError();
			cout << "Receive failed with error: " << dwRecvError << " - " << FormatLastError(dwRecvError) << endl;
		}
	} while (iResult > 0);

	// Разрываем TCP-соединение
	shutdown(connect_socket, SD_BOTH);
	closesocket(connect_socket);
	WSACleanup();
}