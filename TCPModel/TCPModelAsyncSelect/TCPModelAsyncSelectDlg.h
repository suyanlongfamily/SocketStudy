// TCPModelAsyncSelectDlg.h : ͷ�ļ�
//

#pragma once


// CTCPModelAsyncSelectDlg �Ի���
class CTCPModelAsyncSelectDlg : public CDialog
{
// ����
public:
	CTCPModelAsyncSelectDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TCPMODELASYNCSELECT_DIALOG };

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
	afx_msg void OnDestroy();
	afx_msg LRESULT OnSocketProcess( WPARAM wParam, LPARAM lParam );
};
