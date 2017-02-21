#include "..\ServerSocketInit.h"
#include "process.h"

typedef enum
{
	RECV_POSTED
}OPERATION_TYPE;

typedef struct
{
	WSAOVERLAPPED overlap;
	WSABUF Buffer;
	char szMessage[MESSAGESIZE];
	DWORD NumberOfBytesRecved;
	DWORD Flags;
	OPERATION_TYPE OperationType;
}PER_IO_OPERATION_DATA,*LPPER_IO_OPERATION_DATA;


bool bEnd=false;
unsigned int __stdcall WorkThread(LPVOID lpParam);

void main()
{
	if(ServerSocketInit()==false)
	{
		printf("服务器网络环境初始化失败！\n");
		return;
	}

	SOCKET socketClient;
	HANDLE CompletionPort = INVALID_HANDLE_VALUE;
	unsigned int i,dwThreadId;
	LPPER_IO_OPERATION_DATA lpPerIOData=NULL;

	CompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,0,0);

	if (CompletionPort==NULL)
	{
		printf("创建完成端口失败！Error = %d",GetLastError());
		return;
	}
	else
		printf("创建完成端口成功！");

	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);

	for (i = 0; i< sysInfo.dwNumberOfProcessors; i++)
	{
		_beginthreadex(NULL,0,WorkThread,CompletionPort,0,&dwThreadId);
	}

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
			printf("服务器成功接受一个新的连接：%s,端口号为%d！\n",inet_ntoa(addrClient.sin_addr),addrClient.sin_port);
		}

		CreateIoCompletionPort((HANDLE)socketClient,CompletionPort,(DWORD)socketClient,0);

		lpPerIOData = (LPPER_IO_OPERATION_DATA)HeapAlloc(GetProcessHeap(),
			HEAP_ZERO_MEMORY,sizeof(PER_IO_OPERATION_DATA));

		lpPerIOData->Buffer.len=MESSAGESIZE;
		lpPerIOData->Buffer.buf=lpPerIOData->szMessage;
		lpPerIOData->OperationType = RECV_POSTED;

		WSARecv(socketClient,&lpPerIOData->Buffer,1,
			&lpPerIOData->NumberOfBytesRecved,&lpPerIOData->Flags,
			&lpPerIOData->overlap,NULL);
	}

	PostQueuedCompletionStatus(CompletionPort,0xFFFFFFFF,0,NULL);
	CloseHandle(CompletionPort);
	closesocket(socketServer);
	WSACleanup();
	return;
}

unsigned int __stdcall WorkThread(LPVOID lpParam)
{
	HANDLE CompletionPort = (HANDLE)lpParam;

	DWORD dwByteTransferred;

	SOCKET socketClient;
	LPPER_IO_OPERATION_DATA lpPerIOData = NULL;

	while(TRUE)
	{
		GetQueuedCompletionStatus(CompletionPort,&dwByteTransferred,
			(PULONG_PTR)&socketClient,(LPWSAOVERLAPPED *)&lpPerIOData,INFINITE);

		if (dwByteTransferred == 0xFFFFFFFF)
		{
			return 0;
		}

		if (lpPerIOData->OperationType == RECV_POSTED)
		{
			if (dwByteTransferred == 0)
			{
				closesocket(socketClient);
				HeapFree(GetProcessHeap(),0,lpPerIOData);
			}
			else
			{
				if(strncmp(lpPerIOData->szMessage,"shutserver",strlen("shutserver"))==0)
				{
					printf("服务器关闭成功！");

					Sleep(3000);
					bEnd=true;
				}
				printf("服务器接收内容成功！内容为：%s\n",lpPerIOData->szMessage);
				lpPerIOData->szMessage[dwByteTransferred]='\0';
				send(socketClient,lpPerIOData->szMessage,strlen(lpPerIOData->szMessage),0);

				memset(&lpPerIOData->overlap,0,sizeof(WSAOVERLAPPED));
				lpPerIOData->Buffer.len = MESSAGESIZE;
				lpPerIOData->Buffer.buf = lpPerIOData->szMessage;
				lpPerIOData->OperationType = RECV_POSTED;

				WSARecv(socketClient,&lpPerIOData->Buffer,1,
					&lpPerIOData->NumberOfBytesRecved,&lpPerIOData->Flags,
					&lpPerIOData->overlap,NULL);
			}
		}
	}
}