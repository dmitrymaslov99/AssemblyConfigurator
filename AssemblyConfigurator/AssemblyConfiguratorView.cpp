
// AssemblyConfiguratorView.cpp: реализация класса CAssemblyConfiguratorView
//

#include "stdafx.h"
// SHARED_HANDLERS можно определить в обработчиках фильтров просмотра реализации проекта ATL, эскизов
// и поиска; позволяет совместно использовать код документа в данным проекте.
#ifndef SHARED_HANDLERS
#include "AssemblyConfigurator.h"
#endif

#include "AssemblyConfiguratorDoc.h"
#include "AssemblyConfiguratorView.h"
#include "AssemblyConfigurator.h"
#include "DLGStartAssembly.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma once
#include <atlbase.h>
#import <C:\Users\Dmitry\source\repos\AssemblyConfigurator\RxInventor.tlb> \
rename_namespace("InventorNative") \
named_guids raw_dispinterfaces \
high_method_prefix("Method") \
rename("DeleteFile", "APIDeleteFile") \
rename("CopyFile", "APICopyFile") \
rename("MoveFile", "APIMoveFile") \
rename("SetEnvironmentVariable", "APISetEnvironmentVariable") \
rename("GetObject", "APIGetObject") \
exclude("_FILETIME", "IStream", "ISequentialStream", \
"_LARGE_INTEGER", "_ULARGE_INTEGER", "tagSTATSTG", \
"IEnumUnknown", "IPersistFile", "IPersist", "IPictureDisp")



using namespace InventorNative;

CComPtr<Application> pInvApp;
InventorNative::PartDocumentPtr pPartDoc;
InventorNative::PartComponentDefinition *pPartComDef;
InventorNative::TransientGeometry *pTransGeom;
InventorNative::Transaction *pTrans;

TransactionManagerPtr pTransManager;
Document *Doc;

PlanarSketches *sketches;
Sketches3D   *sketches3D;
WorkPlanes *wp;
PartFeatures *ft;
WorkAxes *wax;
WorkPoints *wpt;


// CAssemblyConfiguratorView

IMPLEMENT_DYNCREATE(CAssemblyConfiguratorView, CFormView)

BEGIN_MESSAGE_MAP(CAssemblyConfiguratorView, CFormView)
	// Стандартные команды печати
	ON_COMMAND(ID_FILE_PRINT, &CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CFormView::OnFilePrintPreview)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CAssemblyConfiguratorView::OnTcnSelchangeTab1)
	ON_BN_CLICKED(IDC_BUTTON1, &CAssemblyConfiguratorView::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CAssemblyConfiguratorView::OnBnClickedButton2)
	//	ON_WM_ACTIVATE()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON3, &CAssemblyConfiguratorView::OnBnClickedButton3)
	ON_COMMAND(ID_32771, &CAssemblyConfiguratorView::OnSaveFile)
	ON_BN_CLICKED(IDC_BUTTON5, &CAssemblyConfiguratorView::OnBnClickedButton5)
END_MESSAGE_MAP()

// Создание или уничтожение CAssemblyConfiguratorView

CAssemblyConfiguratorView::CAssemblyConfiguratorView()
	: CFormView(IDD_ASSEMBLYCONFIGURATOR_FORM)
	, m_Variable1(0)
	, m_Variable2(0)
	, m_Variable3(0)
	, m_Variable4(0)
	, m_Variable5(0)
	, m_Variable6(0)
	, m_Variable7(0)
	, m_Variable8(0)
	, m_Variable9(0)
	, m_Variable10(0)
	, m_CSName(_T(""))
{
	// TODO: добавьте код создания

}

CAssemblyConfiguratorView::~CAssemblyConfiguratorView()
{
}

void CAssemblyConfiguratorView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PICTURE1, m_picture);
	DDX_Control(pDX, IDC_TAB1, m_tabs);
	DDX_Text(pDX, IDC_EDIT1, m_Variable1);
	DDV_MinMaxDouble(pDX, m_Variable1, 0, 10000);
	DDX_Text(pDX, IDC_EDIT2, m_Variable2);
	DDV_MinMaxDouble(pDX, m_Variable2, 0, 10000);
	DDX_Text(pDX, IDC_EDIT3, m_Variable3);
	DDV_MinMaxDouble(pDX, m_Variable3, 0, 10000);
	DDX_Text(pDX, IDC_EDIT4, m_Variable4);
	DDV_MinMaxDouble(pDX, m_Variable4, 0, 10000);
	DDX_Text(pDX, IDC_EDIT5, m_Variable5);
	DDV_MinMaxDouble(pDX, m_Variable5, 0, 10000);
	DDX_Text(pDX, IDC_EDIT6, m_Variable6);
	DDV_MinMaxDouble(pDX, m_Variable6, 0, 10000);
	DDX_Text(pDX, IDC_EDIT7, m_Variable7);
	DDV_MinMaxDouble(pDX, m_Variable7, 0, 10000);
	DDX_Text(pDX, IDC_EDIT8, m_Variable8);
	DDV_MinMaxDouble(pDX, m_Variable8, 0, 10000);
	DDX_Text(pDX, IDC_EDIT9, m_Variable9);
	DDV_MinMaxDouble(pDX, m_Variable9, 0, 10000);
	DDX_Text(pDX, IDC_EDIT10, m_Variable10);
	DDV_MinMaxDouble(pDX, m_Variable10, 0, 10000);
	DDX_Control(pDX, IDC_PROGRESS1, m_progBarCtrl);
	DDX_Text(pDX, IDC_EDIT_NAME, m_CSName);
}

BOOL CAssemblyConfiguratorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: изменить класс Window или стили посредством изменения
	//  CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void CAssemblyConfiguratorView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	m_tabs.InsertItem(0, _T("2D Вид. Размеры"));
	m_tabs.InsertItem(1, _T("3D Вид"));
	m_tabs.SetMinTabWidth(100);

	CLSID InvAppClsid;
	HRESULT hRes = CLSIDFromProgID(L"Inventor.Application", &InvAppClsid);

	//Проверка запущено ли приложение
	CComPtr<IUnknown> pInvAppUnk = nullptr;
	hRes = ::GetActiveObject(InvAppClsid, NULL, &pInvAppUnk);
	if (FAILED(hRes)) {
		// Если не запущено - запускаем
		SetDlgItemText(IDC_STATIC_COLOR, _T("Запустите Autodesk Inventor!"));
		/*GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);*/
	}
	else
	{
		SetDlgItemText(IDC_STATIC_COLOR, _T("Autodesk Inventor запущен!"));
	}

	m_progBarCtrl.SetRange(0, 100);

	UpdateForm(-1);

}

// Печать CAssemblyConfiguratorView

BOOL CAssemblyConfiguratorView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// подготовка по умолчанию
	return DoPreparePrinting(pInfo);
}

void CAssemblyConfiguratorView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: добавьте дополнительную инициализацию перед печатью
}

void CAssemblyConfiguratorView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: добавьте очистку после печати
}

void CAssemblyConfiguratorView::OnPrint(CDC* pDC, CPrintInfo* /*pInfo*/)
{
	// TODO: добавьте свой код печати
}


// Диагностика CAssemblyConfiguratorView

#ifdef _DEBUG
void CAssemblyConfiguratorView::AssertValid() const
{
	CFormView::AssertValid();
}

void CAssemblyConfiguratorView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CAssemblyConfiguratorDoc* CAssemblyConfiguratorView::GetDocument() const // встроена неотлаженная версия
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAssemblyConfiguratorDoc)));
	return (CAssemblyConfiguratorDoc*)m_pDocument;
}
#endif //_DEBUG

// Глобальные переменные
int flag = 999;
TCHAR   buf[MAX_PATH];
int bla = 0;

// Функции построения 1 вала
void CAssemblyConfiguratorView::Shaft1Build(double m_lenght1, double m_lenght2, double m_lenght3, double m_diameter1, double m_diameter2, double m_diameter3, int save, TCHAR * buf)
{
	//Поиск приложения по ID
	UpdateData(true);

	CLSID InvAppClsid;
	HRESULT hRes = CLSIDFromProgID(L"Inventor.Application", &InvAppClsid);
	if (FAILED(hRes)) {
		pInvApp = nullptr;
		return;
	}

	//Проверка запущено ли приложение
	CComPtr<IUnknown> pInvAppUnk = nullptr;
	hRes = ::GetActiveObject(InvAppClsid, NULL, &pInvAppUnk);
	if (FAILED(hRes)) {
		MessageBox(_T("Запустите Inventor!"));
		return;
	}
	else
	{
		// Получаем указатель на запущенное приложение (hRes)
		// если происходит повторный запуск то указатель уже существует и с ним не надо ничего   делать
		if (!pInvApp)
			hRes = pInvAppUnk->QueryInterface(__uuidof(Application), (void **)&pInvApp);

		if (FAILED(hRes)) {
			return;
		}

		pInvApp->put_Visible(TRUE);
		// Prorgess Bar
		m_progBarCtrl.SetPos(40);

		pPartDoc = pInvApp->Documents->MethodAdd(kPartDocumentObject, pInvApp->FileManager->MethodGetTemplateFile(kPartDocumentObject, kMetricSystemOfMeasure, kGOST_DraftingStandard), true);

		char name[20];
		sprintf_s(name, "%S", m_CSName);

		pPartDoc->DisplayName = name;

		pPartDoc->get_ComponentDefinition(&pPartComDef);

		pInvApp->get_TransientGeometry(&pTransGeom);
		// Prorgess Bar
		m_progBarCtrl.SetPos(65);

		pTransManager = pInvApp->GetTransactionManager();

		Doc = CComQIPtr <Document>(pPartDoc);

		pTrans = pTransManager->MethodStartTransaction(Doc, _T(""));
		// Prorgess Bar
		m_progBarCtrl.SetPos(75);

		pPartComDef->get_Sketches(&sketches);
		pPartComDef->get_Sketches3D(&sketches3D);

		pPartComDef->get_WorkPlanes(&wp);
		pPartComDef->get_Features(&ft);
		pPartComDef->get_WorkAxes(&wax);
		pPartComDef->get_WorkPoints(&wpt);
	}

	// Вращение
	m_progBarCtrl.SetPos(10);

	PlanarSketch *pSketch1;
	sketches->raw_Add(wp->GetItem(3), false, &pSketch1);

	SketchPointPtr point[10];
	SketchLinePtr lines[10];

	SketchPoints *skPoints;
	SketchLines *skLines;
	Profiles *skProfiles;


	pSketch1->get_SketchPoints(&skPoints);
	pSketch1->get_SketchLines(&skLines);
	pSketch1->get_Profiles(&skProfiles);

	point[0] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(0, 0), false);
	point[1] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(m_diameter1 / 2, 0), false);
	point[2] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(m_diameter1 / 2, m_lenght1), false);
	point[3] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(m_diameter2 / 2, m_lenght1), false);
	point[4] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(m_diameter2 / 2, m_lenght1 + m_lenght2), false);
	point[5] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(m_diameter3 / 2, m_lenght1 + m_lenght2), false);
	point[6] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(m_diameter3 / 2, m_lenght1 + m_lenght2 + m_lenght3), false);
	point[7] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(m_diameter2 / 2, m_lenght1 + m_lenght2 + m_lenght3), false);
	point[8] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(m_diameter2 / 2, m_lenght1 + (m_lenght2 * 2) + m_lenght3), false);
	point[9] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(0, m_lenght1 + (m_lenght2 * 2) + m_lenght3), false);


	lines[0] = skLines->MethodAddByTwoPoints(point[0], point[1]);
	lines[1] = skLines->MethodAddByTwoPoints(point[1], point[2]);
	lines[2] = skLines->MethodAddByTwoPoints(point[2], point[3]);
	lines[3] = skLines->MethodAddByTwoPoints(point[3], point[4]);
	lines[4] = skLines->MethodAddByTwoPoints(point[4], point[5]);
	lines[5] = skLines->MethodAddByTwoPoints(point[5], point[6]);
	lines[6] = skLines->MethodAddByTwoPoints(point[6], point[7]);
	lines[7] = skLines->MethodAddByTwoPoints(point[7], point[8]);
	lines[8] = skLines->MethodAddByTwoPoints(point[8], point[9]);
	lines[9] = skLines->MethodAddByTwoPoints(point[9], point[0]);

	Profile *pProfile;
	skProfiles->raw__AddForSolid(&pProfile);

	RevolveFeatures *ftRevolve;
	ft->get_RevolveFeatures(&ftRevolve);

	RevolveFeaturePtr revolve = ftRevolve->MethodAddFull(pProfile, wax->GetItem(2), kNewBodyOperation);
	m_progBarCtrl.SetPos(35);
	// 1 паз на D1
	wp->MethodAddByPlaneAndOffset(wp->GetItem(3), m_diameter1 / 2, true);

	PlanarSketch *pSketch2;
	sketches->raw_Add(wp->GetItem(4), false, &pSketch2);

	SketchPointPtr point2[4];
	SketchLinePtr lines2[2];
	SketchArcPtr arc[2];

	SketchPoints *skPoints2;
	SketchLines *skLines2;
	Profiles *skProfiles2;
	SketchArcs *skArc;

	pSketch2->get_SketchPoints(&skPoints2);
	pSketch2->get_SketchLines(&skLines2);
	pSketch2->get_Profiles(&skProfiles2);
	pSketch2->get_SketchArcs(&skArc);

	point2[0] = skPoints2->MethodAdd(pTransGeom->MethodCreatePoint2d(-0.5, ((m_lenght1 / 4) * 3) - 0.5), false);
	point2[1] = skPoints2->MethodAdd(pTransGeom->MethodCreatePoint2d(-0.5, m_lenght1 / 4 + 0.5), false);
	point2[2] = skPoints2->MethodAdd(pTransGeom->MethodCreatePoint2d(0.5, m_lenght1 / 4 + 0.5), false);
	point2[3] = skPoints2->MethodAdd(pTransGeom->MethodCreatePoint2d(0.5, ((m_lenght1 / 4) * 3) - 0.5), false);

	lines2[0] = skLines2->MethodAddByTwoPoints(point2[0], point2[1]);
	lines2[1] = skLines2->MethodAddByTwoPoints(point2[2], point2[3]);

	arc[0] = skArc->MethodAddByThreePoints(point2[0], pTransGeom->MethodCreatePoint2d(0, (m_lenght1 / 4) * 3), point2[3]);
	arc[1] = skArc->MethodAddByThreePoints(point2[1], pTransGeom->MethodCreatePoint2d(0, m_lenght1 / 4), point2[2]);

	Profile *pProfile2;

	skProfiles2->raw__AddForSolid(&pProfile2);

	ExtrudeFeatures *ftExtrude;
	ft->get_ExtrudeFeatures(&ftExtrude);

	ExtrudeFeaturePtr extrude = ftExtrude->MethodAddByDistanceExtent(pProfile2, 1, kNegativeExtentDirection, kCutOperation);
	m_progBarCtrl.SetPos(55);
	// 2 паз на D2
	wp->MethodAddByPlaneAndOffset(wp->GetItem(3), -(m_diameter2 / 2), true);

	PlanarSketch *pSketch3;
	sketches->raw_Add(wp->GetItem(5), false, &pSketch3);

	SketchPointPtr point3[8];
	SketchLinePtr lines3[4];
	SketchArcPtr arc2[4];

	SketchPoints *skPoints3;
	SketchLines *skLines3;
	Profiles *skProfiles3;
	SketchArcs *skArc2;

	pSketch3->get_SketchPoints(&skPoints3);
	pSketch3->get_SketchLines(&skLines3);
	pSketch3->get_Profiles(&skProfiles3);
	pSketch3->get_SketchArcs(&skArc2);

	// 1 паз
	point3[0] = skPoints3->MethodAdd(pTransGeom->MethodCreatePoint2d(0.5, m_lenght1 + (((m_lenght2 / 4) * 3) - 0.5)), false);
	point3[1] = skPoints3->MethodAdd(pTransGeom->MethodCreatePoint2d(0.5, m_lenght1 + (m_lenght2 / 4 + 0.5)), false);
	point3[2] = skPoints3->MethodAdd(pTransGeom->MethodCreatePoint2d(-0.5, m_lenght1 + (m_lenght2 / 4 + 0.5)), false);
	point3[3] = skPoints3->MethodAdd(pTransGeom->MethodCreatePoint2d(-0.5, m_lenght1 + (((m_lenght2 / 4) * 3) - 0.5)), false);

	lines3[0] = skLines3->MethodAddByTwoPoints(point3[0], point3[1]);
	lines3[1] = skLines3->MethodAddByTwoPoints(point3[2], point3[3]);

	arc2[0] = skArc2->MethodAddByThreePoints(point3[0], pTransGeom->MethodCreatePoint2d(0, m_lenght1 + ((m_lenght2 / 4) * 3)), point3[3]);
	arc2[1] = skArc2->MethodAddByThreePoints(point3[1], pTransGeom->MethodCreatePoint2d(0, m_lenght1 + (m_lenght2 / 4)), point3[2]);

	// 2 паз
	point3[4] = skPoints3->MethodAdd(pTransGeom->MethodCreatePoint2d(0.5, m_lenght1 + m_lenght2 + m_lenght3 + (((m_lenght2 / 4) * 3) - 0.5)), false);
	point3[5] = skPoints3->MethodAdd(pTransGeom->MethodCreatePoint2d(0.5, m_lenght1 + m_lenght2 + m_lenght3 + (m_lenght2 / 4 + 0.5)), false);
	point3[6] = skPoints3->MethodAdd(pTransGeom->MethodCreatePoint2d(-0.5, m_lenght1 + m_lenght2 + m_lenght3 + (m_lenght2 / 4 + 0.5)), false);
	point3[7] = skPoints3->MethodAdd(pTransGeom->MethodCreatePoint2d(-0.5, m_lenght1 + m_lenght2 + m_lenght3 + (((m_lenght2 / 4) * 3) - 0.5)), false);

	lines3[2] = skLines3->MethodAddByTwoPoints(point3[4], point3[5]);
	lines3[3] = skLines3->MethodAddByTwoPoints(point3[6], point3[7]);

	arc2[2] = skArc2->MethodAddByThreePoints(point3[4], pTransGeom->MethodCreatePoint2d(0, m_lenght1 + m_lenght2 + m_lenght3 + ((m_lenght2 / 4) * 3)), point3[7]);
	arc2[3] = skArc2->MethodAddByThreePoints(point3[5], pTransGeom->MethodCreatePoint2d(0, m_lenght1 + m_lenght2 + m_lenght3 + (m_lenght2 / 4)), point3[6]);

	Profile *pProfile3;

	skProfiles3->raw__AddForSolid(&pProfile3);

	ExtrudeFeatures *ftExtrude2;
	ft->get_ExtrudeFeatures(&ftExtrude2);

	ExtrudeFeaturePtr extrude2 = ftExtrude2->MethodAddByDistanceExtent(pProfile3, 1, kPositiveExtentDirection, kCutOperation);
	m_progBarCtrl.SetPos(75);
	// 3 паз на D3
	wp->MethodAddByPlaneAndOffset(wp->GetItem(3), m_diameter3 / 2, true);

	PlanarSketch *pSketch4;
	sketches->raw_Add(wp->GetItem(6), false, &pSketch4);

	SketchPointPtr point4[4];
	SketchLinePtr lines4[2];
	SketchArcPtr arc3[2];

	SketchPoints *skPoints4;
	SketchLines *skLines4;
	Profiles *skProfiles4;
	SketchArcs *skArc3;

	pSketch4->get_SketchPoints(&skPoints4);
	pSketch4->get_SketchLines(&skLines4);
	pSketch4->get_Profiles(&skProfiles4);
	pSketch4->get_SketchArcs(&skArc3);

	point4[0] = skPoints4->MethodAdd(pTransGeom->MethodCreatePoint2d(-0.5, m_lenght1 + m_lenght2 + (((m_lenght3 / 4) * 3) - 0.5)), false);
	point4[1] = skPoints4->MethodAdd(pTransGeom->MethodCreatePoint2d(-0.5, m_lenght1 + m_lenght2 + (m_lenght3 / 4 + 0.5)), false);
	point4[2] = skPoints4->MethodAdd(pTransGeom->MethodCreatePoint2d(0.5, m_lenght1 + m_lenght2 + (m_lenght3 / 4 + 0.5)), false);
	point4[3] = skPoints4->MethodAdd(pTransGeom->MethodCreatePoint2d(0.5, m_lenght1 + m_lenght2 + (((m_lenght3 / 4) * 3) - 0.5)), false);

	lines4[0] = skLines4->MethodAddByTwoPoints(point4[0], point4[1]);
	lines4[1] = skLines4->MethodAddByTwoPoints(point4[2], point4[3]);

	arc3[0] = skArc3->MethodAddByThreePoints(point4[0], pTransGeom->MethodCreatePoint2d(0, m_lenght1 + m_lenght2 + ((m_lenght3 / 4) * 3)), point4[3]);
	arc3[1] = skArc3->MethodAddByThreePoints(point4[1], pTransGeom->MethodCreatePoint2d(0, m_lenght1 + m_lenght2 + (m_lenght3 / 4)), point4[2]);

	Profile *pProfile4;

	skProfiles4->raw__AddForSolid(&pProfile4);

	ExtrudeFeatures *ftExtrude3;
	ft->get_ExtrudeFeatures(&ftExtrude3);

	ExtrudeFeaturePtr extrude3 = ftExtrude3->MethodAddByDistanceExtent(pProfile4, 1, kNegativeExtentDirection, kCutOperation);
	m_progBarCtrl.SetPos(85);
	// Фаска
	ChamferFeatures *pChamferFt;
	ft->get_ChamferFeatures(&pChamferFt);

	EdgeCollection *edgeColl;

	pInvApp->TransientObjects->raw_CreateEdgeCollection(vtMissing, &edgeColl);

	SurfaceBody *SurfBody;
	SurfaceBodies *SurfBodies;

	pPartComDef->get_SurfaceBodies(&SurfBodies);

	SurfBodies->get_Item(1, &SurfBody);

	Edges *edges;

	SurfBody->get_Edges(&edges);

	Edge *ed;
	edges->get_Item(64, &ed);
	edgeColl->MethodAdd(ed);
	edges->get_Item(62, &ed);
	edgeColl->MethodAdd(ed);
	edges->get_Item(60, &ed);
	edgeColl->MethodAdd(ed);
	edges->get_Item(59, &ed);
	edgeColl->MethodAdd(ed);
	edges->get_Item(57, &ed);
	edgeColl->MethodAdd(ed);

	ChamferFeaturePtr chamFeature = pChamferFt->MethodAddUsingDistance(edgeColl, 0.5f, false, false, false);
	m_progBarCtrl.SetPos(100);

	/*pTrans = pTransManager->MethodEndTransaction();*/
	m_progBarCtrl.SetPos(0);

	//BSTR bstrpathName = pathName.AllocSysString();

	//pPartDoc->MethodSaveAs(bstrpathName, false);

	//pTrans = pTransManager->MethodEndTransaction();

	if (save == 1)
	{
		pPartDoc->MethodSaveAs(buf + _bstr_t("\\Вал 1.ipt"), true);

		pTrans = pTransManager->MethodEndTransaction();
	}
	if (save == 2)
	{
		pPartDoc->MethodSaveAs(buf + _bstr_t("\\Вал 2.ipt"), true);

		pTrans = pTransManager->MethodEndTransaction();
	}

}

