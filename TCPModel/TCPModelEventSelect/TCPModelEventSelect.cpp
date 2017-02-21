#include "..\ServerSocketInit.h"
#include "process.h"
int g_nTotalConNum=0;
SOCKET g_CliSocketAddr[MAXIMUM_WAIT_OBJECTS];
WSAEVENT g_CliEventAddr[MAXIMUM_WAIT_OBJECTS];
bool bEnd=false;
unsigned int __stdcall WorkThread(LPVOID lpParam);
void Cleanup(int index);

void main()
{
	if(ServerSocketInit()==false)
	{
		printf("���������绷����ʼ��ʧ�ܣ�\n");
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
			printf("����������ʧ�ܣ�error=%d\n",WSAGetLastError());
			continue;
		}
		else
		{
			g_CliSocketAddr[g_nTotalConNum] = socketClient;
			g_CliEventAddr[g_nTotalConNum]=WSACreateEvent();
			
			WSAEventSelect(g_CliSocketAddr[g_nTotalConNum],g_CliEventAddr[g_nTotalConNum],FD_READ|FD_CLOSE);

			g_nTotalConNum++;
			printf("�������ɹ�����һ���µ����ӣ�%s,�˿ں�Ϊ%d��\n",inet_ntoa(addrClient.sin_addr),addrClient.sin_port);
		}
	}

	for (int i=0;i<g_nTotalConNum;i++)
	{
		closesocket(g_CliSocketAddr[i]);

		WSACloseEvent(g_CliEventAddr[i]);
	}

	closesocket(socketServer);
	WSACleanup();

	_endthreadex(nThreadId);
}

unsigned int __stdcall WorkThread(LPVOID lpParam)
{
	int nRet,index;
	WSANETWORKEVENTS NetWorkEvents;

	char szMessage[MESSAGESIZE]={0};

	while(TRUE)
	{
		nRet = WSAWaitForMultipleEvents(g_nTotalConNum,g_CliEventAddr,FALSE,1000,FALSE);

		if (nRet == WSA_WAIT_FAILED || nRet == WSA_WAIT_TIMEOUT)
		{
			continue;
		}

		index = nRet - WAIT_OBJECT_0;

		WSAEnumNetworkEvents(g_CliSocketAddr[index],g_CliEventAddr[index],&NetWorkEvents);

		if(NetWorkEvents.lNetworkEvents & FD_READ)
		{
			nRet = recv(g_CliSocketAddr[index],szMessage,MESSAGESIZE,0);

			if (nRet == 0 || (nRet == SOCKET_ERROR && WSAGetLastError() == WSAECONNRESET))
			{
				printf("Client socket %d closed.\n", g_CliSocketAddr[index]);
				Cleanup(index);
			}
			else
			{
				if(strncmp(szMessage,"shutserver",strlen("shutserver"))==0)
				{
					printf("�������رճɹ���");

					Cleanup(index);
					Sleep(3000);
					bEnd=true;
					break;
				}
				printf("�������������ݳɹ�������Ϊ��%s\n",szMessage);
				szMessage[nRet]='\0';
				send(g_CliSocketAddr[index],szMessage,strlen(szMessage),0);
			}
		}
		if(NetWorkEvents.lNetworkEvents & FD_CLOSE)
		{
			Cleanup(index);
		}
	}

	return 0;
}

void Cleanup(int index)
{
	closesocket(g_CliSocketAddr[index]);

	WSACloseEvent(g_CliEventAddr[index]);

	if(index < g_nTotalConNum - 1);
	{
		g_CliSocketAddr[index] = g_CliSocketAddr[g_nTotalConNum-1];
		g_CliEventAddr[index] = g_CliEventAddr[g_nTotalConNum-1];
	}

	g_nTotalConNum --;
}