
// AssemblyConfigurator.h: основной файл заголовка для приложения AssemblyConfigurator
//
#pragma once

#ifndef __AFXWIN_H__
	#error "включить stdafx.h до включения этого файла в PCH"
#endif

#include "resource.h"       // основные символы


// CAssemblyConfiguratorApp:
// Сведения о реализации этого класса: AssemblyConfigurator.cpp
//

class CAssemblyConfiguratorApp : public CWinApp
{
public:
	CAssemblyConfiguratorApp();


// Переопределение
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Реализация
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CAssemblyConfiguratorApp theApp;