//Функция построения 2 вала
void CAssemblyConfiguratorView::Shaft2Build(double m_lenght1, double m_lenght2, double m_lenght3, double m_diameter1, double m_diameter2, double m_diameter3, int save, TCHAR * buf)
{

	//Поиск приложения по ID
	UpdateData(true);

	CLSID InvAppClsid;
	HRESULT hRes = CLSIDFromProgID(L"Inventor.Application", &InvAppClsid);
	if (FAILED(hRes)) {
		pInvApp = nullptr;
		return;
	}

	//Проверка запущено ли приложение
	CComPtr<IUnknown> pInvAppUnk = nullptr;
	hRes = ::GetActiveObject(InvAppClsid, NULL, &pInvAppUnk);
	if (FAILED(hRes)) {
		MessageBox(_T("Запустите Inventor!"));
		return;
	}
	else
	{
		// Получаем указатель на запущенное приложение (hRes)
		// если происходит повторный запуск то указатель уже существует и с ним не надо ничего   делать
		if (!pInvApp)
			hRes = pInvAppUnk->QueryInterface(__uuidof(Application), (void **)&pInvApp);

		if (FAILED(hRes)) {
			return;
		}

		pInvApp->put_Visible(TRUE);
		// Prorgess Bar
		m_progBarCtrl.SetPos(40);

		pPartDoc = pInvApp->Documents->MethodAdd(kPartDocumentObject, pInvApp->FileManager->MethodGetTemplateFile(kPartDocumentObject, kMetricSystemOfMeasure, kGOST_DraftingStandard), true);

		char name[20];
		sprintf_s(name, "%S", m_CSName);

		pPartDoc->DisplayName = name;

		pPartDoc->get_ComponentDefinition(&pPartComDef);

		pInvApp->get_TransientGeometry(&pTransGeom);
		// Prorgess Bar
		m_progBarCtrl.SetPos(65);

		pTransManager = pInvApp->GetTransactionManager();

		Doc = CComQIPtr <Document>(pPartDoc);

		pTrans = pTransManager->MethodStartTransaction(Doc, _T(""));
		// Prorgess Bar
		m_progBarCtrl.SetPos(75);

		pPartComDef->get_Sketches(&sketches);
		pPartComDef->get_Sketches3D(&sketches3D);

		pPartComDef->get_WorkPlanes(&wp);
		pPartComDef->get_Features(&ft);
		pPartComDef->get_WorkAxes(&wax);
		pPartComDef->get_WorkPoints(&wpt);
	}

	// Вращение
	m_progBarCtrl.SetPos(10);

	PlanarSketch *pSketch1;
	sketches->raw_Add(wp->GetItem(3), false, &pSketch1);

	SketchPointPtr point[20];
	SketchLinePtr lines[20];

	SketchPoints *skPoints;
	SketchLines *skLines;
	Profiles *skProfiles;


	pSketch1->get_SketchPoints(&skPoints);
	pSketch1->get_SketchLines(&skLines);
	pSketch1->get_Profiles(&skProfiles);

	point[0] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(0, 0), false);
	point[1] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(m_diameter1 / 2, 0), false);
	point[2] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(m_diameter1 / 2, 0.5), false);
	point[3] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d((m_diameter1 / 2) - 0.5, 0.5), false);
	point[4] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d((m_diameter1 / 2) - 0.5, 1), false);
	point[5] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(m_diameter1 / 2, 1), false);
	point[6] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(m_diameter1 / 2, m_lenght1), false);
	point[7] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(m_diameter2 / 2, m_lenght1), false);
	point[8] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(m_diameter2 / 2, m_lenght1 + m_lenght3), false);
	point[9] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(m_diameter3 / 2, m_lenght1 + m_lenght3), false);
	point[10] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(m_diameter3 / 2, m_lenght1 + m_lenght3 + m_lenght2), false);
	point[11] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(m_diameter2 / 2, m_lenght1 + m_lenght3 + m_lenght2), false);
	point[12] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(m_diameter2 / 2, m_lenght1 + (m_lenght3 * 2) + m_lenght2), false);
	point[13] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(m_diameter1 / 2, m_lenght1 + (m_lenght3 * 2) + m_lenght2), false);
	point[14] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(m_diameter1 / 2, (m_lenght1 * 2) + (m_lenght3 * 2) + m_lenght2 - 1), false);
	point[15] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d((m_diameter1 / 2) - 0.5, (m_lenght1 * 2) + (m_lenght3 * 2) + m_lenght2 - 1), false);
	point[16] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d((m_diameter1 / 2) - 0.5, (m_lenght1 * 2) + (m_lenght3 * 2) + m_lenght2 - 0.5), false);
	point[17] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(m_diameter1 / 2, (m_lenght1 * 2) + (m_lenght3 * 2) + m_lenght2 - 0.5), false);
	point[18] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(m_diameter1 / 2, (m_lenght1 * 2) + (m_lenght3 * 2) + m_lenght2), false);
	point[19] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(0, (m_lenght1 * 2) + (m_lenght3 * 2) + m_lenght2), false);

	lines[0] = skLines->MethodAddByTwoPoints(point[0], point[1]);
	lines[1] = skLines->MethodAddByTwoPoints(point[1], point[2]);
	lines[2] = skLines->MethodAddByTwoPoints(point[2], point[3]);
	lines[3] = skLines->MethodAddByTwoPoints(point[3], point[4]);
	lines[4] = skLines->MethodAddByTwoPoints(point[4], point[5]);
	lines[5] = skLines->MethodAddByTwoPoints(point[5], point[6]);
	lines[6] = skLines->MethodAddByTwoPoints(point[6], point[7]);
	lines[7] = skLines->MethodAddByTwoPoints(point[7], point[8]);
	lines[8] = skLines->MethodAddByTwoPoints(point[8], point[9]);
	lines[9] = skLines->MethodAddByTwoPoints(point[9], point[10]);
	lines[10] = skLines->MethodAddByTwoPoints(point[10], point[11]);
	lines[11] = skLines->MethodAddByTwoPoints(point[11], point[12]);
	lines[12] = skLines->MethodAddByTwoPoints(point[12], point[13]);
	lines[13] = skLines->MethodAddByTwoPoints(point[13], point[14]);
	lines[14] = skLines->MethodAddByTwoPoints(point[14], point[15]);
	lines[15] = skLines->MethodAddByTwoPoints(point[15], point[16]);
	lines[16] = skLines->MethodAddByTwoPoints(point[16], point[17]);
	lines[17] = skLines->MethodAddByTwoPoints(point[17], point[18]);
	lines[18] = skLines->MethodAddByTwoPoints(point[18], point[19]);
	lines[19] = skLines->MethodAddByTwoPoints(point[19], point[0]);

	Profile *pProfile;
	skProfiles->raw__AddForSolid(&pProfile);

	RevolveFeatures *ftRevolve;
	ft->get_RevolveFeatures(&ftRevolve);

	RevolveFeaturePtr revolve = ftRevolve->MethodAddFull(pProfile, wax->GetItem(2), kNewBodyOperation);
	m_progBarCtrl.SetPos(35);

	//1 пазы на D2
	wp->MethodAddByPlaneAndOffset(wp->GetItem(3), m_diameter2 / 2, true);

	PlanarSketch *pSketch2;
	sketches->raw_Add(wp->GetItem(4), false, &pSketch2);

	SketchPointPtr point2[8];
	SketchLinePtr lines2[4];
	SketchArcPtr arc[4];

	SketchPoints *skPoints2;
	SketchLines *skLines2;
	Profiles *skProfiles2;
	SketchArcs *skArc;

	pSketch2->get_SketchPoints(&skPoints2);
	pSketch2->get_SketchLines(&skLines2);
	pSketch2->get_Profiles(&skProfiles2);
	pSketch2->get_SketchArcs(&skArc);
	//1 паз
	point2[0] = skPoints2->MethodAdd(pTransGeom->MethodCreatePoint2d(-0.5, m_lenght1 + 1.5), false);
	point2[1] = skPoints2->MethodAdd(pTransGeom->MethodCreatePoint2d(-0.5, m_lenght1 + m_lenght3 - 1.5), false);
	point2[2] = skPoints2->MethodAdd(pTransGeom->MethodCreatePoint2d(0.5, m_lenght1 + 1.5), false);
	point2[3] = skPoints2->MethodAdd(pTransGeom->MethodCreatePoint2d(0.5, m_lenght1 + m_lenght3 - 1.5), false);

	lines2[0] = skLines2->MethodAddByTwoPoints(point2[0], point2[1]);
	lines2[1] = skLines2->MethodAddByTwoPoints(point2[2], point2[3]);

	arc[0] = skArc->MethodAddByThreePoints(point2[0], pTransGeom->MethodCreatePoint2d(0, m_lenght1 + 1), point2[2]);
	arc[1] = skArc->MethodAddByThreePoints(point2[1], pTransGeom->MethodCreatePoint2d(0, m_lenght1 + m_lenght3 - 1), point2[3]);

	// 2 паз
	point2[4] = skPoints2->MethodAdd(pTransGeom->MethodCreatePoint2d(-0.5, m_lenght1 + m_lenght2 + m_lenght3 + 1.5), false);
	point2[5] = skPoints2->MethodAdd(pTransGeom->MethodCreatePoint2d(-0.5, m_lenght1 + m_lenght2 + (m_lenght3 * 2) - 1.5), false);
	point2[6] = skPoints2->MethodAdd(pTransGeom->MethodCreatePoint2d(0.5, m_lenght1 + m_lenght2 + m_lenght3 + 1.5), false);
	point2[7] = skPoints2->MethodAdd(pTransGeom->MethodCreatePoint2d(0.5, m_lenght1 + m_lenght2 + (m_lenght3 * 2) - 1.5), false);

	lines2[2] = skLines2->MethodAddByTwoPoints(point2[4], point2[5]);
	lines2[3] = skLines2->MethodAddByTwoPoints(point2[6], point2[7]);

	arc[2] = skArc->MethodAddByThreePoints(point2[4], pTransGeom->MethodCreatePoint2d(0, m_lenght1 + m_lenght2 + m_lenght3 + 1), point2[6]);
	arc[3] = skArc->MethodAddByThreePoints(point2[5], pTransGeom->MethodCreatePoint2d(0, m_lenght1 + m_lenght2 + (m_lenght3 * 2) - 1), point2[7]);


	Profile *pProfile2;

	skProfiles2->raw__AddForSolid(&pProfile2);

	ExtrudeFeatures *ftExtrude;
	ft->get_ExtrudeFeatures(&ftExtrude);

	ExtrudeFeaturePtr extrude = ftExtrude->MethodAddByDistanceExtent(pProfile2, 1, kNegativeExtentDirection, kCutOperation);
	m_progBarCtrl.SetPos(55);
	// 2 паз на D1
	wp->MethodAddByPlaneAndOffset(wp->GetItem(3), -(m_diameter1 / 2), true);

	PlanarSketch *pSketch3;
	sketches->raw_Add(wp->GetItem(5), false, &pSketch3);

	SketchPointPtr point3[8];
	SketchLinePtr lines3[4];
	SketchArcPtr arc2[4];

	SketchPoints *skPoints3;
	SketchLines *skLines3;
	Profiles *skProfiles3;
	SketchArcs *skArc2;

	pSketch3->get_SketchPoints(&skPoints3);
	pSketch3->get_SketchLines(&skLines3);
	pSketch3->get_Profiles(&skProfiles3);
	pSketch3->get_SketchArcs(&skArc2);

	// 1 паз
	point3[0] = skPoints3->MethodAdd(pTransGeom->MethodCreatePoint2d(0.5, m_lenght1 + 1.5), false);
	point3[1] = skPoints3->MethodAdd(pTransGeom->MethodCreatePoint2d(0.5, m_lenght1 + m_lenght2 - 1), false);
	point3[2] = skPoints3->MethodAdd(pTransGeom->MethodCreatePoint2d(-0.5, m_lenght1 + 1.5), false);
	point3[3] = skPoints3->MethodAdd(pTransGeom->MethodCreatePoint2d(-0.5, m_lenght1 + m_lenght2 - 1), false);

	lines3[0] = skLines3->MethodAddByTwoPoints(point3[0], point3[1]);
	lines3[1] = skLines3->MethodAddByTwoPoints(point3[2], point3[3]);

	arc2[0] = skArc2->MethodAddByThreePoints(point3[0], pTransGeom->MethodCreatePoint2d(0, m_lenght1 + 1), point3[2]);
	arc2[1] = skArc2->MethodAddByThreePoints(point3[1], pTransGeom->MethodCreatePoint2d(0, m_lenght1 + m_lenght2 - 0.5), point3[3]);

	// 2 паз
	point3[4] = skPoints3->MethodAdd(pTransGeom->MethodCreatePoint2d(0.5, 1 + ((m_lenght1 - 1) / 4) + 0.5), false);
	point3[5] = skPoints3->MethodAdd(pTransGeom->MethodCreatePoint2d(0.5, 1 + (((m_lenght1 - 1) / 4) * 3) - 0.5), false);
	point3[6] = skPoints3->MethodAdd(pTransGeom->MethodCreatePoint2d(-0.5, 1 + ((m_lenght1 - 1) / 4) + 0.5), false);
	point3[7] = skPoints3->MethodAdd(pTransGeom->MethodCreatePoint2d(-0.5, 1 + (((m_lenght1 - 1) / 4) * 3) - 0.5), false);

	lines3[2] = skLines3->MethodAddByTwoPoints(point3[4], point3[5]);
	lines3[3] = skLines3->MethodAddByTwoPoints(point3[6], point3[7]);

	arc2[2] = skArc2->MethodAddByThreePoints(point3[4], pTransGeom->MethodCreatePoint2d(0, 1 + ((m_lenght1 - 1) / 4)), point3[6]);
	arc2[3] = skArc2->MethodAddByThreePoints(point3[5], pTransGeom->MethodCreatePoint2d(0, 1 + (((m_lenght1 - 1) / 4) * 3)), point3[7]);

	Profile *pProfile3;

	skProfiles3->raw__AddForSolid(&pProfile3);

	ExtrudeFeatures *ftExtrude2;
	ft->get_ExtrudeFeatures(&ftExtrude2);

	ExtrudeFeaturePtr extrude2 = ftExtrude2->MethodAddByDistanceExtent(pProfile3, 1, kPositiveExtentDirection, kCutOperation);
	m_progBarCtrl.SetPos(75);

	//Зеркальное отражение

	ObjectCollection *pCollection;

	pInvApp->TransientObjects->raw_CreateObjectCollection(vtMissing, &pCollection);

	pCollection->MethodAdd(extrude2);

	MirrorFeatures *pMirrorFeat;

	ft->get_MirrorFeatures(&pMirrorFeat);
	wp->MethodAddByPlaneAndOffset(wp->GetItem(2), m_lenght1 + m_lenght3 + (m_lenght2 / 2), true);


	MirrorFeature *Mirrir1 = pMirrorFeat->MethodAdd(pCollection, wp->GetItem(6), true, kIdenticalCompute);

	// Фаска
	ChamferFeatures *pChamferFt;
	ft->get_ChamferFeatures(&pChamferFt);

	EdgeCollection *edgeColl;

	pInvApp->TransientObjects->raw_CreateEdgeCollection(vtMissing, &edgeColl);

	SurfaceBody *SurfBody;
	SurfaceBodies *SurfBodies;

	pPartComDef->get_SurfaceBodies(&SurfBodies);

	SurfBodies->get_Item(1, &SurfBody);

	Edges *edges;

	SurfBody->get_Edges(&edges);

	Edge *ed;
	edges->get_Item(68, &ed);
	edgeColl->MethodAdd(ed);
	edges->get_Item(66, &ed);
	edgeColl->MethodAdd(ed);
	edges->get_Item(65, &ed);
	edgeColl->MethodAdd(ed);
	edges->get_Item(63, &ed);
	edgeColl->MethodAdd(ed);

	ChamferFeaturePtr chamFeature = pChamferFt->MethodAddUsingDistance(edgeColl, 0.5f, false, false, false);
	m_progBarCtrl.SetPos(100);

	pPartDoc->MethodSaveAs(buf + _bstr_t("\\Вал 3.ipt"), true);

	pTrans = pTransManager->MethodEndTransaction();

	/*pTrans = pTransManager->MethodEndTransaction();*/
	m_progBarCtrl.SetPos(0);

}

