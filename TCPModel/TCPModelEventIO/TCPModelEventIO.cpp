#include "..\ServerSocketInit.h"
#include "process.h"

typedef struct
{
	WSAOVERLAPPED overlap;
	WSABUF Buffer;
	char szMessage[MESSAGESIZE];
	DWORD NumberOfBytesRecved;
	DWORD Flags;
}PER_IO_OPERATION_DATA,*LPPER_IO_OPERATION_DATA;

int g_nTotalConNum=0;
SOCKET g_CliSocketAddr[MAXIMUM_WAIT_OBJECTS];
WSAEVENT g_CliEventAddr[MAXIMUM_WAIT_OBJECTS];
LPPER_IO_OPERATION_DATA g_PerIODataAddr[MAXIMUM_WAIT_OBJECTS];

bool bEnd=false;
unsigned int __stdcall WorkThread(LPVOID lpParam);
void Cleanup(int index);

void main()
{
	if(ServerSocketInit()==false)
	{
		printf("服务器网络环境初始化失败！\n");
		return;
	}

	unsigned int nThreadId=-1;

	_beginthreadex(NULL,0,WorkThread,NULL,0,&nThreadId);

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
			g_CliSocketAddr[g_nTotalConNum] = socketClient;
			g_PerIODataAddr[g_nTotalConNum] = (LPPER_IO_OPERATION_DATA)HeapAlloc(GetProcessHeap(),
				HEAP_ZERO_MEMORY,sizeof(PER_IO_OPERATION_DATA));
			g_PerIODataAddr[g_nTotalConNum]->Buffer.len=MESSAGESIZE;
			g_PerIODataAddr[g_nTotalConNum]->Buffer.buf=g_PerIODataAddr[g_nTotalConNum]->szMessage;

			g_CliEventAddr[g_nTotalConNum]=g_PerIODataAddr[g_nTotalConNum]->overlap.hEvent=WSACreateEvent();

			WSARecv(g_CliSocketAddr[g_nTotalConNum],&g_PerIODataAddr[g_nTotalConNum]->Buffer,1,
				&g_PerIODataAddr[g_nTotalConNum]->NumberOfBytesRecved,&g_PerIODataAddr[g_nTotalConNum]->Flags,
				&g_PerIODataAddr[g_nTotalConNum]->overlap,NULL);

			g_nTotalConNum++;
			printf("服务器成功接受一个新的连接：%s,端口号为%d！\n",inet_ntoa(addrClient.sin_addr),addrClient.sin_port);
		}
	}

	for (int i=0;i<g_nTotalConNum;i++)
	{
		closesocket(g_CliSocketAddr[i]);
		WSACloseEvent(g_CliEventAddr[i]);
		HeapFree(GetProcessHeap(),0,g_PerIODataAddr[i]);
	}

	closesocket(socketServer);
	WSACleanup();
}

unsigned int __stdcall WorkThread(LPVOID lpParam)
{
	int nRet,index;
	DWORD cbTransfered;

	while(TRUE)
	{
		nRet = WSAWaitForMultipleEvents(g_nTotalConNum,g_CliEventAddr,FALSE,1000,FALSE);

		if (nRet == WSA_WAIT_FAILED || nRet == WSA_WAIT_TIMEOUT)
		{
			continue;
		}

		index = nRet - WAIT_OBJECT_0;

		WSAResetEvent(g_CliEventAddr[index]);

		WSAGetOverlappedResult(g_CliSocketAddr[index],&g_PerIODataAddr[index]->overlap,
			&cbTransfered,TRUE,&g_PerIODataAddr[index]->Flags);


	

		if (cbTransfered == 0)
		{
			printf("Client socket %d closed.\n", g_CliSocketAddr[index]);
			Cleanup(index);
		}
		else
		{
			if(strncmp(g_PerIODataAddr[index]->szMessage,"shutserver",strlen("shutserver"))==0)
			{
				printf("服务器关闭成功！");

				Cleanup(index);
				Sleep(3000);
				bEnd=true;
				break;
			}
			printf("服务器接收内容成功！内容为：%s\n",g_PerIODataAddr[index]->szMessage);
			g_PerIODataAddr[index]->szMessage[cbTransfered]='\0';
			send(g_CliSocketAddr[index],g_PerIODataAddr[index]->szMessage,strlen(g_PerIODataAddr[index]->szMessage),0);

			WSARecv(g_CliSocketAddr[index],&g_PerIODataAddr[index]->Buffer,1,
				&g_PerIODataAddr[index]->NumberOfBytesRecved,&g_PerIODataAddr[index]->Flags,
				&g_PerIODataAddr[index]->overlap,NULL);
		}
		
	}

	return 0;
}

void Cleanup(int index)
{
	closesocket(g_CliSocketAddr[index]);
	WSACloseEvent(g_CliEventAddr[index]);
	HeapFree(GetProcessHeap(),0,g_PerIODataAddr[index]);

	if(index < g_nTotalConNum - 1);
	{
		g_CliSocketAddr[index] = g_CliSocketAddr[g_nTotalConNum-1];
		g_CliEventAddr[index] = g_CliEventAddr[g_nTotalConNum-1];
		g_PerIODataAddr[index] = g_PerIODataAddr[g_nTotalConNum-1]; 
	}

	g_nTotalConNum --;
	g_PerIODataAddr[g_nTotalConNum]=NULL;
}