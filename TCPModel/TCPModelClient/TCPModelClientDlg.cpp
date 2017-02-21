// TCPModelClientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TCPModelClient.h"
#include "TCPModelClientDlg.h"
#include ".\tcpmodelclientdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// CTCPModelClientDlg �Ի���



CTCPModelClientDlg::CTCPModelClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTCPModelClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTCPModelClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_RECV, m_edit_recv);
	DDX_Control(pDX, IDC_EDIT_SEND, m_edit_send);
}

BEGIN_MESSAGE_MAP(CTCPModelClientDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CTCPModelClientDlg ��Ϣ�������

BOOL CTCPModelClientDlg::OnInitDialog()
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

	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(2,2),&wsaData)!=0)
	{
		printf("��ʼ���ͻ���WinSock����ʧ�ܣ�\n");
		return false;
	}
	else
		printf("��ʼ���ͻ���WinSock����ɹ���\n");

	if (LOBYTE(wsaData.wVersion)!=2 || HIBYTE(wsaData.wVersion)!=2)
	{
		printf(("��ʼ���ͻ���WinSock����ʧ��,����ȷ������ϵͳ�Ƿ�֧��Windows Sockets DLL 2.2�汾\n"));
		WSACleanup();
		return false;
	}

	return TRUE;  // ���������˿ؼ��Ľ��㣬���򷵻� TRUE
}

void CTCPModelClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CTCPModelClientDlg::OnPaint() 
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
HCURSOR CTCPModelClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTCPModelClientDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here

	SOCKET socketClient;

	socketClient=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (socketClient==INVALID_SOCKET)
	{
		printf("�����ͻ����׽���ʧ�ܣ�,error = %d\n",WSAGetLastError());
		WSACleanup();
		return;
	}
	else
		printf("�����ͻ����׽��ֳɹ���\n");

	memset(sendBuf,0,MESSAGESIZE);

	SOCKADDR_IN addrServer;
	
	memset(&addrServer,0,sizeof(SOCKADDR_IN));
	addrServer.sin_addr.S_un.S_addr=inet_addr(SERVER_IP);
	addrServer.sin_family=AF_INET;
	addrServer.sin_port=PORT;

	if(connect(socketClient,(struct sockaddr *)&addrServer,sizeof(SOCKADDR_IN))!=0)
	{
		printf("�ͻ������ӷ�����ʧ��!��error=%d\n",WSAGetLastError());
		closesocket(socketClient);
		return;
	}
	else
		printf("�ͻ������ӷ������ɹ�!\n");


	CString strSend="";
	m_edit_send.GetWindowText(strSend);
	sprintf(sendBuf,"%s",strSend);
	int nSendSize=send(socketClient,sendBuf,strlen(sendBuf),0);

	if (nSendSize==strlen(sendBuf))
	{
		printf("�ͻ��˷������ݳɹ���\n");
	}
	else
	{
		printf("�ͻ��˷�������ʧ�ܣ���error=%d\n",WSAGetLastError());
	}

	int nRetSize=recv(socketClient,sendBuf,MESSAGESIZE,0);

	if (nRetSize<=0)
	{
		printf("�ͻ��˽�������ʧ�ܣ���error=%d\n",WSAGetLastError());
		closesocket(socketClient);
		return;
	}
	else
	{
		printf("�ͻ��˽������ݳɹ���\n");
		m_edit_recv.SetWindowText(sendBuf);
	}

	closesocket(socketClient);


}

void CTCPModelClientDlg::OnDestroy()
{
	CDialog::OnDestroy();
	WSACleanup();
	// TODO: Add your message handler code here
}