//Функция построения шестерни
void CAssemblyConfiguratorView::GearOne(double modul, double number, double diameter, double lenght3, int save, TCHAR * buf)
{
	//Поиск приложения по ID
	UpdateData(true);
	m_progBarCtrl.SetPos(0);
	CLSID InvAppClsid;
	HRESULT hRes = CLSIDFromProgID(L"Inventor.Application", &InvAppClsid);
	if (FAILED(hRes)) {
		pInvApp = nullptr;
		return;
	}

	//Проверка запущено ли приложение
	CComPtr<IUnknown> pInvAppUnk = nullptr;
	hRes = ::GetActiveObject(InvAppClsid, NULL, &pInvAppUnk);
	if (FAILED(hRes)) {
		MessageBox(_T("Запустите Inventor!"));
		return;
	}
	else
	{
		// Получаем указатель на запущенное приложение (hRes)
		// если происходит повторный запуск то указатель уже существует и с ним не надо ничего   делать
		if (!pInvApp)
			hRes = pInvAppUnk->QueryInterface(__uuidof(Application), (void **)&pInvApp);

		if (FAILED(hRes)) {
			return;
		}

		pInvApp->put_Visible(TRUE);
		// Prorgess Bar
		m_progBarCtrl.SetPos(40);

		pPartDoc = pInvApp->Documents->MethodAdd(kPartDocumentObject, pInvApp->FileManager->MethodGetTemplateFile(kPartDocumentObject, kMetricSystemOfMeasure, kGOST_DraftingStandard), true);

		char name[20];
		sprintf_s(name, "%S", m_CSName);

		pPartDoc->DisplayName = name;

		pPartDoc->get_ComponentDefinition(&pPartComDef);

		pInvApp->get_TransientGeometry(&pTransGeom);
		// Prorgess Bar
		m_progBarCtrl.SetPos(65);

		pTransManager = pInvApp->GetTransactionManager();

		Doc = CComQIPtr <Document>(pPartDoc);

		pTrans = pTransManager->MethodStartTransaction(Doc, _T(""));
		// Prorgess Bar
		m_progBarCtrl.SetPos(75);

		pPartComDef->get_Sketches(&sketches);
		pPartComDef->get_Sketches3D(&sketches3D);

		pPartComDef->get_WorkPlanes(&wp);
		pPartComDef->get_Features(&ft);
		pPartComDef->get_WorkAxes(&wax);
		pPartComDef->get_WorkPoints(&wpt);
	}

	// Вращение
	m_progBarCtrl.SetPos(10);

	PlanarSketch *pSketch1;
	sketches->raw_Add(wp->GetItem(3), false, &pSketch1);

	SketchPointPtr point[20];
	SketchLinePtr lines[20];

	SketchPoints *skPoints;
	SketchLines *skLines;
	Profiles *skProfiles;


	pSketch1->get_SketchPoints(&skPoints);
	pSketch1->get_SketchLines(&skLines);
	pSketch1->get_Profiles(&skProfiles);

	double dimetrY = (modul * number) / 10;

	point[0] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d((diameter / 2) + 0.1, 0), false);
	point[1] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d((dimetrY + ((modul * 2) / 10)) / 2, 0), false);
	point[2] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d((dimetrY + ((modul * 2) / 10)) / 2, lenght3 / 1.8), false);
	point[3] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d((diameter / 2) + 0.1, lenght3 / 1.8), false);

	lines[0] = skLines->MethodAddByTwoPoints(point[0], point[1]);
	lines[1] = skLines->MethodAddByTwoPoints(point[1], point[2]);
	lines[2] = skLines->MethodAddByTwoPoints(point[2], point[3]);
	lines[3] = skLines->MethodAddByTwoPoints(point[3], point[0]);

	Profile *pProfile;
	skProfiles->raw__AddForSolid(&pProfile);

	RevolveFeatures *ftRevolve;
	ft->get_RevolveFeatures(&ftRevolve);

	RevolveFeaturePtr revolve = ftRevolve->MethodAddFull(pProfile, wax->GetItem(2), kNewBodyOperation);

	double step = 3.14 * modul;
	double thickness = step * 0.5;
	double down = (thickness*0.5) / 10;
	double up = (1.75*thickness) / 10;

	double r = (dimetrY + ((modul * 2) / 10)) / 2;

	double rblue = (dimetrY - ((modul * 2.25) / 10)) / 2;

	double Yup = sqrt(pow(r, 2) - pow(up / 2, 2));

	double Ydown = sqrt(pow(rblue, 2) - pow(up / 2, 2));

	m_progBarCtrl.SetPos(25);

	PlanarSketch *pSketch2;
	sketches->raw_Add(wp->GetItem(2), false, &pSketch2);

	SketchPointPtr point2[4];
	SketchLinePtr lines2[2];
	SketchArcPtr arc2[2];

	SketchPoints *skPoints2;
	SketchLines *skLines2;
	Profiles *skProfiles2;
	SketchArcs *skArc2;

	pSketch2->get_SketchPoints(&skPoints2);
	pSketch2->get_SketchLines(&skLines2);
	pSketch2->get_Profiles(&skProfiles2);
	pSketch2->get_SketchArcs(&skArc2);

	point2[0] = skPoints2->MethodAdd(pTransGeom->MethodCreatePoint2d(up / 2, Yup), false);
	point2[1] = skPoints2->MethodAdd(pTransGeom->MethodCreatePoint2d(-up / 2, Yup), false);
	point2[2] = skPoints2->MethodAdd(pTransGeom->MethodCreatePoint2d(down / 2, Ydown), false);
	point2[3] = skPoints2->MethodAdd(pTransGeom->MethodCreatePoint2d(-down / 2, Ydown), false);

	lines2[0] = skLines2->MethodAddByTwoPoints(point2[0], point2[2]);
	lines2[1] = skLines2->MethodAddByTwoPoints(point2[1], point2[3]);


	arc2[0] = skArc2->MethodAddByThreePoints(point2[0], pTransGeom->MethodCreatePoint2d(0, r), point2[1]);
	arc2[1] = skArc2->MethodAddByThreePoints(point2[2], pTransGeom->MethodCreatePoint2d(0, rblue), point2[3]);


	Profile *pProfile2;
	skProfiles2->raw__AddForSolid(&pProfile2);

	ExtrudeFeatures *ftExtrude;
	ft->get_ExtrudeFeatures(&ftExtrude);

	ExtrudeFeaturePtr extrude = ftExtrude->MethodAddByDistanceExtent(pProfile2, lenght3 / 1.8, kPositiveExtentDirection, kCutOperation);
	m_progBarCtrl.SetPos(50);

	ObjectCollection *pCollection;

	pInvApp->TransientObjects->raw_CreateObjectCollection(vtMissing, &pCollection);

	pCollection->MethodAdd(extrude);

	CircularPatternFeatures *pCircPatFeat;

	ft->get_CircularPatternFeatures(&pCircPatFeat);

	CircularPatternFeature *circFeat = pCircPatFeat->MethodAdd(pCollection, wax->GetItem(2), true, number, "360 град", true, kIdenticalCompute);

	// вырез
	PlanarSketch *pSketch3;
	sketches->raw_Add(wp->GetItem(3), false, &pSketch3);

	SketchPointPtr point3[4];
	SketchLinePtr lines3[4];
	SketchArcPtr arc3[2];

	SketchPoints *skPoints3;
	SketchLines *skLines3;
	Profiles *skProfiles3;
	SketchArcs *skArc3;

	pSketch3->get_SketchPoints(&skPoints3);
	pSketch3->get_SketchLines(&skLines3);
	pSketch3->get_Profiles(&skProfiles3);
	pSketch3->get_SketchArcs(&skArc3);

	//центр
	double bla = (lenght3 / 1.8) / 2;

	point3[0] = skPoints3->MethodAdd(pTransGeom->MethodCreatePoint2d(0.5, bla - (lenght3 / 4) + 0.5), false);
	point3[1] = skPoints3->MethodAdd(pTransGeom->MethodCreatePoint2d(0.5, bla + (lenght3 / 4) - 0.5), false);
	point3[2] = skPoints3->MethodAdd(pTransGeom->MethodCreatePoint2d(-0.5, bla - (lenght3 / 4) + 0.5), false);
	point3[3] = skPoints3->MethodAdd(pTransGeom->MethodCreatePoint2d(-0.5, bla + (lenght3 / 4) - 0.5), false);

	lines3[0] = skLines3->MethodAddByTwoPoints(point3[0], point3[1]);
	lines3[1] = skLines3->MethodAddByTwoPoints(point3[2], point3[3]);

	arc3[0] = skArc3->MethodAddByThreePoints(point3[0], pTransGeom->MethodCreatePoint2d(0, bla - (lenght3 / 4)), point3[2]);
	arc3[1] = skArc3->MethodAddByThreePoints(point3[1], pTransGeom->MethodCreatePoint2d(0, bla + (lenght3 / 4)), point3[3]);

	Profile *pProfile4;

	skProfiles3->raw__AddForSolid(&pProfile4);

	ExtrudeFeatures *ftExtrude3;
	ft->get_ExtrudeFeatures(&ftExtrude3);

	ExtrudeFeaturePtr extrude3 = ftExtrude3->MethodAddByDistanceExtent(pProfile4, (lenght3 / 2) + 1, kPositiveExtentDirection, kCutOperation);


	if (save == 1)
	{
		pPartDoc->MethodSaveAs(buf + _bstr_t("\\Шестерня 1.ipt"), true);

		pTrans = pTransManager->MethodEndTransaction();
	}
	if (save == 2)
	{
		pPartDoc->MethodSaveAs(buf + _bstr_t("\\Шестерня 2.ipt"), true);

		pTrans = pTransManager->MethodEndTransaction();
	}

	m_progBarCtrl.SetPos(100);
	m_progBarCtrl.SetPos(0);
}

