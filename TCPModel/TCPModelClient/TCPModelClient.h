// TCPModelClient.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error �ڰ������� PCH �Ĵ��ļ�֮ǰ������stdafx.h��
#endif

#include "resource.h"		// ������


// CTCPModelClientApp:
// �йش����ʵ�֣������ TCPModelClient.cpp
//

class CTCPModelClientApp : public CWinApp
{
public:
	CTCPModelClientApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CTCPModelClientApp theApp;
