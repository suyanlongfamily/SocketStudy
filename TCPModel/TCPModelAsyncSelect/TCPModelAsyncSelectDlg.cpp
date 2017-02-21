// TCPModelAsyncSelectDlg.cpp : 实现文件
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


// CTCPModelAsyncSelectDlg 对话框



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


// CTCPModelAsyncSelectDlg 消息处理程序

BOOL CTCPModelAsyncSelectDlg::OnInitDialog()
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
	
	AllocConsole();
	freopen( "CONOUT$", "w", stdout );
	freopen( "CONOUT$", "w", stderr );
	freopen( "CONIN$", "r", stdin );

	if(ServerSocketInit()==false)
	{
		printf("服务器网络环境初始化失败！\n");
		return FALSE;
	}

	WSAAsyncSelect(socketServer,this->m_hWnd,WM_SOCKET,FD_ACCEPT);
	//SendMessage(WM_SOCKET,1,1);

	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTCPModelAsyncSelectDlg::OnPaint() 
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
			printf("服务器接受失败！error=%d\n",WSAGetLastError());
			
		}
		else
		{
			printf("服务器成功接受一个新的连接：%s,端口号为%d！\n",inet_ntoa(addrClient.sin_addr),addrClient.sin_port);
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
				printf("服务器关闭成功！");

				closesocket(wParam);
				OnDestroy();
				exit(0);
			}
			printf("服务器接收内容成功！内容为：%s\n",szMessage);
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