//Функция построения колеса
void CAssemblyConfiguratorView::GearTwo(double modul, double ratio, double number, double diameter, double lenght3, int save, TCHAR * buf)
{
	//Поиск приложения по ID
	UpdateData(true);
	m_progBarCtrl.SetPos(0);
	CLSID InvAppClsid;
	HRESULT hRes = CLSIDFromProgID(L"Inventor.Application", &InvAppClsid);
	if (FAILED(hRes)) {
		pInvApp = nullptr;
		return;
	}

	//Проверка запущено ли приложение
	CComPtr<IUnknown> pInvAppUnk = nullptr;
	hRes = ::GetActiveObject(InvAppClsid, NULL, &pInvAppUnk);
	if (FAILED(hRes)) {
		MessageBox(_T("Запустите Inventor!"));
		return;
	}
	else
	{
		// Получаем указатель на запущенное приложение (hRes)
		// если происходит повторный запуск то указатель уже существует и с ним не надо ничего   делать
		if (!pInvApp)
			hRes = pInvAppUnk->QueryInterface(__uuidof(Application), (void **)&pInvApp);

		if (FAILED(hRes)) {
			return;
		}

		pInvApp->put_Visible(TRUE);
		// Prorgess Bar
		m_progBarCtrl.SetPos(40);

		pPartDoc = pInvApp->Documents->MethodAdd(kPartDocumentObject, pInvApp->FileManager->MethodGetTemplateFile(kPartDocumentObject, kMetricSystemOfMeasure, kGOST_DraftingStandard), true);

		char name[20];
		sprintf_s(name, "%S", m_CSName);

		pPartDoc->DisplayName = name;

		pPartDoc->get_ComponentDefinition(&pPartComDef);

		pInvApp->get_TransientGeometry(&pTransGeom);
		// Prorgess Bar
		m_progBarCtrl.SetPos(65);

		pTransManager = pInvApp->GetTransactionManager();

		Doc = CComQIPtr <Document>(pPartDoc);

		pTrans = pTransManager->MethodStartTransaction(Doc, _T(""));
		// Prorgess Bar
		m_progBarCtrl.SetPos(75);

		pPartComDef->get_Sketches(&sketches);
		pPartComDef->get_Sketches3D(&sketches3D);

		pPartComDef->get_WorkPlanes(&wp);
		pPartComDef->get_Features(&ft);
		pPartComDef->get_WorkAxes(&wax);
		pPartComDef->get_WorkPoints(&wpt);
	}

	// Вращение
	m_progBarCtrl.SetPos(10);

	PlanarSketch *pSketch1;
	sketches->raw_Add(wp->GetItem(3), false, &pSketch1);

	SketchPointPtr point[20];
	SketchLinePtr lines[20];

	SketchPoints *skPoints;
	SketchLines *skLines;
	Profiles *skProfiles;


	pSketch1->get_SketchPoints(&skPoints);
	pSketch1->get_SketchLines(&skLines);
	pSketch1->get_Profiles(&skProfiles);

	double dimetrY = (modul * number) / 10;

	point[0] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d((diameter / 2) + 0.1, 0), false);
	point[1] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d((dimetrY + ((modul * 2) / 10)) / 2, 0), false);
	point[2] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d((dimetrY + ((modul * 2) / 10)) / 2, lenght3 / 1.8), false);
	point[3] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d((diameter / 2) + 0.1, lenght3 / 1.8), false);

	lines[0] = skLines->MethodAddByTwoPoints(point[0], point[1]);
	lines[1] = skLines->MethodAddByTwoPoints(point[1], point[2]);
	lines[2] = skLines->MethodAddByTwoPoints(point[2], point[3]);
	lines[3] = skLines->MethodAddByTwoPoints(point[3], point[0]);

	Profile *pProfile;
	skProfiles->raw__AddForSolid(&pProfile);

	RevolveFeatures *ftRevolve;
	ft->get_RevolveFeatures(&ftRevolve);

	RevolveFeaturePtr revolve = ftRevolve->MethodAddFull(pProfile, wax->GetItem(2), kNewBodyOperation);

	double step = 3.14 * modul;
	double thickness = step * 0.5;
	double down = (thickness*0.5) / 10;
	double up = (1.75*thickness) / 10;

	double r = (dimetrY + ((modul * 2) / 10)) / 2;

	double rblue = (dimetrY - ((modul * 2.25) / 10)) / 2;

	double Yup = sqrt(pow(r, 2) - pow(up / 2, 2));

	double Ydown = sqrt(pow(rblue, 2) - pow(up / 2, 2));

	m_progBarCtrl.SetPos(25);

	PlanarSketch *pSketch2;
	sketches->raw_Add(wp->GetItem(2), false, &pSketch2);

	SketchPointPtr point2[4];
	SketchLinePtr lines2[2];
	SketchArcPtr arc2[2];

	SketchPoints *skPoints2;
	SketchLines *skLines2;
	Profiles *skProfiles2;
	SketchArcs *skArc2;

	pSketch2->get_SketchPoints(&skPoints2);
	pSketch2->get_SketchLines(&skLines2);
	pSketch2->get_Profiles(&skProfiles2);
	pSketch2->get_SketchArcs(&skArc2);

	point2[0] = skPoints2->MethodAdd(pTransGeom->MethodCreatePoint2d(up / 2, Yup), false);
	point2[1] = skPoints2->MethodAdd(pTransGeom->MethodCreatePoint2d(-up / 2, Yup), false);
	point2[2] = skPoints2->MethodAdd(pTransGeom->MethodCreatePoint2d(down / 2, Ydown), false);
	point2[3] = skPoints2->MethodAdd(pTransGeom->MethodCreatePoint2d(-down / 2, Ydown), false);

	lines2[0] = skLines2->MethodAddByTwoPoints(point2[0], point2[2]);
	lines2[1] = skLines2->MethodAddByTwoPoints(point2[1], point2[3]);


	arc2[0] = skArc2->MethodAddByThreePoints(point2[0], pTransGeom->MethodCreatePoint2d(0, r), point2[1]);
	arc2[1] = skArc2->MethodAddByThreePoints(point2[2], pTransGeom->MethodCreatePoint2d(0, rblue), point2[3]);


	Profile *pProfile2;
	skProfiles2->raw__AddForSolid(&pProfile2);

	ExtrudeFeatures *ftExtrude;
	ft->get_ExtrudeFeatures(&ftExtrude);

	ExtrudeFeaturePtr extrude = ftExtrude->MethodAddByDistanceExtent(pProfile2, lenght3 / 1.8, kPositiveExtentDirection, kCutOperation);


	ObjectCollection *pCollection;

	pInvApp->TransientObjects->raw_CreateObjectCollection(vtMissing, &pCollection);

	pCollection->MethodAdd(extrude);

	CircularPatternFeatures *pCircPatFeat;

	ft->get_CircularPatternFeatures(&pCircPatFeat);

	CircularPatternFeature *circFeat = pCircPatFeat->MethodAdd(pCollection, wax->GetItem(2), true, number, "360 град", true, kIdenticalCompute);



	// вырез
	PlanarSketch *pSketch3;
	sketches->raw_Add(wp->GetItem(3), false, &pSketch3);

	SketchPointPtr point3[4];
	SketchLinePtr lines3[4];
	SketchArcPtr arc3[2];

	SketchPoints *skPoints3;
	SketchLines *skLines3;
	Profiles *skProfiles3;
	SketchArcs *skArc3;

	pSketch3->get_SketchPoints(&skPoints3);
	pSketch3->get_SketchLines(&skLines3);
	pSketch3->get_Profiles(&skProfiles3);
	pSketch3->get_SketchArcs(&skArc3);

	//центр
	double bla = (lenght3 / 1.8) / 2;

	point3[0] = skPoints3->MethodAdd(pTransGeom->MethodCreatePoint2d(0.5, bla - (lenght3 / 4) + 0.5), false);
	point3[1] = skPoints3->MethodAdd(pTransGeom->MethodCreatePoint2d(0.5, bla + (lenght3 / 4) - 0.5), false);
	point3[2] = skPoints3->MethodAdd(pTransGeom->MethodCreatePoint2d(-0.5, bla - (lenght3 / 4) + 0.5), false);
	point3[3] = skPoints3->MethodAdd(pTransGeom->MethodCreatePoint2d(-0.5, bla + (lenght3 / 4) - 0.5), false);

	lines3[0] = skLines3->MethodAddByTwoPoints(point3[0], point3[1]);
	lines3[1] = skLines3->MethodAddByTwoPoints(point3[2], point3[3]);

	arc3[0] = skArc3->MethodAddByThreePoints(point3[0], pTransGeom->MethodCreatePoint2d(0, bla - (lenght3 / 4)), point3[2]);
	arc3[1] = skArc3->MethodAddByThreePoints(point3[1], pTransGeom->MethodCreatePoint2d(0, bla + (lenght3 / 4)), point3[3]);

	Profile *pProfile4;

	skProfiles3->raw__AddForSolid(&pProfile4);

	ExtrudeFeatures *ftExtrude3;
	ft->get_ExtrudeFeatures(&ftExtrude3);

	ExtrudeFeaturePtr extrude3 = ftExtrude3->MethodAddByDistanceExtent(pProfile4, (lenght3 / 2) + 1, kPositiveExtentDirection, kCutOperation);

	m_progBarCtrl.SetPos(100);

	if (save == 1)
	{
		pPartDoc->MethodSaveAs(buf + _bstr_t("\\Колесо 1.ipt"), true);

		pTrans = pTransManager->MethodEndTransaction();
	}
	if (save == 2)
	{
		pPartDoc->MethodSaveAs(buf + _bstr_t("\\Колесо 2.ipt"), true);

		pTrans = pTransManager->MethodEndTransaction();
	}

}

//Функция построения подшипника
void CAssemblyConfiguratorView::Bearing(double out, double in, double diametr, double lenght, int save, TCHAR * buf)
{
	//Поиск приложения по ID
	UpdateData(true);
	m_progBarCtrl.SetPos(0);
	CLSID InvAppClsid;
	HRESULT hRes = CLSIDFromProgID(L"Inventor.Application", &InvAppClsid);
	if (FAILED(hRes)) {
		pInvApp = nullptr;
		return;
	}

	//Проверка запущено ли приложение
	CComPtr<IUnknown> pInvAppUnk = nullptr;
	hRes = ::GetActiveObject(InvAppClsid, NULL, &pInvAppUnk);
	if (FAILED(hRes)) {
		MessageBox(_T("Запустите Inventor!"));
		return;
	}
	else
	{
		// Получаем указатель на запущенное приложение (hRes)
		// если происходит повторный запуск то указатель уже существует и с ним не надо ничего   делать
		if (!pInvApp)
			hRes = pInvAppUnk->QueryInterface(__uuidof(Application), (void **)&pInvApp);

		if (FAILED(hRes)) {
			return;
		}

		pInvApp->put_Visible(TRUE);
		// Prorgess Bar
		m_progBarCtrl.SetPos(40);

		pPartDoc = pInvApp->Documents->MethodAdd(kPartDocumentObject, pInvApp->FileManager->MethodGetTemplateFile(kPartDocumentObject, kMetricSystemOfMeasure, kGOST_DraftingStandard), true);

		char name[20];
		sprintf_s(name, "%S", m_CSName);

		pPartDoc->DisplayName = name;

		pPartDoc->get_ComponentDefinition(&pPartComDef);

		pInvApp->get_TransientGeometry(&pTransGeom);
		// Prorgess Bar
		m_progBarCtrl.SetPos(65);

		pTransManager = pInvApp->GetTransactionManager();

		Doc = CComQIPtr <Document>(pPartDoc);

		pTrans = pTransManager->MethodStartTransaction(Doc, _T(""));
		// Prorgess Bar
		m_progBarCtrl.SetPos(75);

		pPartComDef->get_Sketches(&sketches);
		pPartComDef->get_Sketches3D(&sketches3D);

		pPartComDef->get_WorkPlanes(&wp);
		pPartComDef->get_Features(&ft);
		pPartComDef->get_WorkAxes(&wax);
		pPartComDef->get_WorkPoints(&wpt);
	}

	// Вращение
	m_progBarCtrl.SetPos(10);

	PlanarSketch *pSketch1;
	sketches->raw_Add(wp->GetItem(3), false, &pSketch1);

	SketchPointPtr point[12];
	SketchLinePtr lines[10];
	SketchArcPtr arc[2];

	SketchPoints *skPoints;
	SketchLines *skLines;
	Profiles *skProfiles;
	SketchArcs *skArc;

	pSketch1->get_SketchPoints(&skPoints);
	pSketch1->get_SketchLines(&skLines);
	pSketch1->get_Profiles(&skProfiles);
	pSketch1->get_SketchArcs(&skArc);

	point[0] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(diametr / 2 + 0.1, 0), false);
	point[1] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(diametr / 2 + 0.1, lenght / 1.8), false);
	point[2] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(in / 2, lenght / 1.8), false);
	point[3] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(in / 2, (lenght / 1.8)*0.9), false);
	point[4] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(in / 2, (lenght / 1.8)*0.1), false);
	point[5] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(in / 2, 0), false);

	lines[0] = skLines->MethodAddByTwoPoints(point[0], point[1]);
	lines[1] = skLines->MethodAddByTwoPoints(point[1], point[2]);
	lines[2] = skLines->MethodAddByTwoPoints(point[2], point[3]);
	lines[3] = skLines->MethodAddByTwoPoints(point[4], point[5]);
	lines[4] = skLines->MethodAddByTwoPoints(point[5], point[0]);

	double rarc = (in - diametr) / 4;

	arc[0] = skArc->MethodAddByThreePoints(point[3], pTransGeom->MethodCreatePoint2d(diametr / 2 + rarc + 0.1, (lenght / 1.8) / 2), point[4]);

	point[6] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(out / 2, 0), false);
	point[7] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(out / 2, lenght / 1.8), false);
	point[8] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d((out / 2) - (rarc * 2), lenght / 1.8), false);
	point[9] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d((out / 2) - (rarc * 2), (lenght / 1.8)*0.9), false);
	point[10] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d((out / 2) - (rarc * 2), (lenght / 1.8)*0.1), false);
	point[11] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d((out / 2) - (rarc * 2), 0), false);


	lines[5] = skLines->MethodAddByTwoPoints(point[6], point[7]);
	lines[6] = skLines->MethodAddByTwoPoints(point[7], point[8]);
	lines[7] = skLines->MethodAddByTwoPoints(point[8], point[9]);
	lines[8] = skLines->MethodAddByTwoPoints(point[10], point[11]);
	lines[9] = skLines->MethodAddByTwoPoints(point[11], point[6]);


	arc[1] = skArc->MethodAddByThreePoints(point[9], pTransGeom->MethodCreatePoint2d(out / 2 - rarc, (lenght / 1.8) / 2), point[10]);


	Profile *pProfile;
	skProfiles->raw__AddForSolid(&pProfile);

	RevolveFeatures *ftRevolve;
	ft->get_RevolveFeatures(&ftRevolve);

	RevolveFeaturePtr revolve = ftRevolve->MethodAddFull(pProfile, wax->GetItem(2), kNewBodyOperation);

	m_progBarCtrl.SetPos(25);

	PlanarSketch *pSketch2;
	sketches->raw_Add(wp->GetItem(1), false, &pSketch2);

	SketchPointPtr point2[2];
	SketchLinePtr lines2[1];
	SketchArcPtr arc2[1];

	SketchPoints *skPoints2;
	SketchLines *skLines2;
	Profiles *skProfiles2;
	SketchArcs *skArc2;

	pSketch2->get_SketchPoints(&skPoints2);
	pSketch2->get_SketchLines(&skLines2);
	pSketch2->get_Profiles(&skProfiles2);
	pSketch2->get_SketchArcs(&skArc2);

	double b = out / 2;
	double c = (diametr / 2) + 0.1;
	double a = (b - c);
	double ball = a - rarc * 2;

	double centr = c + rarc + (ball / 2);
	double f = (ball * 0.8) / 2;
	double Ycoord1 = centr + f;
	double Ycoord2 = centr - f;
	double Xcoord = (lenght / 1.8) / 2;

	point2[0] = skPoints2->MethodAdd(pTransGeom->MethodCreatePoint2d(Xcoord, Ycoord1), false);
	point2[1] = skPoints2->MethodAdd(pTransGeom->MethodCreatePoint2d(Xcoord, Ycoord2), false);

	lines2[0] = skLines2->MethodAddByTwoPoints(point2[0], point2[1]);

	arc2[0] = skArc2->MethodAddByThreePoints(point2[0], pTransGeom->MethodCreatePoint2d((lenght / 1.8)*0.8, centr), point2[1]);

	Profile *pProfile2;
	skProfiles2->raw__AddForSolid(&pProfile2);

	RevolveFeatures *ftRevolve2;
	ft->get_RevolveFeatures(&ftRevolve2);

	RevolveFeaturePtr revolve2 = ftRevolve2->MethodAddFull(pProfile2, lines2[0], kNewBodyOperation);

	m_progBarCtrl.SetPos(50);

	int number = floor(out);

	ObjectCollection *pCollection;

	pInvApp->TransientObjects->raw_CreateObjectCollection(vtMissing, &pCollection);

	pCollection->MethodAdd(revolve2);

	CircularPatternFeatures *pCircPatFeat;

	ft->get_CircularPatternFeatures(&pCircPatFeat);

	CircularPatternFeature *circFeat = pCircPatFeat->MethodAdd(pCollection, wax->GetItem(2), true, number, "360 град", true, kIdenticalCompute);


	// вырез
	PlanarSketch *pSketch3;
	sketches->raw_Add(wp->GetItem(3), false, &pSketch3);

	SketchPointPtr point3[4];
	SketchLinePtr lines3[4];
	SketchArcPtr arc3[2];

	SketchPoints *skPoints3;
	SketchLines *skLines3;
	Profiles *skProfiles3;
	SketchArcs *skArc3;

	pSketch3->get_SketchPoints(&skPoints3);
	pSketch3->get_SketchLines(&skLines3);
	pSketch3->get_Profiles(&skProfiles3);
	pSketch3->get_SketchArcs(&skArc3);

	//центр
	double bla = (lenght / 1.8) / 2;

	point3[0] = skPoints3->MethodAdd(pTransGeom->MethodCreatePoint2d(0.5, bla - (lenght / 4) + 0.5), false);
	point3[1] = skPoints3->MethodAdd(pTransGeom->MethodCreatePoint2d(0.5, bla + (lenght / 4) - 0.5), false);
	point3[2] = skPoints3->MethodAdd(pTransGeom->MethodCreatePoint2d(-0.5, bla - (lenght / 4) + 0.5), false);
	point3[3] = skPoints3->MethodAdd(pTransGeom->MethodCreatePoint2d(-0.5, bla + (lenght / 4) - 0.5), false);

	lines3[0] = skLines3->MethodAddByTwoPoints(point3[0], point3[1]);
	lines3[1] = skLines3->MethodAddByTwoPoints(point3[2], point3[3]);

	arc3[0] = skArc3->MethodAddByThreePoints(point3[0], pTransGeom->MethodCreatePoint2d(0, bla - (lenght / 4)), point3[2]);
	arc3[1] = skArc3->MethodAddByThreePoints(point3[1], pTransGeom->MethodCreatePoint2d(0, bla + (lenght / 4)), point3[3]);

	Profile *pProfile4;

	skProfiles3->raw__AddForSolid(&pProfile4);

	ExtrudeFeatures *ftExtrude3;
	ft->get_ExtrudeFeatures(&ftExtrude3);

	ExtrudeFeaturePtr extrude3 = ftExtrude3->MethodAddByDistanceExtent(pProfile4, (lenght / 2) + 1, kPositiveExtentDirection, kCutOperation);

	if (save == 1)
	{
		pPartDoc->MethodSaveAs(buf + _bstr_t("\\Подшипник 1.ipt"), true);

		pTrans = pTransManager->MethodEndTransaction();
	}
	if (save == 2)
	{
		pPartDoc->MethodSaveAs(buf + _bstr_t("\\Подшипник 2.ipt"), true);

		pTrans = pTransManager->MethodEndTransaction();
	}
	if (save == 3)
	{
		pPartDoc->MethodSaveAs(buf + _bstr_t("\\Подшипник 3.ipt"), true);

		pTrans = pTransManager->MethodEndTransaction();
	}
	if (save == 4)
	{
		pPartDoc->MethodSaveAs(buf + _bstr_t("\\Подшипник 4.ipt"), true);

		pTrans = pTransManager->MethodEndTransaction();
	}
	if (save == 5)
	{
		pPartDoc->MethodSaveAs(buf + _bstr_t("\\Подшипник 5.ipt"), true);

		pTrans = pTransManager->MethodEndTransaction();
	}
	if (save == 6)
	{
		pPartDoc->MethodSaveAs(buf + _bstr_t("\\Подшипник 6.ipt"), true);

		pTrans = pTransManager->MethodEndTransaction();
	}

	m_progBarCtrl.SetPos(100);
	m_progBarCtrl.SetPos(0);
}

