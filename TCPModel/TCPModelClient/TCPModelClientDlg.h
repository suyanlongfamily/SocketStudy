// TCPModelClientDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"


// CTCPModelClientDlg �Ի���
class CTCPModelClientDlg : public CDialog
{
// ����
public:
	CTCPModelClientDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TCPMODELCLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
