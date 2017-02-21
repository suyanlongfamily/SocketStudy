#ifndef SERVERSOCKETINIT_H_H_H
#define SERVERSOCKETINIT_H_H_H

#include "winsock2.h"

#include "stdio.h"

#define SERVER_IP "127.0.0.1"

#define PORT 80
#define MESSAGESIZE 1024
SOCKET socketServer;
#pragma comment(lib,"ws2_32.lib")

bool ServerSocketInit()
{
	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(2,2),&wsaData)!=0)
	{
		printf("��ʼ��������WinSock����ʧ�ܣ�\n");
		return false;
	}
	else
		printf("��ʼ��������WinSock����ɹ���\n");

	if (LOBYTE(wsaData.wVersion)!=2 || HIBYTE(wsaData.wVersion)!=2)
	{
		printf(("��ʼ��������WinSock����ʧ��,����ȷ������ϵͳ�Ƿ�֧��Windows Sockets DLL 2.2�汾\n"));
		WSACleanup();
		return false;
	}

	
	socketServer=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (socketServer==INVALID_SOCKET)
	{
		printf("�����������׽���ʧ�ܣ�,error = %d\n",WSAGetLastError());
		WSACleanup();
		return false;
	}
	else
		printf("�����������׽��ֳɹ���\n");

	SOCKADDR_IN addrServer;
	addrServer.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
	addrServer.sin_family=AF_INET;
	addrServer.sin_port=PORT;
	if(bind(socketServer,(struct sockaddr *)&addrServer,sizeof(SOCKADDR_IN))!=0)
	{
		printf("�󶨷������׽���ʧ�ܣ�,error = %d\n",WSAGetLastError());
		closesocket(socketServer);
		WSACleanup();
		return false;
	}
	else
		printf("�󶨷������׽��ֳɹ���\n");

	if(listen(socketServer,SOMAXCONN)!=0)
	{
		printf("����������ʧ�ܣ�,error = %d\n",WSAGetLastError());
		closesocket(socketServer);
		WSACleanup();
		return false;
	}
	else
		printf("�����������ɹ���\n");

	return true;
}

#endif