// Сборка
void  CAssemblyConfiguratorView::Assembly(TCHAR * buf)
{
	// Указатель на менеджер по построению геометрии
	TransientGeometry *p_TransGeom;

	CLSID InvAppClsid;
	HRESULT hRes = CLSIDFromProgID(L"Inventor.Application", &InvAppClsid);
	if (FAILED(hRes)) {
		pInvApp = nullptr;
		return;
	}

	//Проверка запущено ли приложение
	CComPtr<IUnknown> pInvAppUnk = nullptr;
	hRes = ::GetActiveObject(InvAppClsid, NULL, &pInvAppUnk);
	if (FAILED(hRes)) {
		MessageBox(_T("Запустите Inventor!"));
		return;
	}
	else
	{
		// Получаем указатель на запущенное приложение (hRes)
		// если происходит повторный запуск то указатель уже существует и с ним не надо ничего   делать
		if (!pInvApp)
			hRes = pInvAppUnk->QueryInterface(__uuidof(Application), (void **)&pInvApp);

		if (FAILED(hRes)) {
			return;
		}
		pInvApp->put_Visible(TRUE);
		pInvApp->get_TransientGeometry(&p_TransGeom);
	}
		

	// 
	AssemblyDocumentPtr p_AssemblyDoc;

	p_AssemblyDoc = pInvApp->Documents->MethodAdd(kAssemblyDocumentObject,
		pInvApp->FileManager->MethodGetTemplateFile(
			kAssemblyDocumentObject,
			kMetricSystemOfMeasure,
			kGOST_DraftingStandard)
		, true);

	ComponentDefinitionPtr p_ComponentDef;
	p_ComponentDef = p_AssemblyDoc->GetComponentDefinition();

	MatrixPtr p_PosMatrix;
	p_PosMatrix = p_TransGeom->MethodCreateMatrix();

	AssemblyComponentDefinitionPtr p_AssemCompDef;
	p_AssemCompDef = p_AssemblyDoc->GetComponentDefinition();



	ComponentOccurrencePtr Shaft1;
	Shaft1 = p_ComponentDef->GetOccurrences()->MethodAdd(buf + _bstr_t("\\Вал 1.ipt"), p_PosMatrix);

	ComponentOccurrencePtr Shaft2;
	Shaft2 = p_ComponentDef->GetOccurrences()->MethodAdd(buf + _bstr_t("\\Вал 2.ipt"), p_PosMatrix);

	ComponentOccurrencePtr Shaft3;
	Shaft3 = p_ComponentDef->GetOccurrences()->MethodAdd(buf + _bstr_t("\\Вал 3.ipt"), p_PosMatrix);

	ComponentOccurrencePtr Gear1_1;
	Gear1_1 = p_ComponentDef->GetOccurrences()->MethodAdd(buf + _bstr_t("\\Колесо 1.ipt"), p_PosMatrix);

	ComponentOccurrencePtr Gear2_2;
	Gear2_2 = p_ComponentDef->GetOccurrences()->MethodAdd(buf + _bstr_t("\\Колесо 2.ipt"), p_PosMatrix);

	ComponentOccurrencePtr Gear1;
	Gear1 = p_ComponentDef->GetOccurrences()->MethodAdd(buf + _bstr_t("\\Шестерня 1.ipt"), p_PosMatrix);

	ComponentOccurrencePtr Gear2;
	Gear2 = p_ComponentDef->GetOccurrences()->MethodAdd(buf + _bstr_t("\\Шестерня 2.ipt"), p_PosMatrix);

	ComponentOccurrencePtr Bearing1;
	Bearing1 = p_ComponentDef->GetOccurrences()->MethodAdd(buf + _bstr_t("\\Подшипник 1.ipt"), p_PosMatrix);

	ComponentOccurrencePtr Bearing2;
	Bearing2 = p_ComponentDef->GetOccurrences()->MethodAdd(buf + _bstr_t("\\Подшипник 2.ipt"), p_PosMatrix);

	ComponentOccurrencePtr Bearing3;
	Bearing3 = p_ComponentDef->GetOccurrences()->MethodAdd(buf + _bstr_t("\\Подшипник 3.ipt"), p_PosMatrix);

	ComponentOccurrencePtr Bearing4;
	Bearing4 = p_ComponentDef->GetOccurrences()->MethodAdd(buf + _bstr_t("\\Подшипник 4.ipt"), p_PosMatrix);

	ComponentOccurrencePtr Bearing5;
	Bearing5 = p_ComponentDef->GetOccurrences()->MethodAdd(buf + _bstr_t("\\Подшипник 5.ipt"), p_PosMatrix);

	ComponentOccurrencePtr Bearing6;
	Bearing6 = p_ComponentDef->GetOccurrences()->MethodAdd(buf + _bstr_t("\\Подшипник 6.ipt"), p_PosMatrix);




	//  оси
	WorkAxisProxyPtr p_WorkAxis1;
	WorkAxisProxyPtr p_WorkAxis2;

	//  плоскости
	WorkPlaneProxyPtr p_WorkPlane1;
	WorkPlaneProxyPtr p_WorkPlane2;

	//  грани
	FaceProxyPtr p_Face1;
	FaceProxyPtr p_Face2;

	//Валы
	PartComponentDefinitionPtr Shaft1Def = Shaft1->Definition;
	PartComponentDefinitionPtr Shaft2Def = Shaft2->Definition;
	PartComponentDefinitionPtr Shaft3Def = Shaft3->Definition;

	////Шестерни
	PartComponentDefinitionPtr Gear1Def = Gear1->Definition;
	PartComponentDefinitionPtr Gear2Def = Gear2->Definition;
	
	////Колеса
	PartComponentDefinitionPtr Gear1_1Def = Gear1_1->Definition;
	PartComponentDefinitionPtr Gear2_2Def = Gear2_2->Definition;
	//Подшипники
	PartComponentDefinitionPtr Bearing1Def = Bearing1->Definition;
	PartComponentDefinitionPtr Bearing2Def = Bearing2->Definition;
	PartComponentDefinitionPtr Bearing3Def = Bearing3->Definition;
	PartComponentDefinitionPtr Bearing4Def = Bearing4->Definition;
	PartComponentDefinitionPtr Bearing5Def = Bearing5->Definition;
	PartComponentDefinitionPtr Bearing6Def = Bearing6->Definition;

	//1 и 3
	Shaft1->MethodCreateGeometryProxy(Shaft1Def->WorkAxes->Item[1], (IDispatch**)&p_WorkAxis1);
	Shaft3->MethodCreateGeometryProxy(Shaft3Def->WorkAxes->Item[1], (IDispatch**)&p_WorkAxis2);
	p_AssemCompDef->Constraints->MethodAddMateConstraint(p_WorkAxis1, p_WorkAxis2, 4.5, kNoInference, kNoInference);

	Shaft1->MethodCreateGeometryProxy(Shaft1Def->WorkAxes->Item[2], (IDispatch**)&p_WorkAxis1);
	Shaft3->MethodCreateGeometryProxy(Shaft3Def->WorkAxes->Item[2], (IDispatch**)&p_WorkAxis2);
	p_AssemCompDef->Constraints->MethodAddMateConstraint(p_WorkAxis1, p_WorkAxis2, 24, kNoInference, kNoInference);

	//Shaft1->MethodCreateGeometryProxy(Shaft1Def->WorkPlanes->Item[1], (IDispatch**)&p_WorkPlane1);
	//Shaft3->MethodCreateGeometryProxy(Shaft3Def->WorkPlanes->Item[1], (IDispatch**)&p_WorkPlane2);
	//p_AssemCompDef->Constraints->MethodAddMateConstraint(p_WorkPlane1, p_WorkPlane2, 0, kNoInference, kNoInference);

	//2 и 3
	Shaft1->MethodCreateGeometryProxy(Shaft1Def->WorkAxes->Item[2], (IDispatch**)&p_WorkAxis1);
	Shaft2->MethodCreateGeometryProxy(Shaft2Def->WorkAxes->Item[2], (IDispatch**)&p_WorkAxis2);
	p_AssemCompDef->Constraints->MethodAddMateConstraint(p_WorkAxis1, p_WorkAxis2, 42, kNoInference, kNoInference);

	Shaft1->MethodCreateGeometryProxy(Shaft1Def->WorkAxes->Item[1], (IDispatch**)&p_WorkAxis1);
	Shaft2->MethodCreateGeometryProxy(Shaft2Def->WorkAxes->Item[1], (IDispatch**)&p_WorkAxis2);
	p_AssemCompDef->Constraints->MethodAddMateConstraint(p_WorkAxis1, p_WorkAxis2, 0, kNoInference, kNoInference);

	// колесо на 1 вал
	Gear1_1->MethodCreateGeometryProxy(Gear1_1Def->WorkAxes->Item[2], (IDispatch**)&p_WorkAxis1);
	Shaft1->MethodCreateGeometryProxy(Shaft1Def->WorkAxes->Item[2], (IDispatch**)&p_WorkAxis2);
	p_AssemCompDef->Constraints->MethodAddMateConstraint(p_WorkAxis1, p_WorkAxis2, 0, kNoInference, kNoInference);
	
	Gear1_1->MethodCreateGeometryProxy(Gear1_1Def->WorkAxes->Item[3], (IDispatch**)&p_WorkAxis1);
	Shaft1->MethodCreateGeometryProxy(Shaft1Def->WorkAxes->Item[3], (IDispatch**)&p_WorkAxis2);
	p_AssemCompDef->Constraints->MethodAddMateConstraint(p_WorkAxis1, p_WorkAxis2, 14.5, kNoInference, kNoInference);

	// шестерня на 3 вал(длинный)
	Gear1->MethodCreateGeometryProxy(Gear1Def->WorkAxes->Item[2], (IDispatch**)&p_WorkAxis1);
	Shaft3->MethodCreateGeometryProxy(Shaft3Def->WorkAxes->Item[2], (IDispatch**)&p_WorkAxis2);
	p_AssemCompDef->Constraints->MethodAddMateConstraint(p_WorkAxis1, p_WorkAxis2, 0, kNoInference, kNoInference);

	Gear1->MethodCreateGeometryProxy(Gear1Def->WorkAxes->Item[3], (IDispatch**)&p_WorkAxis1);
	Shaft3->MethodCreateGeometryProxy(Shaft3Def->WorkAxes->Item[3], (IDispatch**)&p_WorkAxis2);
	p_AssemCompDef->Constraints->MethodAddMateConstraint(p_WorkAxis1, p_WorkAxis2, 20, kNoInference, kNoInference);

	// колесо на 3 вал(длинный)
	Gear2_2->MethodCreateGeometryProxy(Gear2_2Def->WorkAxes->Item[2], (IDispatch**)&p_WorkAxis1);
	Shaft3->MethodCreateGeometryProxy(Shaft3Def->WorkAxes->Item[2], (IDispatch**)&p_WorkAxis2);
	p_AssemCompDef->Constraints->MethodAddMateConstraint(p_WorkAxis1, p_WorkAxis2, 0, kNoInference, kNoInference);

	Gear2_2->MethodCreateGeometryProxy(Gear2_2Def->WorkAxes->Item[3], (IDispatch**)&p_WorkAxis1);
	Shaft3->MethodCreateGeometryProxy(Shaft3Def->WorkAxes->Item[3], (IDispatch**)&p_WorkAxis2);
	p_AssemCompDef->Constraints->MethodAddMateConstraint(p_WorkAxis1, p_WorkAxis2, 10, kNoInference, kNoInference);

	// шестерня на 2 вал
	Gear2->MethodCreateGeometryProxy(Gear2Def->WorkAxes->Item[2], (IDispatch**)&p_WorkAxis1);
	Shaft2->MethodCreateGeometryProxy(Shaft2Def->WorkAxes->Item[2], (IDispatch**)&p_WorkAxis2);
	p_AssemCompDef->Constraints->MethodAddMateConstraint(p_WorkAxis1, p_WorkAxis2, 0, kNoInference, kNoInference);

	Gear2->MethodCreateGeometryProxy(Gear2Def->WorkAxes->Item[3], (IDispatch**)&p_WorkAxis1);
	Shaft2->MethodCreateGeometryProxy(Shaft2Def->WorkAxes->Item[3], (IDispatch**)&p_WorkAxis2);
	p_AssemCompDef->Constraints->MethodAddMateConstraint(p_WorkAxis1, p_WorkAxis2, 14.5, kNoInference, kNoInference);

	//1Подшипник на 1 вал

	Bearing1->MethodCreateGeometryProxy(Bearing1Def->WorkAxes->Item[2], (IDispatch**)&p_WorkAxis1);
	Shaft1->MethodCreateGeometryProxy(Shaft1Def->WorkAxes->Item[2], (IDispatch**)&p_WorkAxis2);
	p_AssemCompDef->Constraints->MethodAddMateConstraint(p_WorkAxis1, p_WorkAxis2, 0, kNoInference, kNoInference);

	Bearing1->MethodCreateGeometryProxy(Bearing1Def->WorkAxes->Item[3], (IDispatch**)&p_WorkAxis1);
	Shaft1->MethodCreateGeometryProxy(Shaft1Def->WorkAxes->Item[3], (IDispatch**)&p_WorkAxis2);
	p_AssemCompDef->Constraints->MethodAddMateConstraint(p_WorkAxis1, p_WorkAxis2, 20, kNoInference, kNoInference);

	//2Подшипник на 1 вал

	Bearing2->MethodCreateGeometryProxy(Bearing2Def->WorkAxes->Item[2], (IDispatch**)&p_WorkAxis1);
	Shaft1->MethodCreateGeometryProxy(Shaft1Def->WorkAxes->Item[2], (IDispatch**)&p_WorkAxis2);
	p_AssemCompDef->Constraints->MethodAddMateConstraint(p_WorkAxis1, p_WorkAxis2, 0, kNoInference, kNoInference);

	Bearing2->MethodCreateGeometryProxy(Bearing2Def->WorkAxes->Item[3], (IDispatch**)&p_WorkAxis1);
	Shaft1->MethodCreateGeometryProxy(Shaft1Def->WorkAxes->Item[3], (IDispatch**)&p_WorkAxis2);
	p_AssemCompDef->Constraints->MethodAddMateConstraint(p_WorkAxis1, p_WorkAxis2, 10, kNoInference, kNoInference);

	//1Подшипник на 3 вал

	Bearing3->MethodCreateGeometryProxy(Bearing3Def->WorkAxes->Item[2], (IDispatch**)&p_WorkAxis1);
	Shaft3->MethodCreateGeometryProxy(Shaft3Def->WorkAxes->Item[2], (IDispatch**)&p_WorkAxis2);
	p_AssemCompDef->Constraints->MethodAddMateConstraint(p_WorkAxis1, p_WorkAxis2, 0, kNoInference, kNoInference);

	Bearing3->MethodCreateGeometryProxy(Bearing3Def->WorkAxes->Item[3], (IDispatch**)&p_WorkAxis1);
	Shaft3->MethodCreateGeometryProxy(Shaft3Def->WorkAxes->Item[3], (IDispatch**)&p_WorkAxis2);
	p_AssemCompDef->Constraints->MethodAddMateConstraint(p_WorkAxis1, p_WorkAxis2, 6, kNoInference, kNoInference);

	//2Подшипник на 3 вал

	Bearing4->MethodCreateGeometryProxy(Bearing4Def->WorkAxes->Item[2], (IDispatch**)&p_WorkAxis1);
	Shaft3->MethodCreateGeometryProxy(Shaft3Def->WorkAxes->Item[2], (IDispatch**)&p_WorkAxis2);
	p_AssemCompDef->Constraints->MethodAddMateConstraint(p_WorkAxis1, p_WorkAxis2, 0, kNoInference, kNoInference);

	Bearing4->MethodCreateGeometryProxy(Bearing4Def->WorkAxes->Item[3], (IDispatch**)&p_WorkAxis1);
	Shaft3->MethodCreateGeometryProxy(Shaft3Def->WorkAxes->Item[3], (IDispatch**)&p_WorkAxis2);
	p_AssemCompDef->Constraints->MethodAddMateConstraint(p_WorkAxis1, p_WorkAxis2,25, kNoInference, kNoInference);

	//1Подшипник на 2 вал

	Bearing5->MethodCreateGeometryProxy(Bearing5Def->WorkAxes->Item[2], (IDispatch**)&p_WorkAxis1);
	Shaft2->MethodCreateGeometryProxy(Shaft2Def->WorkAxes->Item[2], (IDispatch**)&p_WorkAxis2);
	p_AssemCompDef->Constraints->MethodAddMateConstraint(p_WorkAxis1, p_WorkAxis2, 0, kNoInference, kNoInference);

	Bearing5->MethodCreateGeometryProxy(Bearing5Def->WorkAxes->Item[3], (IDispatch**)&p_WorkAxis1);
	Shaft2->MethodCreateGeometryProxy(Shaft2Def->WorkAxes->Item[3], (IDispatch**)&p_WorkAxis2);
	p_AssemCompDef->Constraints->MethodAddMateConstraint(p_WorkAxis1, p_WorkAxis2, 10, kNoInference, kNoInference);

	//2Подшипник на 2 вал

	Bearing6->MethodCreateGeometryProxy(Bearing6Def->WorkAxes->Item[2], (IDispatch**)&p_WorkAxis1);
	Shaft2->MethodCreateGeometryProxy(Shaft2Def->WorkAxes->Item[2], (IDispatch**)&p_WorkAxis2);
	p_AssemCompDef->Constraints->MethodAddMateConstraint(p_WorkAxis1, p_WorkAxis2, 0, kNoInference, kNoInference);

	Bearing6->MethodCreateGeometryProxy(Bearing6Def->WorkAxes->Item[3], (IDispatch**)&p_WorkAxis1);
	Shaft2->MethodCreateGeometryProxy(Shaft2Def->WorkAxes->Item[3], (IDispatch**)&p_WorkAxis2);
	p_AssemCompDef->Constraints->MethodAddMateConstraint(p_WorkAxis1, p_WorkAxis2, 20, kNoInference, kNoInference);

}


