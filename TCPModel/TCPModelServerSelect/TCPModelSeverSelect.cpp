#include "..\ServerSocketInit.h"
#include "process.h"
int g_nTotalConNum=0;
SOCKET g_CliSocketAddr[FD_SETSIZE];

unsigned int __stdcall WorkerThread(LPVOID lpParameter);
bool bEnd=false;
void main()
{
	
	if(ServerSocketInit()==false)
	{
		printf("服务器网络环境初始化失败！\n");
		return;
	}

	unsigned int nWorkThreadId=0;

	_beginthreadex(NULL,0,WorkerThread,NULL,0,&nWorkThreadId);

	SOCKET socketClient;

	while(TRUE)
	{
		if (bEnd)
		{
			break;
		}

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
			g_CliSocketAddr[g_nTotalConNum++] = socketClient;
			printf("服务器成功接受一个新的连接：%s,端口号为%d！\n",inet_ntoa(addrClient.sin_addr),addrClient.sin_port);
		}
	}

	_endthreadex(nWorkThreadId);

	for(int i=0;i<g_nTotalConNum;i++)
	{
		if (g_CliSocketAddr[i]!=INVALID_SOCKET)
		{
			closesocket(g_CliSocketAddr[i]);
		}
	}
	closesocket(socketServer);
	WSACleanup();
	return ;
}

unsigned int __stdcall WorkerThread(LPVOID lpParameter)
{
	int i;
	fd_set fdread;
	int ret;
	struct timeval tv={1,0};
	char szMessage[MESSAGESIZE];

	while(TRUE)
	{
		if (bEnd)
		{
			break;
		}
		FD_ZERO(&fdread);//将fdread初始化空集
		
		for (int i=0;i<g_nTotalConNum;i++)
		{
			FD_SET(g_CliSocketAddr[i],&fdread);
		}

		ret=select(0,&fdread,NULL,NULL,&tv);
		if (ret == 0)
		{
			continue;
		}
		else if (ret == -1)
		{
			Sleep(1);
		}

		for (int i=0;i<g_nTotalConNum;i++)
		{
			if (FD_ISSET(g_CliSocketAddr[i],&fdread))
			{
				ret = recv(g_CliSocketAddr[i],szMessage,MESSAGESIZE,0);

				if (ret == 0 || (ret == SOCKET_ERROR && WSAGetLastError() == WSAECONNRESET))
				{
					printf("Client socket %d closed.\n", g_CliSocketAddr[i]);
					closesocket(g_CliSocketAddr[i]);

					if (i<g_nTotalConNum-1)
					{
						g_CliSocketAddr[i--]=g_CliSocketAddr[--g_nTotalConNum];
					}
				}
				else
				{
					if(strncmp(szMessage,"shutserver",strlen("shutserver"))==0)
					{
						printf("服务器关闭成功！");

						closesocket(g_CliSocketAddr[i]);
						Sleep(3000);
						bEnd=true;
						break;
					}
					printf("服务器接收内容成功！内容为：%s\n",szMessage);
					szMessage[ret]='\0';
					send(g_CliSocketAddr[i],szMessage,strlen(szMessage),0);
				}
			}
			else
				Sleep(1);
		}
	}
	return 0;
}