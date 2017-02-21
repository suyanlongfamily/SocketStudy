#include "..\ServerSocketInit.h"


void main()
{
	//WSADATA wsaData;

	//if (WSAStartup(MAKEWORD(2,2),&wsaData)!=0)
	//{
	//	printf("��ʼ��������WinSock����ʧ�ܣ�\n");
	//	return;
	//}
	//else
	//	printf("��ʼ��������WinSock����ɹ���\n");

	//if (LOBYTE(wsaData.wVersion)!=2 || HIBYTE(wsaData.wVersion)!=2)
	//{
	//	printf(("��ʼ��������WinSock����ʧ��,����ȷ������ϵͳ�Ƿ�֧��Windows Sockets DLL 2.2�汾\n"));
	//	WSACleanup();
	//	return;
	//}

	//SOCKET socketServer;
	//socketServer=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	//if (socketServer==INVALID_SOCKET)
	//{
	//	printf("�����������׽���ʧ�ܣ�,error = %d\n",WSAGetLastError());
	//	WSACleanup();
	//	return;
	//}
	//else
	//	printf("�����������׽��ֳɹ���\n");

	//SOCKADDR_IN addrServer;
	//addrServer.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
	//addrServer.sin_family=AF_INET;
	//addrServer.sin_port=PORT;
	//if(bind(socketServer,(struct sockaddr *)&addrServer,sizeof(SOCKADDR_IN))!=0)
	//{
	//	printf("�󶨷������׽���ʧ�ܣ�,error = %d\n",WSAGetLastError());
	//	closesocket(socketServer);
	//	WSACleanup();
	//	return;
	//}
	//else
	//	printf("�󶨷������׽��ֳɹ���\n");

	//if(listen(socketServer,SOMAXCONN)!=0)
	//{
	//	printf("����������ʧ�ܣ�,error = %d\n",WSAGetLastError());
	//	closesocket(socketServer);
	//	WSACleanup();
	//	return;
	//}
	//else
	//	printf("�����������ɹ���\n");


	if(ServerSocketInit()==false)
	{
		printf("���������绷����ʼ��ʧ�ܣ�\n");
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
			printf("����������ʧ�ܣ�error=%d\n",WSAGetLastError());
			continue;
		}
		else
		{
			nConNum++;
			printf("�������ɹ�����һ���µ����ӣ�%s,���Ϊ%d��\n",inet_ntoa(addrClient.sin_addr),nConNum);
		}

		memset(sendBuf,0,MESSAGESIZE);
		sprintf(sendBuf,"Hello,����%s,���Ϊ:%d",inet_ntoa(addrClient.sin_addr),nConNum);
		int nSendSize=send(socketClient,sendBuf,strlen(sendBuf),0);

		if (nSendSize!=strlen(sendBuf))
		{
			printf("��������������ʧ�ܣ�error =%d\n",WSAGetLastError());
			closesocket(socketClient);
			continue;
		} 
		else
		{
			printf("�������������ݳɹ�������Ϊ��%s\n",sendBuf);
		}

		memset(sendBuf,0,MESSAGESIZE);
		int nRetSize=recv(socketClient,sendBuf,MESSAGESIZE,0);

		if (nRetSize<=0)
		{
			printf("��������������ʧ�ܣ�error =%d\n",WSAGetLastError());
			closesocket(socketClient);
			continue;
		}
		else
			printf("�������������ݳɹ�������Ϊ��%s\n",sendBuf);

		if(strncmp(sendBuf,"shutserver",strlen("shutserver"))==0)
		{
			printf("�������رճɹ����������ܽ��ܿͻ�����Ϊ:%d\n",nConNum);
			
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