// TCPModelAsyncSelect.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error �ڰ������� PCH �Ĵ��ļ�֮ǰ������stdafx.h��
#endif

#include "resource.h"		// ������


// CTCPModelAsyncSelectApp:
// �йش����ʵ�֣������ TCPModelAsyncSelect.cpp
//

class CTCPModelAsyncSelectApp : public CWinApp
{
public:
	CTCPModelAsyncSelectApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CTCPModelAsyncSelectApp theApp;