//Глобальные перемеренные, определяею значения

// Вал 1
double Shaft1_L1 = 80;
double Shaft1_L2 = 50;
double Shaft1_L3 = 60;
double Shaft1_D1 = 40;
double Shaft1_D2 = 50;
double Shaft1_D3 = 60;

// Вал 2
double Shaft2_L1 = 110;
double Shaft2_L2 = 150;
double Shaft2_L3 = 60;
double Shaft2_D1 = 50;
double Shaft2_D2 = 60;
double Shaft2_D3 = 70;

//Шестерня
double PinionModule = 2;
double PinionQuantity = 60;

//Колесо
double WheelModule = 2;
double WheelRatio = 2;
double WheelQuantity = PinionQuantity * WheelRatio;

//Подшипник
double BearingIn = 90;
double BearingOut = 140;

// Флаги для проверки сохранения

int flagsave = 0;


// Обработчики сообщений CAssemblyConfiguratorView
void CAssemblyConfiguratorView::UpdateForm(int Itemselection)
{
	if (Itemselection == -1)
	{
		//DLGStartAssembly dlg;
		//
		//dlg.DoModal();
		m_picture.SetBitmap(::LoadBitmap(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP11)));

		GetDlgItem(IDC_GROUP)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_ERROR)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_TITTLE)->SetWindowTextW(_T("Введите название сборки"));
		GetDlgItem(IDC_BUTTON1)->MoveWindow(820, 90, 120, 25, TRUE);   //Измение положения кнопки "Построить"

		GetDlgItem(IDC_BUTTON3)->MoveWindow(957, 90, 120, 25, TRUE);   //Измение положения кнопки "Запустить Inventor"


		//Элементы

		GetDlgItem(IDC_BUTTON1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTTON3)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTTON2)->ShowWindow(SW_HIDE);


		GetDlgItem(IDC_STATIC_P1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_P2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_P3)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_P4)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_P5)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_P6)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_P7)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_P8)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_P9)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_P10)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_BUTTON5)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_EDIT1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT3)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT4)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT5)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT6)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT7)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT8)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT9)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT10)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_EDIT_NAME)->SetWindowText(_T("Сборка")); //Название документа Inventor
		flag = -1;
	}
	if (Itemselection == 0)
	{

		m_tabs.SetCurSel(0);

		//Првоерка сохранеия детали
		if (flagsave == 1)
		{
			if (AfxMessageBox(_T("Сохранить деталь?"), MB_YESNO | MB_ICONQUESTION) == IDYES)
			{
				CAssemblyConfiguratorView::Save();
			}
		}

		m_picture.SetBitmap(::LoadBitmap(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP1))); //Картинка

		//Размещение элементов, ширина, координаты
		GetDlgItem(IDC_GROUP)->MoveWindow(820, 75, 257, 200, TRUE);   //Измение положения и размера Group Box

		GetDlgItem(IDC_BUTTON5)->MoveWindow(820, 280, 120, 25, TRUE);   //Измение положения кнопки "Построить"

		GetDlgItem(IDC_BUTTON3)->MoveWindow(957, 280, 120, 25, TRUE);   //Измение положения кнопки "Запустить Inventor"
		GetDlgItem(IDC_BUTTON1)->MoveWindow(820, 320, 257, 25, TRUE);
		// Навзания для Static Text
		GetDlgItem(IDC_STATIC_P1)->SetWindowTextW(_T("L1"));
		GetDlgItem(IDC_STATIC_P2)->SetWindowTextW(_T("L2"));
		GetDlgItem(IDC_STATIC_P3)->SetWindowTextW(_T("L3"));

		GetDlgItem(IDC_STATIC_P4)->SetWindowTextW(_T("D1"));
		GetDlgItem(IDC_STATIC_P5)->SetWindowTextW(_T("D2"));
		GetDlgItem(IDC_STATIC_P6)->SetWindowTextW(_T("D3"));

		// Видимость элементов
		GetDlgItem(IDC_BUTTON1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTTON5)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_GROUP)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_GROUP)->ShowWindow(IDC_STATIC_ERROR);
		GetDlgItem(IDC_STATIC_P1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_P2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_P3)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_P4)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_P5)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_P6)->ShowWindow(SW_SHOW);

		GetDlgItem(IDC_EDIT1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT3)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT4)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT5)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT6)->ShowWindow(SW_SHOW);
		//Hide
		/*GetDlgItem(IDC_BUTTON3)->ShowWindow(SW_HIDE);*/
		GetDlgItem(IDC_STATIC_P7)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_P8)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_P9)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_P10)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_EDIT7)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT8)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT9)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT10)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON2)->ShowWindow(SW_HIDE);
		// Знаачения по умолчанию

		// to string
		CString str;
		str.Format(_T("%g"), Shaft1_L1);
		GetDlgItem(IDC_EDIT1)->SetWindowText(str);

		str.Format(_T("%g"), Shaft1_L2);
		GetDlgItem(IDC_EDIT2)->SetWindowText(str);

		str.Format(_T("%g"), Shaft1_L3);
		GetDlgItem(IDC_EDIT3)->SetWindowText(str);

		str.Format(_T("%g"), Shaft1_D1);
		GetDlgItem(IDC_EDIT4)->SetWindowText(str);

		str.Format(_T("%g"), Shaft1_D2);
		GetDlgItem(IDC_EDIT5)->SetWindowText(str);

		str.Format(_T("%g"), Shaft1_D3);
		GetDlgItem(IDC_EDIT6)->SetWindowText(str);

		GetDlgItem(IDC_EDIT_NAME)->SetWindowText(_T("Вал 1")); //Название документа Inventor


		//Исключения и изменения значение и Readonly
		GetDlgItem(IDC_EDIT1)->SendMessage(EM_SETREADONLY, 0, 1);
		GetDlgItem(IDC_EDIT2)->SendMessage(EM_SETREADONLY, 0, 1);
		GetDlgItem(IDC_EDIT3)->SendMessage(EM_SETREADONLY, 0, 1);
		GetDlgItem(IDC_EDIT4)->SendMessage(EM_SETREADONLY, 0, 1);
		GetDlgItem(IDC_EDIT5)->SendMessage(EM_SETREADONLY, 0, 1);
		GetDlgItem(IDC_EDIT6)->SendMessage(EM_SETREADONLY, 0, 1);
		if (flag == 1)
		{
			// l3
			if (Shaft2_L3 != Shaft1_L3)
			{
				Shaft1_L3 = Shaft2_L3;

				str.Format(_T("%g"), Shaft1_L3);

				GetDlgItem(IDC_EDIT3)->SetWindowText(str);
				GetDlgItem(IDC_EDIT3)->SendMessage(EM_SETREADONLY, 1, 0); //режим чтения
			}
			else
			{
				GetDlgItem(IDC_EDIT3)->SendMessage(EM_SETREADONLY, 0, 1); //режим редактирования
			}
			if (Shaft2_D2 != Shaft1_D3)
			{
				Shaft1_D3 = Shaft2_D2;

				str.Format(_T("%g"), Shaft1_D3);

				GetDlgItem(IDC_EDIT6)->SetWindowText(str);
				GetDlgItem(IDC_EDIT6)->SendMessage(EM_SETREADONLY, 1, 0); //режим чтения
			}
			else
			{
				GetDlgItem(IDC_EDIT6)->SendMessage(EM_SETREADONLY, 0, 1); //режим редактирования
			}
			//// l2 и l1
			//if (Shaft1_L2 > Shaft2_L1)
			//{
			//	Shaft1_L2 = Shaft2_L1;

			//	str.Format(_T("%g"), Shaft1_L2);

			//	GetDlgItem(IDC_EDIT2)->SetWindowText(str);
			//	GetDlgItem(IDC_EDIT2)->SendMessage(EM_SETREADONLY, 1, 0); //режим чтения
			//}
			//else
			//{
			//	GetDlgItem(IDC_EDIT2)->SendMessage(EM_SETREADONLY, 0, 1); //режим редактирования
			//	GetDlgItem(IDC_EDIT1)->SendMessage(EM_SETREADONLY, 0, 1); //режим редактирования
			//}
		}

		UpdateData(true);

		// Перевод в милиметры
		m_Variable1 = m_Variable1 / (double)10;
		m_Variable2 = m_Variable2 / (double)10;
		m_Variable3 = m_Variable3 / (double)10;
		m_Variable4 = m_Variable4 / (double)10;
		m_Variable5 = m_Variable5 / (double)10;
		m_Variable6 = m_Variable6 / (double)10;

		// Флажок для кнопки "Построить"
		flag = 0;
		flagsave = 0;
		//CString str;
		//str.Format(_T("%g"), m_Variable1);

		///*m_Variable1 = m_Variable1 /(double)10;*/
	}
	if (Itemselection == 1)
	{
		m_tabs.SetCurSel(0);
		//Првоерка сохранеия детали
		if (flagsave == 1)
		{
			if (AfxMessageBox(_T("Сохранить деталь?"), MB_YESNO | MB_ICONQUESTION) == IDYES)
			{
				CAssemblyConfiguratorView::Save();
			}
		}
		m_picture.SetBitmap(::LoadBitmap(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP3))); //Картинка
																								   //Размещение элементов, ширина, координаты
		GetDlgItem(IDC_GROUP)->MoveWindow(820, 75, 257, 200, TRUE);   //Измение положения и размера Group Box

		GetDlgItem(IDC_BUTTON5)->MoveWindow(820, 280, 120, 25, TRUE);   //Измение положения кнопки "Построить"

		GetDlgItem(IDC_BUTTON3)->MoveWindow(957, 280, 120, 25, TRUE);   //Измение положения кнопки "Запустить Inventor"


		GetDlgItem(IDC_BUTTON1)->MoveWindow(820, 320, 257, 25, TRUE);					// Навзания для Static Text
		GetDlgItem(IDC_STATIC_P1)->SetWindowTextW(_T("L1"));
		GetDlgItem(IDC_STATIC_P2)->SetWindowTextW(_T("L2"));
		GetDlgItem(IDC_STATIC_P3)->SetWindowTextW(_T("L3"));

		GetDlgItem(IDC_STATIC_P4)->SetWindowTextW(_T("D1"));
		GetDlgItem(IDC_STATIC_P5)->SetWindowTextW(_T("D2"));
		GetDlgItem(IDC_STATIC_P6)->SetWindowTextW(_T("D3"));

		// Видимость элементов\
		//Show
		GetDlgItem(IDC_BUTTON5)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_GROUP)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_GROUP)->ShowWindow(IDC_STATIC_ERROR);
		GetDlgItem(IDC_STATIC_P1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_P2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_P3)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_P4)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_P5)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_P6)->ShowWindow(SW_SHOW);

		GetDlgItem(IDC_EDIT1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT3)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT4)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT5)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT6)->ShowWindow(SW_SHOW);
		//Hide
		/*GetDlgItem(IDC_BUTTON3)->ShowWindow(SW_HIDE);*/

		GetDlgItem(IDC_STATIC_P7)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_P8)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_P9)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_P10)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_EDIT7)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT8)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT9)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT10)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON2)->ShowWindow(SW_HIDE);
		// Знаачения по умолчанию

		// to string
		CString str;
		str.Format(_T("%g"), Shaft2_L1);
		GetDlgItem(IDC_EDIT1)->SetWindowText(str);

		str.Format(_T("%g"), Shaft2_L2);
		GetDlgItem(IDC_EDIT2)->SetWindowText(str);

		str.Format(_T("%g"), Shaft2_L3);
		GetDlgItem(IDC_EDIT3)->SetWindowText(str);

		str.Format(_T("%g"), Shaft2_D1);
		GetDlgItem(IDC_EDIT4)->SetWindowText(str);

		str.Format(_T("%g"), Shaft2_D2);
		GetDlgItem(IDC_EDIT5)->SetWindowText(str);

		str.Format(_T("%g"), Shaft2_D3);
		GetDlgItem(IDC_EDIT6)->SetWindowText(str);

		GetDlgItem(IDC_EDIT_NAME)->SetWindowText(_T("Вал 2")); //Название документа Inventor

		// Исключения

		GetDlgItem(IDC_EDIT1)->SendMessage(EM_SETREADONLY, 0, 1);
		GetDlgItem(IDC_EDIT2)->SendMessage(EM_SETREADONLY, 0, 1);
		GetDlgItem(IDC_EDIT3)->SendMessage(EM_SETREADONLY, 0, 1);
		GetDlgItem(IDC_EDIT4)->SendMessage(EM_SETREADONLY, 0, 1);
		GetDlgItem(IDC_EDIT5)->SendMessage(EM_SETREADONLY, 0, 1);
		GetDlgItem(IDC_EDIT6)->SendMessage(EM_SETREADONLY, 0, 1);
		if (flag == 0)
		{
			//l3
			if (Shaft1_L3 != Shaft2_L3)
			{
				Shaft2_L3 = Shaft1_L3;

				CString str;
				str.Format(_T("%g"), Shaft2_L3);

				GetDlgItem(IDC_EDIT3)->SetWindowText(str);
				GetDlgItem(IDC_EDIT3)->SendMessage(EM_SETREADONLY, 1, 0); //режим чтения
			}
			else
			{
				GetDlgItem(IDC_EDIT3)->SendMessage(EM_SETREADONLY, 0, 1); //режим редактирования
			}
			if (Shaft1_D3 != Shaft2_D2)
			{
				Shaft2_D2 = Shaft1_D3;

				str.Format(_T("%g"), Shaft2_D2);

				GetDlgItem(IDC_EDIT5)->SetWindowText(str);
				GetDlgItem(IDC_EDIT5)->SendMessage(EM_SETREADONLY, 1, 0); //режим чтения
			}
			else
			{
				GetDlgItem(IDC_EDIT5)->SendMessage(EM_SETREADONLY, 0, 1); //режим редактирования
			}
			//// l2 и l1
			//if (Shaft1_L2 > Shaft2_L1)
			//{
			//	Shaft2_L1 = Shaft1_L2;

			//	str.Format(_T("%g"), Shaft2_L1);

			//	GetDlgItem(IDC_EDIT1)->SetWindowText(str);
			//	GetDlgItem(IDC_EDIT1)->SendMessage(EM_SETREADONLY, 1, 0); //режим чтения
			//}
			//else
			//{
			//	GetDlgItem(IDC_EDIT2)->SendMessage(EM_SETREADONLY, 0, 1); //режим редактирования
			//	GetDlgItem(IDC_EDIT1)->SendMessage(EM_SETREADONLY, 0, 1); //режим редактирования
			//}
		}

		// Флажок для кнопки "Построить"
		flag = 1;
		flagsave = 0;
	}
	if (Itemselection == 2)
	{
		m_tabs.SetCurSel(0);
		if (flagsave == 1)
		{
			if (AfxMessageBox(_T("Сохранить деталь?"), MB_YESNO | MB_ICONQUESTION) == IDYES)
			{
				CAssemblyConfiguratorView::Save();
			}
		}
		m_picture.SetBitmap(::LoadBitmap(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP5))); //Картинка
																								   //Размещение элементов, ширина, координаты
		GetDlgItem(IDC_GROUP)->MoveWindow(820, 75, 257, 100, TRUE);   //Измение положения и размера Group Box

		GetDlgItem(IDC_BUTTON5)->MoveWindow(820, 180, 120, 25, TRUE);   //Измение положения кнопки "Построить"

		GetDlgItem(IDC_BUTTON3)->MoveWindow(957, 180, 120, 25, TRUE);   //Измение положения кнопки "Запустить Inventor"
		GetDlgItem(IDC_BUTTON1)->MoveWindow(820, 240, 257, 25, TRUE);

																		// Навзания для Static Text
		GetDlgItem(IDC_STATIC_P1)->SetWindowTextW(_T("Модуль"));
		GetDlgItem(IDC_STATIC_P2)->SetWindowTextW(_T("Количество зубьев"));

		// Видимость элементов
		//show
		GetDlgItem(IDC_BUTTON5)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_GROUP)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_GROUP)->ShowWindow(IDC_STATIC_ERROR);
		GetDlgItem(IDC_STATIC_P1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_P2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_P3)->ShowWindow(SW_SHOW);

		GetDlgItem(IDC_EDIT1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT3)->ShowWindow(SW_SHOW);
		//Hide
		/*GetDlgItem(IDC_BUTTON3)->ShowWindow(SW_HIDE);*/
		GetDlgItem(IDC_STATIC_P3)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_P4)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_P5)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_P6)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_P7)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_P8)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_P9)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_P10)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_EDIT3)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT4)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT5)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT6)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT7)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT8)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT9)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT10)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON2)->ShowWindow(SW_HIDE);
		// Знаачения по умолчанию

		// to string
		CString str;
		str.Format(_T("%g"), PinionModule);
		GetDlgItem(IDC_EDIT1)->SetWindowText(str);

		str.Format(_T("%g"), PinionQuantity);
		GetDlgItem(IDC_EDIT2)->SetWindowText(str);

		GetDlgItem(IDC_EDIT_NAME)->SetWindowText(_T("Шестерня")); //Название документа Inventor

		flag = 2;
		flagsave = 0;
	}
	if (Itemselection == 3)
	{
		m_tabs.SetCurSel(0);
		if (flagsave == 1)
		{
			if (AfxMessageBox(_T("Сохранить деталь?"), MB_YESNO | MB_ICONQUESTION) == IDYES)
			{
				CAssemblyConfiguratorView::Save();
			}
		}
		m_picture.SetBitmap(::LoadBitmap(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP7))); //Картинка
																								   //Размещение элементов, ширина, координаты
		GetDlgItem(IDC_GROUP)->MoveWindow(820, 75, 257, 130, TRUE);   //Измение положения и размера Group Box

		GetDlgItem(IDC_BUTTON5)->MoveWindow(820, 210, 120, 25, TRUE);   //Измение положения кнопки "Построить"

		GetDlgItem(IDC_BUTTON3)->MoveWindow(957, 210, 120, 25, TRUE);   //Измение положения кнопки "Запустить Inventor"
		GetDlgItem(IDC_BUTTON1)->MoveWindow(820, 240, 257, 25, TRUE);

																		// Навзания для Static Text
		GetDlgItem(IDC_STATIC_P1)->SetWindowTextW(_T("Модуль"));
		GetDlgItem(IDC_STATIC_P2)->SetWindowTextW(_T("Передаточное отношение"));
		GetDlgItem(IDC_STATIC_P3)->SetWindowTextW(_T("Количество зубьев"));


		// Видимость элементов
		//show
		GetDlgItem(IDC_BUTTON5)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_GROUP)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_GROUP)->ShowWindow(IDC_STATIC_ERROR);
		GetDlgItem(IDC_STATIC_P1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_P2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_P3)->ShowWindow(SW_SHOW);

		GetDlgItem(IDC_EDIT1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT3)->ShowWindow(SW_SHOW);
		//Hide
		/*GetDlgItem(IDC_BUTTON3)->ShowWindow(SW_HIDE);*/
		GetDlgItem(IDC_STATIC_P4)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_P5)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_P6)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_P7)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_P8)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_P9)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_P10)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_EDIT4)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT5)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT6)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT7)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT8)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT9)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT10)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON2)->ShowWindow(SW_HIDE);
		// Знаачения по умолчанию

		// to string
		CString str;
		str.Format(_T("%g"), WheelModule);
		GetDlgItem(IDC_EDIT1)->SetWindowText(str);

		str.Format(_T("%g"), WheelRatio);
		GetDlgItem(IDC_EDIT2)->SetWindowText(str);

		str.Format(_T("%g"), WheelQuantity);
		GetDlgItem(IDC_EDIT3)->SetWindowText(str);

		GetDlgItem(IDC_EDIT_NAME)->SetWindowText(_T("Колесо")); //Название документа Inventor

		flag = 3;
		flagsave = 0;
	}
	if (Itemselection == 4)
	{
		m_tabs.SetCurSel(0);
		if (flagsave == 1)
		{
			if (AfxMessageBox(_T("Сохранить деталь?"), MB_YESNO | MB_ICONQUESTION) == IDYES)
			{
				CAssemblyConfiguratorView::Save();
			}
		}
		m_picture.SetBitmap(::LoadBitmap(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP9))); //Картинка
																								   //Размещение элементов, ширина, координаты
		GetDlgItem(IDC_GROUP)->MoveWindow(820, 75, 257, 100, TRUE);   //Измение положения и размера Group Box

		GetDlgItem(IDC_BUTTON5)->MoveWindow(820, 180, 120, 25, TRUE);   //Измение положения кнопки "Построить"

		GetDlgItem(IDC_BUTTON3)->MoveWindow(957, 180, 120, 25, TRUE);   //Измение положения кнопки "Запустить Inventor"

		GetDlgItem(IDC_BUTTON1)->MoveWindow(820, 240, 257, 25, TRUE);
																		// Навзания для Static Text
		GetDlgItem(IDC_STATIC_P1)->SetWindowTextW(_T("Диаметр внутреннего кольца"));
		GetDlgItem(IDC_STATIC_P2)->SetWindowTextW(_T("Диаметр внешнего кольца"));

		// Видимость элементов
		//show
		GetDlgItem(IDC_BUTTON5)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_GROUP)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_GROUP)->ShowWindow(IDC_STATIC_ERROR);
		GetDlgItem(IDC_STATIC_P1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_P2)->ShowWindow(SW_SHOW);

		GetDlgItem(IDC_EDIT1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT2)->ShowWindow(SW_SHOW);
		//Hide
		/*GetDlgItem(IDC_BUTTON3)->ShowWindow(SW_HIDE);*/
		GetDlgItem(IDC_STATIC_P3)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_P4)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_P5)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_P6)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_P7)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_P8)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_P9)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_P10)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_EDIT3)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT4)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT5)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT6)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT7)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT8)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT9)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT10)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON2)->ShowWindow(SW_HIDE);
		// Знаачения по умолчанию

		// to string
		CString str;
		str.Format(_T("%g"), BearingIn);
		GetDlgItem(IDC_EDIT1)->SetWindowText(str);

		str.Format(_T("%g"), BearingOut);
		GetDlgItem(IDC_EDIT2)->SetWindowText(str);

		GetDlgItem(IDC_EDIT_NAME)->SetWindowText(_T("Подшипник")); //Название документа Inventor

		flag = 4;
		flagsave = 0;
	}

}


