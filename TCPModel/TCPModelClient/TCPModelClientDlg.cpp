// TCPModelClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TCPModelClient.h"
#include "TCPModelClientDlg.h"
#include ".\tcpmodelclientdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CTCPModelClientDlg 对话框



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


// CTCPModelClientDlg 消息处理程序

BOOL CTCPModelClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将\“关于...\”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(2,2),&wsaData)!=0)
	{
		printf("初始化客户端WinSock服务失败！\n");
		return false;
	}
	else
		printf("初始化客户端WinSock服务成功！\n");

	if (LOBYTE(wsaData.wVersion)!=2 || HIBYTE(wsaData.wVersion)!=2)
	{
		printf(("初始化客户端WinSock服务失败,请您确认您的系统是否支持Windows Sockets DLL 2.2版本\n"));
		WSACleanup();
		return false;
	}

	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTCPModelClientDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
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
		printf("创建客户端套接字失败！,error = %d\n",WSAGetLastError());
		WSACleanup();
		return;
	}
	else
		printf("创建客户端套接字成功！\n");

	memset(sendBuf,0,MESSAGESIZE);

	SOCKADDR_IN addrServer;
	
	memset(&addrServer,0,sizeof(SOCKADDR_IN));
	addrServer.sin_addr.S_un.S_addr=inet_addr(SERVER_IP);
	addrServer.sin_family=AF_INET;
	addrServer.sin_port=PORT;

	if(connect(socketClient,(struct sockaddr *)&addrServer,sizeof(SOCKADDR_IN))!=0)
	{
		printf("客户端连接服务器失败!，error=%d\n",WSAGetLastError());
		closesocket(socketClient);
		return;
	}
	else
		printf("客户端连接服务器成功!\n");


	CString strSend="";
	m_edit_send.GetWindowText(strSend);
	sprintf(sendBuf,"%s",strSend);
	int nSendSize=send(socketClient,sendBuf,strlen(sendBuf),0);

	if (nSendSize==strlen(sendBuf))
	{
		printf("客户端发送内容成功！\n");
	}
	else
	{
		printf("客户端发送内容失败！，error=%d\n",WSAGetLastError());
	}

	int nRetSize=recv(socketClient,sendBuf,MESSAGESIZE,0);

	if (nRetSize<=0)
	{
		printf("客户端接收内容失败！，error=%d\n",WSAGetLastError());
		closesocket(socketClient);
		return;
	}
	else
	{
		printf("客户端接收内容成功！\n");
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
