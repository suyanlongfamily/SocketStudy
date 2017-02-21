#include "..\ServerSocketInit.h"


void main()
{
	//WSADATA wsaData;

	//if (WSAStartup(MAKEWORD(2,2),&wsaData)!=0)
	//{
	//	printf("初始化服务器WinSock服务失败！\n");
	//	return;
	//}
	//else
	//	printf("初始化服务器WinSock服务成功！\n");

	//if (LOBYTE(wsaData.wVersion)!=2 || HIBYTE(wsaData.wVersion)!=2)
	//{
	//	printf(("初始化服务器WinSock服务失败,请您确认您的系统是否支持Windows Sockets DLL 2.2版本\n"));
	//	WSACleanup();
	//	return;
	//}

	//SOCKET socketServer;
	//socketServer=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	//if (socketServer==INVALID_SOCKET)
	//{
	//	printf("创建服务器套接字失败！,error = %d\n",WSAGetLastError());
	//	WSACleanup();
	//	return;
	//}
	//else
	//	printf("创建服务器套接字成功！\n");

	//SOCKADDR_IN addrServer;
	//addrServer.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
	//addrServer.sin_family=AF_INET;
	//addrServer.sin_port=PORT;
	//if(bind(socketServer,(struct sockaddr *)&addrServer,sizeof(SOCKADDR_IN))!=0)
	//{
	//	printf("绑定服务器套接字失败！,error = %d\n",WSAGetLastError());
	//	closesocket(socketServer);
	//	WSACleanup();
	//	return;
	//}
	//else
	//	printf("绑定服务器套接字成功！\n");

	//if(listen(socketServer,SOMAXCONN)!=0)
	//{
	//	printf("服务器监听失败！,error = %d\n",WSAGetLastError());
	//	closesocket(socketServer);
	//	WSACleanup();
	//	return;
	//}
	//else
	//	printf("服务器监听成功！\n");


	if(ServerSocketInit()==false)
	{
		printf("服务器网络环境初始化失败！\n");
		return;
	}


	char sendBuf[MESSAGESIZE]={0};
	int nConNum=0;
	while(true)
	{
		SOCKET socketClient;
		SOCKADDR_IN addrClient;
		int addrClientSize=sizeof(SOCKADDR_IN);
		socketClient=accept(socketServer,(struct sockaddr*)&addrClient,&addrClientSize);
		if (socketClient==INVALID_SOCKET)
		{
			printf("服务器接受失败！error=%d\n",WSAGetLastError());
			continue;
		}
		else
		{
			nConNum++;
			printf("服务器成功接受一个新的连接：%s,序号为%d！\n",inet_ntoa(addrClient.sin_addr),nConNum);
		}

		memset(sendBuf,0,MESSAGESIZE);
		sprintf(sendBuf,"Hello,主机%s,序号为:%d",inet_ntoa(addrClient.sin_addr),nConNum);
		int nSendSize=send(socketClient,sendBuf,strlen(sendBuf),0);

		if (nSendSize!=strlen(sendBuf))
		{
			printf("服务器发送内容失败！error =%d\n",WSAGetLastError());
			closesocket(socketClient);
			continue;
		} 
		else
		{
			printf("服务器发送内容成功！内容为：%s\n",sendBuf);
		}

		memset(sendBuf,0,MESSAGESIZE);
		int nRetSize=recv(socketClient,sendBuf,MESSAGESIZE,0);

		if (nRetSize<=0)
		{
			printf("服务器接收内容失败！error =%d\n",WSAGetLastError());
			closesocket(socketClient);
			continue;
		}
		else
			printf("服务器接收内容成功！内容为：%s\n",sendBuf);

		if(strncmp(sendBuf,"shutserver",strlen("shutserver"))==0)
		{
			printf("服务器关闭成功！服务器总接受客户端数为:%d\n",nConNum);
			
			closesocket(socketClient);
			Sleep(3000);
			break;
		}

		closesocket(socketClient);
	}
    
	closesocket(socketServer);
	WSACleanup();
	return;
}