// TCPModelClientDlg.h : 头文件
//

#pragma once
#include "afxwin.h"


// CTCPModelClientDlg 对话框
class CTCPModelClientDlg : public CDialog
{
// 构造
public:
	CTCPModelClientDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_TCPMODELCLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CEdit m_edit_recv;
	CEdit m_edit_send;
	
	char sendBuf[MESSAGESIZE];
	afx_msg void OnDestroy();
};
