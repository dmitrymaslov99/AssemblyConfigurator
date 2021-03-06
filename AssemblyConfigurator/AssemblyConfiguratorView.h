
// AssemblyConfiguratorView.h: интерфейс класса CAssemblyConfiguratorView
//

#pragma once
#include "AssemblyConfiguratorDoc.h"


class CAssemblyConfiguratorView : public CFormView
{
protected: // создать только из сериализации
	CAssemblyConfiguratorView();
	DECLARE_DYNCREATE(CAssemblyConfiguratorView)

public:
#ifdef AFX_DESIGN_TIME
	enum{ IDD = IDD_ASSEMBLYCONFIGURATOR_FORM };
#endif

// Атрибуты
public:
	CAssemblyConfiguratorDoc* GetDocument() const;

	void Shaft1Build(double m_lenght1, double m_lenght2, double m_lenght3, double m_diameter1, double m_diameter2, double m_diameter3, int save, TCHAR * buf);

	void Shaft2Build(double m_lenght1, double m_lenght2, double m_lenght3, double m_diameter1, double m_diameter2, double m_diameter3, int save, TCHAR * buf);

	void GearOne(double modul, double number, double diameter, double lenght3, int save, TCHAR * buf);

	void GearTwo(double modul, double ratio, double number, double diameter, double lenght3, int save, TCHAR * buf);

	void Bearing(double out, double in, double diametr, double lenght, int save, TCHAR * buf);

	void Assembly(TCHAR * buf);

// Операции
public:

// Переопределение
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV
	virtual void OnInitialUpdate(); // вызывается в первый раз после конструктора
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);

// Реализация
public:
	virtual ~CAssemblyConfiguratorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
	void UpdateForm(int Itemselection);
#endif

protected:

// Созданные функции схемы сообщений
protected:
	DECLARE_MESSAGE_MAP()
public:
	CStatic m_picture;
	CTabCtrl m_tabs;
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	double m_Variable1;
	double m_Variable2;
	double m_Variable3;
	double m_Variable4;
	double m_Variable5;
	double m_Variable6;
	double m_Variable7;
	double m_Variable8;
	double m_Variable9;
	double m_Variable10;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void CAssemblyConfiguratorView::OnActivateView(BOOL bActivate, CView * pActivateView, CView * pDeactiveView);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedButton3();
	CProgressCtrl m_progBarCtrl;
	CString m_CSName;
	

	afx_msg void OnBnClickedButton4();
	afx_msg void OnSaveFile();
	void Save();
	afx_msg void OnBnClickedButton5();
};

#ifndef _DEBUG  // версия отладки в AssemblyConfiguratorView.cpp
inline CAssemblyConfiguratorDoc* CAssemblyConfiguratorView::GetDocument() const
   { return reinterpret_cast<CAssemblyConfiguratorDoc*>(m_pDocument); }
#endif

