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
		printf("���������绷����ʼ��ʧ�ܣ�\n");
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
			printf("����������ʧ�ܣ�error=%d\n",WSAGetLastError());
			continue;
		}
		else
		{
			g_CliSocketAddr[g_nTotalConNum++] = socketClient;
			printf("�������ɹ�����һ���µ����ӣ�%s,�˿ں�Ϊ%d��\n",inet_ntoa(addrClient.sin_addr),addrClient.sin_port);
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
		FD_ZERO(&fdread);//��fdread��ʼ���ռ�
		
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
						printf("�������رճɹ���");

						closesocket(g_CliSocketAddr[i]);
						Sleep(3000);
						bEnd=true;
						break;
					}
					printf("�������������ݳɹ�������Ϊ��%s\n",szMessage);
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