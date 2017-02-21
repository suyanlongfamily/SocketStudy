#include "..\ServerSocketInit.h"
#include "process.h"

typedef struct
{
	WSAOVERLAPPED overlap;
	WSABUF Buffer;
	char szMessage[MESSAGESIZE];
	DWORD NumberOfBytesRecved;
	DWORD Flags;
	SOCKET scoketClient;
}PER_IO_OPERATION_DATA,*LPPER_IO_OPERATION_DATA;

int g_nTotalConNum=0;

void CALLBACK CompletionROUTINE(DWORD dwError,DWORD csTransferred,
								LPWSAOVERLAPPED lpOverlappped,DWORD dwFlags);

bool bEnd=false;
unsigned int __stdcall WorkThread(LPVOID lpParam);
SOCKET g_sNewClientConnection;

bool g_bNewConnectionArrived = FALSE;

void main()
{
	if(ServerSocketInit()==false)
	{
		printf("���������绷����ʼ��ʧ�ܣ�\n");
		return;
	}

	unsigned int nThreadId=-1;

	_beginthreadex(NULL,0,WorkThread,NULL,0,&nThreadId);

	while(TRUE)
	{
		if (bEnd)
		{
			break;
		}

		SOCKADDR_IN addrClient;
		int addrClientSize=sizeof(SOCKADDR_IN);
		g_sNewClientConnection=accept(socketServer,(struct sockaddr*)&addrClient,&addrClientSize);

		if (g_sNewClientConnection==INVALID_SOCKET)
		{
			printf("����������ʧ�ܣ�error=%d\n",WSAGetLastError());
			continue;
		}
		else
		{
			g_bNewConnectionArrived=true;
		}
	}
	closesocket(socketServer);
	WSACleanup();
	return;
}

unsigned int __stdcall WorkThread(LPVOID lpParam)
{
	LPPER_IO_OPERATION_DATA lperIOData=NULL;

	while(TRUE)
	{
		if (g_bNewConnectionArrived)
		{
			lperIOData = (LPPER_IO_OPERATION_DATA)HeapAlloc(GetProcessHeap(),
				HEAP_ZERO_MEMORY,sizeof(PER_IO_OPERATION_DATA));

			lperIOData->Buffer.buf = lperIOData->szMessage;
			lperIOData->Buffer.len = MESSAGESIZE;
			lperIOData->scoketClient = g_sNewClientConnection;

			WSARecv(g_sNewClientConnection,&lperIOData->Buffer,1,
				&lperIOData->NumberOfBytesRecved,&lperIOData->Flags,
				&lperIOData->overlap,CompletionROUTINE);

			g_bNewConnectionArrived=false;
		}

		SleepEx(10,TRUE);
	}
	return 0;
}

void CALLBACK CompletionROUTINE(DWORD dwError,DWORD csTransferred, 
								LPWSAOVERLAPPED lpOverlappped,DWORD dwFlags)
{
	LPPER_IO_OPERATION_DATA lperIOData=(LPPER_IO_OPERATION_DATA)lpOverlappped;
	
	if (dwError != 0 || csTransferred == 0)
	{
		closesocket(lperIOData->scoketClient);
		HeapFree(GetProcessHeap(),0,lperIOData);
	}
	else
	{
		if(strncmp(lperIOData->szMessage,"shutserver",strlen("shutserver"))==0)
		{
			printf("�������رճɹ���");

			Sleep(3000);
			bEnd=true;
		}
		printf("�������������ݳɹ�������Ϊ��%s\n",lperIOData->szMessage);
		lperIOData->szMessage[csTransferred]='\0';
		send(lperIOData->scoketClient,lperIOData->szMessage,strlen(lperIOData->szMessage),0);

		memset(&lperIOData->overlap,0,sizeof(WSAOVERLAPPED));
		lperIOData->Buffer.len = MESSAGESIZE;
		lperIOData->Buffer.buf = lperIOData->szMessage;

		WSARecv(lperIOData->scoketClient,&lperIOData->Buffer,1,
			&lperIOData->NumberOfBytesRecved,&lperIOData->Flags,
			&lperIOData->overlap,CompletionROUTINE);

	}
}