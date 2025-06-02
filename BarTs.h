
// BarTs.h: BarTs 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含 'pch.h' 以生成 PCH"
#endif

#include "resource.h"       // 主符号
#include "CColorButton.h"

//定义全局变量
extern bool LightAll;

// CBarTsApp:
// 有关此类的实现，请参阅 BarTs.cpp
//

class CBarTsApp : public CWinApp
{
public:
	CBarTsApp() noexcept;


// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 实现
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnFileOpennifti();
};

extern CBarTsApp theApp;
