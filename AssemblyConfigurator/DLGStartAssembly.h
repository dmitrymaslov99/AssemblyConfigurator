#pragma once


// Диалоговое окно DLGStartAssembly

class DLGStartAssembly : public CDialog
{
	DECLARE_DYNAMIC(DLGStartAssembly)

public:
	void Shaft1Build(double m_lenght1, double m_lenght2, double m_lenght3, double m_diameter1, double m_diameter2, double m_diameter3, int save, int progress);
	void Shaft2Build(double m_lenght1, double m_lenght2, double m_lenght3, double m_diameter1, double m_diameter2, double m_diameter3, int progress);
	void GearOne(double modul, double number, double diameter, double lenght3, int save, int progress);
	void GearTwo(double modul, double ratio, double number, double diameter, double lenght3, int save, int progress);
	void Shaft2Build(double m_lenght1, double m_lenght2, double m_lenght3, double m_diameter1, double m_diameter2, double m_diameter3, int save, int progress);
	void Bearing(double out, double in, double diametr, double lenght, int save, int progress);
	DLGStartAssembly(CWnd* pParent = nullptr);   // стандартный конструктор
	virtual ~DLGStartAssembly();

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV

	/*afx_msg void OnTimer(UINT_PTR nIDEvent);*/

	

	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl m_progBarCtrl2;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