// Смена картинки для деталей
void CAssemblyConfiguratorView::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: добавьте свой код обработчика уведомлений

	int selectTab = m_tabs.GetCurSel();
	if (flag == -1)
	{
		if (selectTab == 0)
		{

			m_picture.SetBitmap(::LoadBitmap(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP11)));
		}
		if (selectTab == 1)
		{
			m_picture.SetBitmap(::LoadBitmap(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP12)));
		}
	}
	if (flag == 0)
	{
		if (selectTab == 0)
		{

			m_picture.SetBitmap(::LoadBitmap(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP1)));
		}
		if (selectTab == 1)
		{
			m_picture.SetBitmap(::LoadBitmap(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP2)));
		}
	}
	if (flag == 1)
	{
		if (selectTab == 0)
		{

			m_picture.SetBitmap(::LoadBitmap(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP3)));
		}
		if (selectTab == 1)
		{
			m_picture.SetBitmap(::LoadBitmap(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP4)));
		}
	}
	if (flag == 2)
	{
		if (selectTab == 0)
		{

			m_picture.SetBitmap(::LoadBitmap(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP5)));
		}
		if (selectTab == 1)
		{
			m_picture.SetBitmap(::LoadBitmap(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP6)));
		}
	}
	if (flag == 3)
	{
		if (selectTab == 0)
		{

			m_picture.SetBitmap(::LoadBitmap(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP7)));
		}
		if (selectTab == 1)
		{
			m_picture.SetBitmap(::LoadBitmap(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP8)));
		}
	}
	if (flag == 4)
	{
		if (selectTab == 0)
		{

			m_picture.SetBitmap(::LoadBitmap(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP9)));
		}
		if (selectTab == 1)
		{
			m_picture.SetBitmap(::LoadBitmap(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP10)));
		}
	}

	*pResult = 0;
}

