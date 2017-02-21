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
		printf("初始化服务器WinSock服务失败！\n");
		return false;
	}
	else
		printf("初始化服务器WinSock服务成功！\n");

	if (LOBYTE(wsaData.wVersion)!=2 || HIBYTE(wsaData.wVersion)!=2)
	{
		printf(("初始化服务器WinSock服务失败,请您确认您的系统是否支持Windows Sockets DLL 2.2版本\n"));
		WSACleanup();
		return false;
	}

	
	socketServer=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (socketServer==INVALID_SOCKET)
	{
		printf("创建服务器套接字失败！,error = %d\n",WSAGetLastError());
		WSACleanup();
		return false;
	}
	else
		printf("创建服务器套接字成功！\n");

	SOCKADDR_IN addrServer;
	addrServer.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
	addrServer.sin_family=AF_INET;
	addrServer.sin_port=PORT;
	if(bind(socketServer,(struct sockaddr *)&addrServer,sizeof(SOCKADDR_IN))!=0)
	{
		printf("绑定服务器套接字失败！,error = %d\n",WSAGetLastError());
		closesocket(socketServer);
		WSACleanup();
		return false;
	}
	else
		printf("绑定服务器套接字成功！\n");

	if(listen(socketServer,SOMAXCONN)!=0)
	{
		printf("服务器监听失败！,error = %d\n",WSAGetLastError());
		closesocket(socketServer);
		WSACleanup();
		return false;
	}
	else
		printf("服务器监听成功！\n");

	return true;
}

#endif