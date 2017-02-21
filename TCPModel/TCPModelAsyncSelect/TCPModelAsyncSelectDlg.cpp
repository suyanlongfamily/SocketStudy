// TCPModelAsyncSelectDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TCPModelAsyncSelect.h"
#include "TCPModelAsyncSelectDlg.h"
#include "..\ServerSocketInit.h"
#include ".\tcpmodelasyncselectdlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_SOCKET WM_USER+1

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CTCPModelAsyncSelectDlg �Ի���



CTCPModelAsyncSelectDlg::CTCPModelAsyncSelectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTCPModelAsyncSelectDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTCPModelAsyncSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTCPModelAsyncSelectDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
	ON_MESSAGE(WM_SOCKET,OnSocketProcess)
END_MESSAGE_MAP()


// CTCPModelAsyncSelectDlg ��Ϣ�������

BOOL CTCPModelAsyncSelectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��\������...\���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	
	AllocConsole();
	freopen( "CONOUT$", "w", stdout );
	freopen( "CONOUT$", "w", stderr );
	freopen( "CONIN$", "r", stdin );

	if(ServerSocketInit()==false)
	{
		printf("���������绷����ʼ��ʧ�ܣ�\n");
		return FALSE;
	}

	WSAAsyncSelect(socketServer,this->m_hWnd,WM_SOCKET,FD_ACCEPT);
	//SendMessage(WM_SOCKET,1,1);

	return TRUE;  // ���������˿ؼ��Ľ��㣬���򷵻� TRUE
}

void CTCPModelAsyncSelectDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CTCPModelAsyncSelectDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ��������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
HCURSOR CTCPModelAsyncSelectDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTCPModelAsyncSelectDlg::OnDestroy()
{
	CDialog::OnDestroy();

	closesocket(socketServer);
	WSACleanup();
	// TODO: Add your message handler code here
}

LRESULT CTCPModelAsyncSelectDlg::OnSocketProcess( WPARAM wParam, LPARAM lParam )
{
	if (WSAGETSELECTERROR(lParam))
	{
		closesocket(wParam);
		return 0;
	}

	SOCKET socketClient;
	SOCKADDR_IN addrClient;
	int naddrClientSize=sizeof(SOCKADDR_IN);
	int nRet=0;
	char szMessage[MESSAGESIZE]={0};
	switch(WSAGETSELECTEVENT(lParam))
	{
	case FD_ACCEPT:
		socketClient=accept(wParam,(struct sockaddr *)&addrClient,&naddrClientSize);
		if (socketClient==INVALID_SOCKET)
		{
			printf("����������ʧ�ܣ�error=%d\n",WSAGetLastError());
			
		}
		else
		{
			printf("�������ɹ�����һ���µ����ӣ�%s,�˿ں�Ϊ%d��\n",inet_ntoa(addrClient.sin_addr),addrClient.sin_port);
		}
		WSAAsyncSelect(socketClient,this->m_hWnd,WM_SOCKET,FD_READ|FD_CLOSE);
		break;
	case FD_READ:
		nRet=recv(wParam,szMessage,MESSAGESIZE,0);
		if (nRet == 0 || (nRet == SOCKET_ERROR && WSAGetLastError() == WSAECONNRESET))
		{
			printf("Client socket %d closed.\n", wParam);
			closesocket(wParam);
		}
		else
		{
			if(strncmp(szMessage,"shutserver",strlen("shutserver"))==0)
			{
				printf("�������رճɹ���");

				closesocket(wParam);
				OnDestroy();
				exit(0);
			}
			printf("�������������ݳɹ�������Ϊ��%s\n",szMessage);
			szMessage[nRet]='\0';
			send(wParam,szMessage,strlen(szMessage),0);
		}
		break;
	case FD_CLOSE:
		closesocket(wParam);
		break;
	}
	return 0;
}