//Кнопка построить
void CAssemblyConfiguratorView::OnBnClickedButton1()
{
	// TODO: добавьте свой код обработчика уведомлений

	BeginWaitCursor();

	UpdateData(true);

	if (flag == -1)
	{

		CLSID InvAppClsid;
		HRESULT hRes = CLSIDFromProgID(L"Inventor.Application", &InvAppClsid);

		//Проверка запущено ли приложение
		CComPtr<IUnknown> pInvAppUnk = nullptr;
		hRes = ::GetActiveObject(InvAppClsid, NULL, &pInvAppUnk);
		if (FAILED(hRes)) {
			// Если не запущено - запускаем
			AfxMessageBox(_T("Запустите Autodesk Inventor!"));
			return;
			/*GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);*/
		}

		if (bla == 0)
		{
			if (AfxMessageBox(_T("Выберите папку для сохранения!"), MB_OK | MB_ICONINFORMATION) == IDOK)
			{
				CAssemblyConfiguratorView::OnSaveFile();
			}
		}
		Shaft1Build(Shaft1_L1 / 10, Shaft1_L2 / 10, Shaft1_L3 / 10, Shaft1_D1 / 10, Shaft1_D2 / 10, Shaft1_D3 / 10, 1, buf);

		Shaft1Build(Shaft1_L1 / 10, Shaft1_L2 / 10, Shaft1_L3 / 10, Shaft1_D1 / 10, Shaft1_D2 / 10, Shaft1_D3 / 10, 2, buf);

		Shaft2Build(Shaft1_L1 / 10, Shaft1_L2 / 10, Shaft1_L3 / 10, Shaft1_D1 / 10, Shaft1_D2 / 10, Shaft1_D3 / 10, 1, buf);

		
		GearOne(PinionModule, PinionQuantity, Shaft1_D3 / 10, Shaft1_L3 / 10, 1, buf);

		GearOne(PinionModule, PinionQuantity, Shaft1_D3 / 10, Shaft1_L3 / 10, 2, buf);

		GearTwo(WheelModule, WheelRatio, WheelQuantity, Shaft2_D2 / 10, Shaft2_L3 / 10, 1, buf);

		GearTwo(WheelModule, WheelRatio, WheelQuantity, Shaft2_D2 / 10, Shaft2_L3 / 10, 2, buf);

		Bearing(BearingOut / 10, BearingIn / 10, Shaft1_D2 / 10, Shaft1_L2 / 10, 1, buf);

		Bearing(BearingOut / 10, BearingIn / 10, Shaft1_D2 / 10, Shaft1_L2 / 10, 2, buf);

		Bearing(BearingOut / 10, BearingIn / 10, Shaft1_D2 / 10, Shaft1_L2 / 10, 3, buf);

		Bearing(BearingOut / 10, BearingIn / 10, Shaft1_D2 / 10, Shaft1_L2 / 10, 4, buf);

		Bearing(BearingOut / 10, BearingIn / 10, Shaft1_D2 / 10, Shaft1_L2 / 10, 5, buf);

		Bearing(BearingOut / 10, BearingIn / 10, Shaft1_D2 / 10, Shaft1_L2 / 10, 6, buf);


		Assembly(buf);
		// Функции и путь к папке

	}
	if (flag == 0)
	{
		CLSID InvAppClsid;
		HRESULT hRes = CLSIDFromProgID(L"Inventor.Application", &InvAppClsid);

		//Проверка запущено ли приложение
		CComPtr<IUnknown> pInvAppUnk = nullptr;
		hRes = ::GetActiveObject(InvAppClsid, NULL, &pInvAppUnk);
		if (FAILED(hRes)) {
			// Если не запущено - запускаем
			AfxMessageBox(_T("Запустите Autodesk Inventor!"));
			return;
			/*GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);*/
		}

		if (bla == 0)
		{
			if (AfxMessageBox(_T("Выберите папку для сохранения!"), MB_OK | MB_ICONINFORMATION) == IDOK)
			{
				CAssemblyConfiguratorView::OnSaveFile();
			}
		}

		// Присваиваем новые(или нет) значения в глоб.переменные (см)
		Shaft1_L1 = m_Variable1;
		Shaft1_L2 = m_Variable2;
		Shaft1_L3 = m_Variable3;
		Shaft1_D1 = m_Variable4;
		Shaft1_D2 = m_Variable5;
		Shaft1_D3 = m_Variable6;


		Shaft1Build(Shaft1_L1 / 10, Shaft1_L2 / 10, Shaft1_L3 / 10, Shaft1_D1 / 10, Shaft1_D2 / 10, Shaft1_D3 / 10, 1, buf);
		flagsave = 1;
	}
	if (flag == 1)
	{
		CLSID InvAppClsid;
		HRESULT hRes = CLSIDFromProgID(L"Inventor.Application", &InvAppClsid);

		//Проверка запущено ли приложение
		CComPtr<IUnknown> pInvAppUnk = nullptr;
		hRes = ::GetActiveObject(InvAppClsid, NULL, &pInvAppUnk);
		if (FAILED(hRes)) {
			// Если не запущено - запускаем
			AfxMessageBox(_T("Запустите Autodesk Inventor!"));
			return;
			/*GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);*/
		}

		if (bla == 0)
		{
			if (AfxMessageBox(_T("Выберите папку для сохранения!"), MB_OK | MB_ICONINFORMATION) == IDOK)
			{
				CAssemblyConfiguratorView::OnSaveFile();
			}
		}

		// Присваиваем новые(или нет) значения в глоб.переменные (см)
		Shaft2_L1 = m_Variable1;
		Shaft2_L2 = m_Variable2;
		Shaft2_L3 = m_Variable3;
		Shaft2_D1 = m_Variable4;
		Shaft2_D2 = m_Variable5;
		Shaft2_D3 = m_Variable6;

		Shaft2Build(Shaft1_L1 / 10, Shaft1_L2 / 10, Shaft1_L3 / 10, Shaft1_D1 / 10, Shaft1_D2 / 10, Shaft1_D3 / 10, 1, buf);
		flagsave = 1;
	}
	if (flag == 2)
	{
		CLSID InvAppClsid;
		HRESULT hRes = CLSIDFromProgID(L"Inventor.Application", &InvAppClsid);

		//Проверка запущено ли приложение
		CComPtr<IUnknown> pInvAppUnk = nullptr;
		hRes = ::GetActiveObject(InvAppClsid, NULL, &pInvAppUnk);
		if (FAILED(hRes)) {
			// Если не запущено - запускаем
			AfxMessageBox(_T("Запустите Autodesk Inventor!"));
			return;
			/*GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);*/
		}

		if (bla == 0)
		{
			if (AfxMessageBox(_T("Выберите папку для сохранения!"), MB_OK | MB_ICONINFORMATION) == IDOK)
			{
				CAssemblyConfiguratorView::OnSaveFile();
			}
		}

		PinionModule = m_Variable1;
		PinionQuantity = m_Variable2;

		//Переводим в милиметры


		GearOne(PinionModule, PinionQuantity, Shaft1_D3 / 10, Shaft1_L3 / 10, 1, buf);
		flagsave = 1;
	}
	if (flag == 3)
	{
		CLSID InvAppClsid;
		HRESULT hRes = CLSIDFromProgID(L"Inventor.Application", &InvAppClsid);

		//Проверка запущено ли приложение
		CComPtr<IUnknown> pInvAppUnk = nullptr;
		hRes = ::GetActiveObject(InvAppClsid, NULL, &pInvAppUnk);
		if (FAILED(hRes)) {
			// Если не запущено - запускаем
			AfxMessageBox(_T("Запустите Autodesk Inventor!"));
			return;
			/*GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);*/
		}

		if (bla == 0)
		{
			if (AfxMessageBox(_T("Выберите папку для сохранения!"), MB_OK | MB_ICONINFORMATION) == IDOK)
			{
				CAssemblyConfiguratorView::OnSaveFile();
			}
		}

		WheelModule = m_Variable1;
		WheelRatio = m_Variable2;
		WheelQuantity = PinionQuantity * WheelRatio;

		//Переводим в милиметры


		GearTwo(WheelModule, WheelRatio, WheelQuantity, Shaft2_D2 / 10, Shaft2_L3 / 10, 1, buf);
		flagsave = 1;
	}
	if (flag == 4)
	{
		CLSID InvAppClsid;
		HRESULT hRes = CLSIDFromProgID(L"Inventor.Application", &InvAppClsid);

		//Проверка запущено ли приложение
		CComPtr<IUnknown> pInvAppUnk = nullptr;
		hRes = ::GetActiveObject(InvAppClsid, NULL, &pInvAppUnk);
		if (FAILED(hRes)) {
			// Если не запущено - запускаем
			AfxMessageBox(_T("Запустите Autodesk Inventor!"));
			return;
			/*GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);*/
		}

		if (bla == 0)
		{
			if (AfxMessageBox(_T("Выберите папку для сохранения!"), MB_OK | MB_ICONINFORMATION) == IDOK)
			{
				CAssemblyConfiguratorView::OnSaveFile();
			}
		}
		BearingOut = m_Variable1;
		BearingIn = m_Variable2;

		Bearing(BearingOut / 10, BearingIn / 10, Shaft1_D2 / 10, Shaft1_L2 / 10, 1, buf);
		flagsave = 1;
	}

}

//Ребра
void CAssemblyConfiguratorView::OnBnClickedButton2()
{
	// TODO: добавьте свой код обработчика уведомлений
	// TODO: добавьте свой код обработчика уведомлений
	SelectSet *pSelect;
	pPartDoc->get_SelectSet(&pSelect);


	if (pSelect->GetCount() > 0)
	{
		EdgePtr Seekedge = pSelect->GetItem(1);

		int seeknumber = -1;
		for (int i = 1; i <= pPartComDef->SurfaceBodies->GetCount(); i++)
		{
			SurfaceBody *SurfBody;
			SurfaceBodies *SurfBodies;

			pPartComDef->get_SurfaceBodies(&SurfBodies);

			SurfBodies->get_Item(i, &SurfBody);


			Edge *edge;
			Edges *edges;

			SurfBody->get_Edges(&edges);

			int N = SurfBody->Edges->GetCount();
			for (int j = 1; j <= SurfBody->Edges->GetCount(); j++)
			{
				edges->get_Item(j, &edge);

				if (Seekedge == edge)
				{
					seeknumber = j;
				}
			}

		}

	}
}

//Проверка состояния Inventor`a
void CAssemblyConfiguratorView::OnActivateView(BOOL bActivate, CView * pActivateView, CView * pDeactiveView)
{
	// TODO: добавьте сюда свой специальный код и / или вызовите базовый класс
	CFormView::OnActivateView(bActivate, pActivateView, pDeactiveView);



	//Поиск приложения по ID
	CLSID InvAppClsid;
	HRESULT hRes = CLSIDFromProgID(L"Inventor.Application", &InvAppClsid);

	//Проверка запущено ли приложение
	CComPtr<IUnknown> pInvAppUnk = nullptr;
	hRes = ::GetActiveObject(InvAppClsid, NULL, &pInvAppUnk);
	if (FAILED(hRes)) {
		// Если не запущено - запускаем
		SetDlgItemText(IDC_STATIC_COLOR, _T("Запустите Autodesk Inventor!"));
		/*GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);*/
	}
	else
	{
		SetDlgItemText(IDC_STATIC_COLOR, _T("Autodesk Inventor запущен!"));
	}


}

//Цвет Static Text
HBRUSH CAssemblyConfiguratorView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CFormView::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Измените любые атрибуты DC

	// TODO:  Вернуть другое значение дескриптора кисти, если оно не определено по умолчанию

	if (pWnd->GetDlgCtrlID() == IDC_STATIC_COLOR)
	{
		//Поиск приложения по ID
		CLSID InvAppClsid;
		HRESULT hRes = CLSIDFromProgID(L"Inventor.Application", &InvAppClsid);

		//Проверка запущено ли приложение
		CComPtr<IUnknown> pInvAppUnk = nullptr;
		hRes = ::GetActiveObject(InvAppClsid, NULL, &pInvAppUnk);
		if (FAILED(hRes)) {
			// Если не запущено - запускаем
			pDC->SetTextColor(RGB(255, 0, 0));
		}
		else
		{
			pDC->SetTextColor(RGB(0, 168, 17));;
		}
	}
	return hbr;
}

//Кнопка "Ззапустить Inventor"
void CAssemblyConfiguratorView::OnBnClickedButton3()
{
	// TODO: добавьте свой код обработчика уведомлений
	//Поиск приложения по ID
	UpdateData(true);
	m_progBarCtrl.SetPos(10);
	CLSID InvAppClsid;
	HRESULT hRes = CLSIDFromProgID(L"Inventor.Application", &InvAppClsid);
	if (FAILED(hRes)) {
		pInvApp = nullptr;
		return;
	}

	//Проверка запущено ли приложение
	CComPtr<IUnknown> pInvAppUnk = nullptr;
	hRes = ::GetActiveObject(InvAppClsid, NULL, &pInvAppUnk);
	if (FAILED(hRes)) {
		// Если не запущено - запускаем
		TRACE(L"Could not get hold of an active Inventor, will start a new session\n");
		hRes = CoCreateInstance(InvAppClsid, NULL, CLSCTX_LOCAL_SERVER, __uuidof(IUnknown), (void **)&pInvAppUnk);

		// Prorgress Bar
		m_progBarCtrl.SetPos(25);
		// Получаем указатель на запущенное приложение (hRes)
		// если происходит повторный запуск то указатель уже существует и с ним не надо ничего   делать
		if (!pInvApp)
			hRes = pInvAppUnk->QueryInterface(__uuidof(Application), (void **)&pInvApp);

		if (FAILED(hRes)) {
			return;
		}

		pInvApp->put_Visible(TRUE);

		// Prorgress Bar
		m_progBarCtrl.SetPos(40);
		pPartDoc = pInvApp->Documents->MethodAdd(kPartDocumentObject, pInvApp->FileManager->MethodGetTemplateFile(kPartDocumentObject, kMetricSystemOfMeasure, kGOST_DraftingStandard), true);

		char name[20];
		sprintf_s(name, "%S", m_CSName);
		pPartDoc->DisplayName = name;

		pPartDoc->get_ComponentDefinition(&pPartComDef);

		pInvApp->get_TransientGeometry(&pTransGeom);

		// Prorgress Bar
		m_progBarCtrl.SetPos(65);

		pTransManager = pInvApp->GetTransactionManager();

		Doc = CComQIPtr <Document>(pPartDoc);

		pTrans = pTransManager->MethodStartTransaction(Doc, _T(""));

		// Prorgress Bar
		m_progBarCtrl.SetPos(75);

		pPartComDef->get_Sketches(&sketches);
		pPartComDef->get_Sketches3D(&sketches3D);

		pPartComDef->get_WorkPlanes(&wp);
		pPartComDef->get_Features(&ft);
		pPartComDef->get_WorkAxes(&wax);
		pPartComDef->get_WorkPoints(&wpt);

		// Prorgress Bar
		m_progBarCtrl.SetPos(100);
		m_progBarCtrl.SetPos(0);

	}
	else
	{
		pInvApp = nullptr;
		m_progBarCtrl.SetPos(0);

		return;
	}
}

void CAssemblyConfiguratorView::OnSaveFile()
{
	// TODO: добавьте свой код обработчика команд
	UpdateData();



	BROWSEINFO bi;
	bi.hwndOwner = NULL;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = buf;
	bi.lpszTitle = _T("Выберите папку для сохранения");
	bi.ulFlags = NULL;
	bi.lpfn = NULL;

	ITEMIDLIST *itls;
	if ((itls = SHBrowseForFolder(&bi)) != NULL)
	{
		SHGetPathFromIDList(itls, buf);
	}

	bla = 1;



}

void CAssemblyConfiguratorView::Save()
{
	if (bla == 0)
	{
		if (AfxMessageBox(_T("Выберите папку для сохранения!"), MB_OK | MB_ICONINFORMATION) == IDOK)
		{
			CAssemblyConfiguratorView::OnSaveFile();
		}
	}
	CString strpath;
	CString str1 = _T("\\");
	CString str3 = _T(".ipt");
	CString str5(buf);
	strpath = str5 + str1 + m_CSName + str3;

	BSTR bstrpathName = strpath.AllocSysString();

	pPartDoc->MethodSaveAs(bstrpathName, true);

	pTrans = pTransManager->MethodEndTransaction();

	flagsave = 0;
}

void CAssemblyConfiguratorView::OnBnClickedButton5()
{
	UpdateData(true);
	if (flag == 0)
	{
		// Присваиваем новые(или нет) значения в глоб.переменные (см)
		Shaft1_L1 = m_Variable1;
		Shaft1_L2 = m_Variable2;
		Shaft1_L3 = m_Variable3;
		Shaft1_D1 = m_Variable4;
		Shaft1_D2 = m_Variable5;
		Shaft1_D3 = m_Variable6;

		/*flagsave = 1;*/
	}
	if (flag == 1)
	{
		// Присваиваем новые(или нет) значения в глоб.переменные (см)
		Shaft2_L1 = m_Variable1;
		Shaft2_L2 = m_Variable2;
		Shaft2_L3 = m_Variable3;
		Shaft2_D1 = m_Variable4;
		Shaft2_D2 = m_Variable5;
		Shaft2_D3 = m_Variable6;

		/*flagsave = 1;*/
	}
	if (flag == 2)
	{
		PinionModule = m_Variable1;
		PinionQuantity = m_Variable2;

		//Переводим в милиметры
		/*flagsave = 1;*/
	}
	if (flag == 3)
	{
		WheelModule = m_Variable1;
		WheelRatio = m_Variable2;
		WheelQuantity = PinionQuantity * WheelRatio;

		//Переводим в милиметры
		/*flagsave = 1;*/
	}
	if (flag == 4)
	{
		BearingOut = m_Variable1;
		BearingIn = m_Variable2;

		//flagsave = 1;
	}
}
