// DLGStartAssembly.cpp: файл реализации
//

#include "stdafx.h"
#include "AssemblyConfigurator.h"
#include "DLGStartAssembly.h"
#include "afxdialogex.h"
#define PROGRESS_TIMER  1000

// Диалоговое окно DLGStartAssembly
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

CComPtr<Application> pInvApp2;
InventorNative::PartDocumentPtr pPartDoc2;
InventorNative::PartComponentDefinition *pPartComDef2;
InventorNative::TransientGeometry *pTransGeom2;
InventorNative::Transaction *pTrans2;

TransactionManagerPtr pTransManager2;
Document *Doc2;

PlanarSketches *sketches2;
Sketches3D   *sketches3D2;
WorkPlanes *wp2;
PartFeatures *ft2;
WorkAxes *wax2;
WorkPoints *wpt2;


IMPLEMENT_DYNAMIC(DLGStartAssembly, CDialog)


void DLGStartAssembly::Shaft1Build(double m_lenght1, double m_lenght2, double m_lenght3, double m_diameter1, double m_diameter2, double m_diameter3, int save, int progress)
{
	//Поиск приложения по ID

	CLSID InvAppClsid;
	HRESULT hRes = CLSIDFromProgID(L"Inventor.Application", &InvAppClsid);
	if (FAILED(hRes)) {
		pInvApp2 = nullptr;
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
		if (!pInvApp2)
			hRes = pInvAppUnk->QueryInterface(__uuidof(Application), (void **)&pInvApp2);

		if (FAILED(hRes)) {
			return;
		}

		pInvApp2->put_Visible(TRUE);
		// Prorgess Bar

		pPartDoc2 = pInvApp2->Documents->MethodAdd(kPartDocumentObject, pInvApp2->FileManager->MethodGetTemplateFile(kPartDocumentObject, kMetricSystemOfMeasure, kGOST_DraftingStandard), true);

		pPartDoc2->DisplayName = _T("Деталь");

		pPartDoc2->get_ComponentDefinition(&pPartComDef2);

		pInvApp2->get_TransientGeometry(&pTransGeom2);
		// Prorgess Bar
		pTransManager2 = pInvApp2->GetTransactionManager();

		Doc2 = CComQIPtr <Document>(pPartDoc2);

		pTrans2 = pTransManager2->MethodStartTransaction(Doc2, _T(""));
		// Prorgess Bar

		pPartComDef2->get_Sketches(&sketches2);
		pPartComDef2->get_Sketches3D(&sketches3D2);

		pPartComDef2->get_WorkPlanes(&wp2);
		pPartComDef2->get_Features(&ft2);
		pPartComDef2->get_WorkAxes(&wax2);
		pPartComDef2->get_WorkPoints(&wpt2);
	}

	// Вращение

	PlanarSketch *pSketch1;
	sketches2->raw_Add(wp2->GetItem(3), false, &pSketch1);

	SketchPointPtr point[10];
	SketchLinePtr lines[10];

	SketchPoints *skPoints;
	SketchLines *skLines;
	Profiles *skProfiles;


	pSketch1->get_SketchPoints(&skPoints);
	pSketch1->get_SketchLines(&skLines);
	pSketch1->get_Profiles(&skProfiles);

	point[0] = skPoints->MethodAdd(pTransGeom2->MethodCreatePoint2d(0, 0), false);
	point[1] = skPoints->MethodAdd(pTransGeom2->MethodCreatePoint2d(m_diameter1 / 2, 0), false);
	point[2] = skPoints->MethodAdd(pTransGeom2->MethodCreatePoint2d(m_diameter1 / 2, m_lenght1), false);
	point[3] = skPoints->MethodAdd(pTransGeom2->MethodCreatePoint2d(m_diameter2 / 2, m_lenght1), false);
	point[4] = skPoints->MethodAdd(pTransGeom2->MethodCreatePoint2d(m_diameter2 / 2, m_lenght1 + m_lenght2), false);
	point[5] = skPoints->MethodAdd(pTransGeom2->MethodCreatePoint2d(m_diameter3 / 2, m_lenght1 + m_lenght2), false);
	point[6] = skPoints->MethodAdd(pTransGeom2->MethodCreatePoint2d(m_diameter3 / 2, m_lenght1 + m_lenght2 + m_lenght3), false);
	point[7] = skPoints->MethodAdd(pTransGeom2->MethodCreatePoint2d(m_diameter2 / 2, m_lenght1 + m_lenght2 + m_lenght3), false);
	point[8] = skPoints->MethodAdd(pTransGeom2->MethodCreatePoint2d(m_diameter2 / 2, m_lenght1 + (m_lenght2 * 2) + m_lenght3), false);
	point[9] = skPoints->MethodAdd(pTransGeom2->MethodCreatePoint2d(0, m_lenght1 + (m_lenght2 * 2) + m_lenght3), false);


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
	ft2->get_RevolveFeatures(&ftRevolve);

	RevolveFeaturePtr revolve = ftRevolve->MethodAddFull(pProfile, wax2->GetItem(2), kNewBodyOperation);
	// 1 паз на D1
	wp2->MethodAddByPlaneAndOffset(wp2->GetItem(3), m_diameter1 / 2, true);

	PlanarSketch *pSketch2;
	sketches2->raw_Add(wp2->GetItem(4), false, &pSketch2);

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

	point2[0] = skPoints2->MethodAdd(pTransGeom2->MethodCreatePoint2d(-0.5, ((m_lenght1 / 4) * 3) - 0.5), false);
	point2[1] = skPoints2->MethodAdd(pTransGeom2->MethodCreatePoint2d(-0.5, m_lenght1 / 4 + 0.5), false);
	point2[2] = skPoints2->MethodAdd(pTransGeom2->MethodCreatePoint2d(0.5, m_lenght1 / 4 + 0.5), false);
	point2[3] = skPoints2->MethodAdd(pTransGeom2->MethodCreatePoint2d(0.5, ((m_lenght1 / 4) * 3) - 0.5), false);

	lines2[0] = skLines2->MethodAddByTwoPoints(point2[0], point2[1]);
	lines2[1] = skLines2->MethodAddByTwoPoints(point2[2], point2[3]);

	arc[0] = skArc->MethodAddByThreePoints(point2[0], pTransGeom2->MethodCreatePoint2d(0, (m_lenght1 / 4) * 3), point2[3]);
	arc[1] = skArc->MethodAddByThreePoints(point2[1], pTransGeom2->MethodCreatePoint2d(0, m_lenght1 / 4), point2[2]);

	Profile *pProfile2;

	skProfiles2->raw__AddForSolid(&pProfile2);

	ExtrudeFeatures *ftExtrude;
	ft2->get_ExtrudeFeatures(&ftExtrude);

	ExtrudeFeaturePtr extrude = ftExtrude->MethodAddByDistanceExtent(pProfile2, 1, kNegativeExtentDirection, kCutOperation);
	// 2 паз на D2
	wp2->MethodAddByPlaneAndOffset(wp2->GetItem(3), -(m_diameter2 / 2), true);

	PlanarSketch *pSketch3;
	sketches2->raw_Add(wp2->GetItem(5), false, &pSketch3);

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
	point3[0] = skPoints3->MethodAdd(pTransGeom2->MethodCreatePoint2d(0.5, m_lenght1 + (((m_lenght2 / 4) * 3) - 0.5)), false);
	point3[1] = skPoints3->MethodAdd(pTransGeom2->MethodCreatePoint2d(0.5, m_lenght1 + (m_lenght2 / 4 + 0.5)), false);
	point3[2] = skPoints3->MethodAdd(pTransGeom2->MethodCreatePoint2d(-0.5, m_lenght1 + (m_lenght2 / 4 + 0.5)), false);
	point3[3] = skPoints3->MethodAdd(pTransGeom2->MethodCreatePoint2d(-0.5, m_lenght1 + (((m_lenght2 / 4) * 3) - 0.5)), false);

	lines3[0] = skLines3->MethodAddByTwoPoints(point3[0], point3[1]);
	lines3[1] = skLines3->MethodAddByTwoPoints(point3[2], point3[3]);

	arc2[0] = skArc2->MethodAddByThreePoints(point3[0], pTransGeom2->MethodCreatePoint2d(0, m_lenght1 + ((m_lenght2 / 4) * 3)), point3[3]);
	arc2[1] = skArc2->MethodAddByThreePoints(point3[1], pTransGeom2->MethodCreatePoint2d(0, m_lenght1 + (m_lenght2 / 4)), point3[2]);

	// 2 паз
	point3[4] = skPoints3->MethodAdd(pTransGeom2->MethodCreatePoint2d(0.5, m_lenght1 + m_lenght2 + m_lenght3 + (((m_lenght2 / 4) * 3) - 0.5)), false);
	point3[5] = skPoints3->MethodAdd(pTransGeom2->MethodCreatePoint2d(0.5, m_lenght1 + m_lenght2 + m_lenght3 + (m_lenght2 / 4 + 0.5)), false);
	point3[6] = skPoints3->MethodAdd(pTransGeom2->MethodCreatePoint2d(-0.5, m_lenght1 + m_lenght2 + m_lenght3 + (m_lenght2 / 4 + 0.5)), false);
	point3[7] = skPoints3->MethodAdd(pTransGeom2->MethodCreatePoint2d(-0.5, m_lenght1 + m_lenght2 + m_lenght3 + (((m_lenght2 / 4) * 3) - 0.5)), false);

	lines3[2] = skLines3->MethodAddByTwoPoints(point3[4], point3[5]);
	lines3[3] = skLines3->MethodAddByTwoPoints(point3[6], point3[7]);

	arc2[2] = skArc2->MethodAddByThreePoints(point3[4], pTransGeom2->MethodCreatePoint2d(0, m_lenght1 + m_lenght2 + m_lenght3 + ((m_lenght2 / 4) * 3)), point3[7]);
	arc2[3] = skArc2->MethodAddByThreePoints(point3[5], pTransGeom2->MethodCreatePoint2d(0, m_lenght1 + m_lenght2 + m_lenght3 + (m_lenght2 / 4)), point3[6]);

	Profile *pProfile3;

	skProfiles3->raw__AddForSolid(&pProfile3);

	ExtrudeFeatures *ftExtrude2;
	ft2->get_ExtrudeFeatures(&ftExtrude2);

	ExtrudeFeaturePtr extrude2 = ftExtrude2->MethodAddByDistanceExtent(pProfile3, 1, kPositiveExtentDirection, kCutOperation);
	// 3 паз на D3
	wp2->MethodAddByPlaneAndOffset(wp2->GetItem(3), m_diameter3 / 2, true);

	PlanarSketch *pSketch4;
	sketches2->raw_Add(wp2->GetItem(6), false, &pSketch4);

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

	point4[0] = skPoints4->MethodAdd(pTransGeom2->MethodCreatePoint2d(-0.5, m_lenght1 + m_lenght2 + (((m_lenght3 / 4) * 3) - 0.5)), false);
	point4[1] = skPoints4->MethodAdd(pTransGeom2->MethodCreatePoint2d(-0.5, m_lenght1 + m_lenght2 + (m_lenght3 / 4 + 0.5)), false);
	point4[2] = skPoints4->MethodAdd(pTransGeom2->MethodCreatePoint2d(0.5, m_lenght1 + m_lenght2 + (m_lenght3 / 4 + 0.5)), false);
	point4[3] = skPoints4->MethodAdd(pTransGeom2->MethodCreatePoint2d(0.5, m_lenght1 + m_lenght2 + (((m_lenght3 / 4) * 3) - 0.5)), false);

	lines4[0] = skLines4->MethodAddByTwoPoints(point4[0], point4[1]);
	lines4[1] = skLines4->MethodAddByTwoPoints(point4[2], point4[3]);

	arc3[0] = skArc3->MethodAddByThreePoints(point4[0], pTransGeom2->MethodCreatePoint2d(0, m_lenght1 + m_lenght2 + ((m_lenght3 / 4) * 3)), point4[3]);
	arc3[1] = skArc3->MethodAddByThreePoints(point4[1], pTransGeom2->MethodCreatePoint2d(0, m_lenght1 + m_lenght2 + (m_lenght3 / 4)), point4[2]);

	Profile *pProfile4;

	skProfiles4->raw__AddForSolid(&pProfile4);

	ExtrudeFeatures *ftExtrude3;
	ft2->get_ExtrudeFeatures(&ftExtrude3);

	ExtrudeFeaturePtr extrude3 = ftExtrude3->MethodAddByDistanceExtent(pProfile4, 1, kNegativeExtentDirection, kCutOperation);
	// Фаска
	ChamferFeatures *pChamferFt;
	ft2->get_ChamferFeatures(&pChamferFt);

	EdgeCollection *edgeColl;

	pInvApp2->TransientObjects->raw_CreateEdgeCollection(vtMissing, &edgeColl);

	SurfaceBody *SurfBody;
	SurfaceBodies *SurfBodies;

	pPartComDef2->get_SurfaceBodies(&SurfBodies);

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
	if (save == 1)
	{
		pPartDoc2->MethodSaveAs("D:\\sborka\\Вал 1.ipt", true);

		pTrans2 = pTransManager2->MethodEndTransaction();
	}
	if (save == 2)
	{
		pPartDoc2->MethodSaveAs("D:\\sborka\\Вал 2.ipt", true);

		pTrans2 = pTransManager2->MethodEndTransaction();
	}



}


void DLGStartAssembly::Shaft2Build(double m_lenght1, double m_lenght2, double m_lenght3, double m_diameter1, double m_diameter2, double m_diameter3, int progress)
{

	//Поиск приложения по ID

	CLSID InvAppClsid;
	HRESULT hRes = CLSIDFromProgID(L"Inventor.Application", &InvAppClsid);
	if (FAILED(hRes)) {
		pInvApp2 = nullptr;
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
		if (!pInvApp2)
			hRes = pInvAppUnk->QueryInterface(__uuidof(Application), (void **)&pInvApp2);

		if (FAILED(hRes)) {
			return;
		}

		pInvApp2->put_Visible(TRUE);
		// Prorgess Bar


		pPartDoc2 = pInvApp2->Documents->MethodAdd(kPartDocumentObject, pInvApp2->FileManager->MethodGetTemplateFile(kPartDocumentObject, kMetricSystemOfMeasure, kGOST_DraftingStandard), true);




		pPartDoc2->DisplayName = _T("Деталь");

		pPartDoc2->get_ComponentDefinition(&pPartComDef2);

		pInvApp2->get_TransientGeometry(&pTransGeom2);
		// Prorgess Bar


		pTransManager2 = pInvApp2->GetTransactionManager();

		Doc2 = CComQIPtr <Document>(pPartDoc2);

		pTrans2 = pTransManager2->MethodStartTransaction(Doc2, _T(""));
		// Prorgess Bar


		pPartComDef2->get_Sketches(&sketches2);
		pPartComDef2->get_Sketches3D(&sketches3D2);

		pPartComDef2->get_WorkPlanes(&wp2);
		pPartComDef2->get_Features(&ft2);
		pPartComDef2->get_WorkAxes(&wax2);
		pPartComDef2->get_WorkPoints(&wpt2);
	}

	// Вращение


	PlanarSketch *pSketch1;
	sketches2->raw_Add(wp2->GetItem(3), false, &pSketch1);

	SketchPointPtr point[20];
	SketchLinePtr lines[20];

	SketchPoints *skPoints;
	SketchLines *skLines;
	Profiles *skProfiles;


	pSketch1->get_SketchPoints(&skPoints);
	pSketch1->get_SketchLines(&skLines);
	pSketch1->get_Profiles(&skProfiles);

	point[0] = skPoints->MethodAdd(pTransGeom2->MethodCreatePoint2d(0, 0), false);
	point[1] = skPoints->MethodAdd(pTransGeom2->MethodCreatePoint2d(m_diameter1 / 2, 0), false);
	point[2] = skPoints->MethodAdd(pTransGeom2->MethodCreatePoint2d(m_diameter1 / 2, 0.5), false);
	point[3] = skPoints->MethodAdd(pTransGeom2->MethodCreatePoint2d((m_diameter1 / 2) - 0.5, 0.5), false);
	point[4] = skPoints->MethodAdd(pTransGeom2->MethodCreatePoint2d((m_diameter1 / 2) - 0.5, 1), false);
	point[5] = skPoints->MethodAdd(pTransGeom2->MethodCreatePoint2d(m_diameter1 / 2, 1), false);
	point[6] = skPoints->MethodAdd(pTransGeom2->MethodCreatePoint2d(m_diameter1 / 2, m_lenght1), false);
	point[7] = skPoints->MethodAdd(pTransGeom2->MethodCreatePoint2d(m_diameter2 / 2, m_lenght1), false);
	point[8] = skPoints->MethodAdd(pTransGeom2->MethodCreatePoint2d(m_diameter2 / 2, m_lenght1 + m_lenght3), false);
	point[9] = skPoints->MethodAdd(pTransGeom2->MethodCreatePoint2d(m_diameter3 / 2, m_lenght1 + m_lenght3), false);
	point[10] = skPoints->MethodAdd(pTransGeom2->MethodCreatePoint2d(m_diameter3 / 2, m_lenght1 + m_lenght3 + m_lenght2), false);
	point[11] = skPoints->MethodAdd(pTransGeom2->MethodCreatePoint2d(m_diameter2 / 2, m_lenght1 + m_lenght3 + m_lenght2), false);
	point[12] = skPoints->MethodAdd(pTransGeom2->MethodCreatePoint2d(m_diameter2 / 2, m_lenght1 + (m_lenght3 * 2) + m_lenght2), false);
	point[13] = skPoints->MethodAdd(pTransGeom2->MethodCreatePoint2d(m_diameter1 / 2, m_lenght1 + (m_lenght3 * 2) + m_lenght2), false);
	point[14] = skPoints->MethodAdd(pTransGeom2->MethodCreatePoint2d(m_diameter1 / 2, (m_lenght1 * 2) + (m_lenght3 * 2) + m_lenght2 - 1), false);
	point[15] = skPoints->MethodAdd(pTransGeom2->MethodCreatePoint2d((m_diameter1 / 2) - 0.5, (m_lenght1 * 2) + (m_lenght3 * 2) + m_lenght2 - 1), false);
	point[16] = skPoints->MethodAdd(pTransGeom2->MethodCreatePoint2d((m_diameter1 / 2) - 0.5, (m_lenght1 * 2) + (m_lenght3 * 2) + m_lenght2 - 0.5), false);
	point[17] = skPoints->MethodAdd(pTransGeom2->MethodCreatePoint2d(m_diameter1 / 2, (m_lenght1 * 2) + (m_lenght3 * 2) + m_lenght2 - 0.5), false);
	point[18] = skPoints->MethodAdd(pTransGeom2->MethodCreatePoint2d(m_diameter1 / 2, (m_lenght1 * 2) + (m_lenght3 * 2) + m_lenght2), false);
	point[19] = skPoints->MethodAdd(pTransGeom2->MethodCreatePoint2d(0, (m_lenght1 * 2) + (m_lenght3 * 2) + m_lenght2), false);

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
	ft2->get_RevolveFeatures(&ftRevolve);

	RevolveFeaturePtr revolve = ftRevolve->MethodAddFull(pProfile, wax2->GetItem(2), kNewBodyOperation);


	//1 пазы на D2
	wp2->MethodAddByPlaneAndOffset(wp2->GetItem(3), m_diameter2 / 2, true);

	PlanarSketch *pSketch2;
	sketches2->raw_Add(wp2->GetItem(4), false, &pSketch2);

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
	point2[0] = skPoints2->MethodAdd(pTransGeom2->MethodCreatePoint2d(-0.5, m_lenght1 + 1.5), false);
	point2[1] = skPoints2->MethodAdd(pTransGeom2->MethodCreatePoint2d(-0.5, m_lenght1 + m_lenght3 - 1.5), false);
	point2[2] = skPoints2->MethodAdd(pTransGeom2->MethodCreatePoint2d(0.5, m_lenght1 + 1.5), false);
	point2[3] = skPoints2->MethodAdd(pTransGeom2->MethodCreatePoint2d(0.5, m_lenght1 + m_lenght3 - 1.5), false);

	lines2[0] = skLines2->MethodAddByTwoPoints(point2[0], point2[1]);
	lines2[1] = skLines2->MethodAddByTwoPoints(point2[2], point2[3]);

	arc[0] = skArc->MethodAddByThreePoints(point2[0], pTransGeom2->MethodCreatePoint2d(0, m_lenght1 + 1), point2[2]);
	arc[1] = skArc->MethodAddByThreePoints(point2[1], pTransGeom2->MethodCreatePoint2d(0, m_lenght1 + m_lenght3 - 1), point2[3]);

	// 2 паз
	point2[4] = skPoints2->MethodAdd(pTransGeom2->MethodCreatePoint2d(-0.5, m_lenght1 + m_lenght2 + m_lenght3 + 1.5), false);
	point2[5] = skPoints2->MethodAdd(pTransGeom2->MethodCreatePoint2d(-0.5, m_lenght1 + m_lenght2 + (m_lenght3 * 2) - 1.5), false);
	point2[6] = skPoints2->MethodAdd(pTransGeom2->MethodCreatePoint2d(0.5, m_lenght1 + m_lenght2 + m_lenght3 + 1.5), false);
	point2[7] = skPoints2->MethodAdd(pTransGeom2->MethodCreatePoint2d(0.5, m_lenght1 + m_lenght2 + (m_lenght3 * 2) - 1.5), false);

	lines2[2] = skLines2->MethodAddByTwoPoints(point2[4], point2[5]);
	lines2[3] = skLines2->MethodAddByTwoPoints(point2[6], point2[7]);

	arc[2] = skArc->MethodAddByThreePoints(point2[4], pTransGeom2->MethodCreatePoint2d(0, m_lenght1 + m_lenght2 + m_lenght3 + 1), point2[6]);
	arc[3] = skArc->MethodAddByThreePoints(point2[5], pTransGeom2->MethodCreatePoint2d(0, m_lenght1 + m_lenght2 + (m_lenght3 * 2) - 1), point2[7]);


	Profile *pProfile2;

	skProfiles2->raw__AddForSolid(&pProfile2);

	ExtrudeFeatures *ftExtrude;
	ft2->get_ExtrudeFeatures(&ftExtrude);

	ExtrudeFeaturePtr extrude = ftExtrude->MethodAddByDistanceExtent(pProfile2, 1, kNegativeExtentDirection, kCutOperation);

	// 2 паз на D1
	wp2->MethodAddByPlaneAndOffset(wp2->GetItem(3), -(m_diameter1 / 2), true);

	PlanarSketch *pSketch3;
	sketches2->raw_Add(wp2->GetItem(5), false, &pSketch3);

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
	point3[0] = skPoints3->MethodAdd(pTransGeom2->MethodCreatePoint2d(0.5, m_lenght1 + 1.5), false);
	point3[1] = skPoints3->MethodAdd(pTransGeom2->MethodCreatePoint2d(0.5, m_lenght1 + m_lenght2 - 1), false);
	point3[2] = skPoints3->MethodAdd(pTransGeom2->MethodCreatePoint2d(-0.5, m_lenght1 + 1.5), false);
	point3[3] = skPoints3->MethodAdd(pTransGeom2->MethodCreatePoint2d(-0.5, m_lenght1 + m_lenght2 - 1), false);

	lines3[0] = skLines3->MethodAddByTwoPoints(point3[0], point3[1]);
	lines3[1] = skLines3->MethodAddByTwoPoints(point3[2], point3[3]);

	arc2[0] = skArc2->MethodAddByThreePoints(point3[0], pTransGeom2->MethodCreatePoint2d(0, m_lenght1 + 1), point3[2]);
	arc2[1] = skArc2->MethodAddByThreePoints(point3[1], pTransGeom2->MethodCreatePoint2d(0, m_lenght1 + m_lenght2 - 0.5), point3[3]);

	// 2 паз
	point3[4] = skPoints3->MethodAdd(pTransGeom2->MethodCreatePoint2d(0.5, 1 + ((m_lenght1 - 1) / 4) + 0.5), false);
	point3[5] = skPoints3->MethodAdd(pTransGeom2->MethodCreatePoint2d(0.5, 1 + (((m_lenght1 - 1) / 4) * 3) - 0.5), false);
	point3[6] = skPoints3->MethodAdd(pTransGeom2->MethodCreatePoint2d(-0.5, 1 + ((m_lenght1 - 1) / 4) + 0.5), false);
	point3[7] = skPoints3->MethodAdd(pTransGeom2->MethodCreatePoint2d(-0.5, 1 + (((m_lenght1 - 1) / 4) * 3) - 0.5), false);

	lines3[2] = skLines3->MethodAddByTwoPoints(point3[4], point3[5]);
	lines3[3] = skLines3->MethodAddByTwoPoints(point3[6], point3[7]);

	arc2[2] = skArc2->MethodAddByThreePoints(point3[4], pTransGeom2->MethodCreatePoint2d(0, 1 + ((m_lenght1 - 1) / 4)), point3[6]);
	arc2[3] = skArc2->MethodAddByThreePoints(point3[5], pTransGeom2->MethodCreatePoint2d(0, 1 + (((m_lenght1 - 1) / 4) * 3)), point3[7]);

	Profile *pProfile3;

	skProfiles3->raw__AddForSolid(&pProfile3);

	ExtrudeFeatures *ftExtrude2;
	ft2->get_ExtrudeFeatures(&ftExtrude2);

	ExtrudeFeaturePtr extrude2 = ftExtrude2->MethodAddByDistanceExtent(pProfile3, 1, kPositiveExtentDirection, kCutOperation);


	//Зеркальное отражение

	ObjectCollection *pCollection;

	pInvApp2->TransientObjects->raw_CreateObjectCollection(vtMissing, &pCollection);

	pCollection->MethodAdd(extrude2);

	MirrorFeatures *pMirrorFeat;

	ft2->get_MirrorFeatures(&pMirrorFeat);
	wp2->MethodAddByPlaneAndOffset(wp2->GetItem(2), m_lenght1 + m_lenght3 + (m_lenght2 / 2), true);


	MirrorFeature *Mirrir1 = pMirrorFeat->MethodAdd(pCollection, wp2->GetItem(6), true, kIdenticalCompute);

	// Фаска
	ChamferFeatures *pChamferFt;
	ft2->get_ChamferFeatures(&pChamferFt);

	EdgeCollection *edgeColl;

	pInvApp2->TransientObjects->raw_CreateEdgeCollection(vtMissing, &edgeColl);

	SurfaceBody *SurfBody;
	SurfaceBodies *SurfBodies;

	pPartComDef2->get_SurfaceBodies(&SurfBodies);

	SurfBodies->get_Item(1, &SurfBody);

	Edges *edges;

	SurfBody->get_Edges(&edges);

	Edge *ed;
	edges->get_Item(80, &ed);
	edgeColl->MethodAdd(ed);
	edges->get_Item(78, &ed);
	edgeColl->MethodAdd(ed);
	edges->get_Item(77, &ed);
	edgeColl->MethodAdd(ed);
	edges->get_Item(75, &ed);
	edgeColl->MethodAdd(ed);

	ChamferFeaturePtr chamFeature = pChamferFt->MethodAddUsingDistance(edgeColl, 0.5f, false, false, false);


	pPartDoc2->MethodSaveAs("D:\\sborka\\Вал 3.ipt", true);

	pTrans2 = pTransManager2->MethodEndTransaction();

}

void DLGStartAssembly::GearOne(double modul, double number, double diameter, double lenght3, int save, int progress)
{
	//Поиск приложения по ID

	CLSID InvAppClsid;
	HRESULT hRes = CLSIDFromProgID(L"Inventor.Application", &InvAppClsid);
	if (FAILED(hRes)) {
		pInvApp2 = nullptr;
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
		if (!pInvApp2)
			hRes = pInvAppUnk->QueryInterface(__uuidof(Application), (void **)&pInvApp2);

		if (FAILED(hRes)) {
			return;
		}

		pInvApp2->put_Visible(TRUE);
		// Prorgess Bar


		pPartDoc2 = pInvApp2->Documents->MethodAdd(kPartDocumentObject, pInvApp2->FileManager->MethodGetTemplateFile(kPartDocumentObject, kMetricSystemOfMeasure, kGOST_DraftingStandard), true);




		pPartDoc2->DisplayName = _T("Деталь");

		pPartDoc2->get_ComponentDefinition(&pPartComDef2);

		pInvApp2->get_TransientGeometry(&pTransGeom2);
		// Prorgess Bar


		pTransManager2 = pInvApp2->GetTransactionManager();

		Doc2 = CComQIPtr <Document>(pPartDoc2);

		pTrans2 = pTransManager2->MethodStartTransaction(Doc2, _T(""));
		// Prorgess Bar


		pPartComDef2->get_Sketches(&sketches2);
		pPartComDef2->get_Sketches3D(&sketches3D2);

		pPartComDef2->get_WorkPlanes(&wp2);
		pPartComDef2->get_Features(&ft2);
		pPartComDef2->get_WorkAxes(&wax2);
		pPartComDef2->get_WorkPoints(&wpt2);
	}

	// Вращение


	PlanarSketch *pSketch1;
	sketches2->raw_Add(wp2->GetItem(3), false, &pSketch1);

	SketchPointPtr point[20];
	SketchLinePtr lines[20];

	SketchPoints *skPoints;
	SketchLines *skLines;
	Profiles *skProfiles;


	pSketch1->get_SketchPoints(&skPoints);
	pSketch1->get_SketchLines(&skLines);
	pSketch1->get_Profiles(&skProfiles);

	double dimetrY = (modul * number) / 10;

	point[0] = skPoints->MethodAdd(pTransGeom2->MethodCreatePoint2d((diameter / 2) + 0.1, 0), false);
	point[1] = skPoints->MethodAdd(pTransGeom2->MethodCreatePoint2d((dimetrY + ((modul * 2) / 10)) / 2, 0), false);
	point[2] = skPoints->MethodAdd(pTransGeom2->MethodCreatePoint2d((dimetrY + ((modul * 2) / 10)) / 2, lenght3 / 1.8), false);
	point[3] = skPoints->MethodAdd(pTransGeom2->MethodCreatePoint2d((diameter / 2) + 0.1, lenght3 / 1.8), false);

	lines[0] = skLines->MethodAddByTwoPoints(point[0], point[1]);
	lines[1] = skLines->MethodAddByTwoPoints(point[1], point[2]);
	lines[2] = skLines->MethodAddByTwoPoints(point[2], point[3]);
	lines[3] = skLines->MethodAddByTwoPoints(point[3], point[0]);

	Profile *pProfile;
	skProfiles->raw__AddForSolid(&pProfile);

	RevolveFeatures *ftRevolve;
	ft2->get_RevolveFeatures(&ftRevolve);

	RevolveFeaturePtr revolve = ftRevolve->MethodAddFull(pProfile, wax2->GetItem(2), kNewBodyOperation);

	double step = 3.14 * modul;
	double thickness = step * 0.5;
	double down = (thickness*0.5) / 10;
	double up = (1.75*thickness) / 10;

	double r = (dimetrY + ((modul * 2) / 10)) / 2;

	double rblue = (dimetrY - ((modul * 2.25) / 10)) / 2;

	double Yup = sqrt(pow(r, 2) - pow(up / 2, 2));

	double Ydown = sqrt(pow(rblue, 2) - pow(up / 2, 2));



	PlanarSketch *pSketch2;
	sketches2->raw_Add(wp2->GetItem(2), false, &pSketch2);

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

	point2[0] = skPoints2->MethodAdd(pTransGeom2->MethodCreatePoint2d(up / 2, Yup), false);
	point2[1] = skPoints2->MethodAdd(pTransGeom2->MethodCreatePoint2d(-up / 2, Yup), false);
	point2[2] = skPoints2->MethodAdd(pTransGeom2->MethodCreatePoint2d(down / 2, Ydown), false);
	point2[3] = skPoints2->MethodAdd(pTransGeom2->MethodCreatePoint2d(-down / 2, Ydown), false);

	lines2[0] = skLines2->MethodAddByTwoPoints(point2[0], point2[2]);
	lines2[1] = skLines2->MethodAddByTwoPoints(point2[1], point2[3]);


	arc2[0] = skArc2->MethodAddByThreePoints(point2[0], pTransGeom2->MethodCreatePoint2d(0, r), point2[1]);
	arc2[1] = skArc2->MethodAddByThreePoints(point2[2], pTransGeom2->MethodCreatePoint2d(0, rblue), point2[3]);


	Profile *pProfile2;
	skProfiles2->raw__AddForSolid(&pProfile2);

	ExtrudeFeatures *ftExtrude;
	ft2->get_ExtrudeFeatures(&ftExtrude);

	ExtrudeFeaturePtr extrude = ftExtrude->MethodAddByDistanceExtent(pProfile2, lenght3 / 1.8, kPositiveExtentDirection, kCutOperation);


	ObjectCollection *pCollection;

	pInvApp2->TransientObjects->raw_CreateObjectCollection(vtMissing, &pCollection);

	pCollection->MethodAdd(extrude);

	CircularPatternFeatures *pCircPatFeat;

	ft2->get_CircularPatternFeatures(&pCircPatFeat);

	CircularPatternFeature *circFeat = pCircPatFeat->MethodAdd(pCollection, wax2->GetItem(2), true, number, "360 град", true, kIdenticalCompute);

	// вырез
	PlanarSketch *pSketch3;
	sketches2->raw_Add(wp2->GetItem(3), false, &pSketch3);

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

	point3[0] = skPoints3->MethodAdd(pTransGeom2->MethodCreatePoint2d(0.5, bla - (lenght3 / 4) + 0.5), false);
	point3[1] = skPoints3->MethodAdd(pTransGeom2->MethodCreatePoint2d(0.5, bla + (lenght3 / 4) - 0.5), false);
	point3[2] = skPoints3->MethodAdd(pTransGeom2->MethodCreatePoint2d(-0.5, bla - (lenght3 / 4) + 0.5), false);
	point3[3] = skPoints3->MethodAdd(pTransGeom2->MethodCreatePoint2d(-0.5, bla + (lenght3 / 4) - 0.5), false);

	lines3[0] = skLines3->MethodAddByTwoPoints(point3[0], point3[1]);
	lines3[1] = skLines3->MethodAddByTwoPoints(point3[2], point3[3]);

	arc3[0] = skArc3->MethodAddByThreePoints(point3[0], pTransGeom2->MethodCreatePoint2d(0, bla - (lenght3 / 4)), point3[2]);
	arc3[1] = skArc3->MethodAddByThreePoints(point3[1], pTransGeom2->MethodCreatePoint2d(0, bla + (lenght3 / 4)), point3[3]);

	Profile *pProfile4;

	skProfiles3->raw__AddForSolid(&pProfile4);

	ExtrudeFeatures *ftExtrude3;
	ft2->get_ExtrudeFeatures(&ftExtrude3);

	ExtrudeFeaturePtr extrude3 = ftExtrude3->MethodAddByDistanceExtent(pProfile4, (lenght3 / 2) + 1, kPositiveExtentDirection, kCutOperation);

	if (save == 1)
	{
		pPartDoc2->MethodSaveAs("D:\\sborka\\Шестерня 1.ipt", true);

		pTrans2 = pTransManager2->MethodEndTransaction();
	}
	if (save == 2)
	{
		pPartDoc2->MethodSaveAs("D:\\sborka\\Шестерня 2.ipt", true);

		pTrans2 = pTransManager2->MethodEndTransaction();
	}

}

void DLGStartAssembly::GearTwo(double modul, double ratio, double number, double diameter, double lenght3, int save, int progress)
{
	//Поиск приложения по ID

	CLSID InvAppClsid;
	HRESULT hRes = CLSIDFromProgID(L"Inventor.Application", &InvAppClsid);
	if (FAILED(hRes)) {
		pInvApp2 = nullptr;
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
		if (!pInvApp2)
			hRes = pInvAppUnk->QueryInterface(__uuidof(Application), (void **)&pInvApp2);

		if (FAILED(hRes)) {
			return;
		}

		pInvApp2->put_Visible(TRUE);
		// Prorgess Bar


		pPartDoc2 = pInvApp2->Documents->MethodAdd(kPartDocumentObject, pInvApp2->FileManager->MethodGetTemplateFile(kPartDocumentObject, kMetricSystemOfMeasure, kGOST_DraftingStandard), true);




		pPartDoc2->DisplayName = _T("Деталь");

		pPartDoc2->get_ComponentDefinition(&pPartComDef2);

		pInvApp2->get_TransientGeometry(&pTransGeom2);
		// Prorgess Bar


		pTransManager2 = pInvApp2->GetTransactionManager();

		Doc2 = CComQIPtr <Document>(pPartDoc2);

		pTrans2 = pTransManager2->MethodStartTransaction(Doc2, _T(""));
		// Prorgess Bar


		pPartComDef2->get_Sketches(&sketches2);
		pPartComDef2->get_Sketches3D(&sketches3D2);

		pPartComDef2->get_WorkPlanes(&wp2);
		pPartComDef2->get_Features(&ft2);
		pPartComDef2->get_WorkAxes(&wax2);
		pPartComDef2->get_WorkPoints(&wpt2);
	}

	// Вращение


	PlanarSketch *pSketch1;
	sketches2->raw_Add(wp2->GetItem(3), false, &pSketch1);

	SketchPointPtr point[20];
	SketchLinePtr lines[20];

	SketchPoints *skPoints;
	SketchLines *skLines;
	Profiles *skProfiles;


	pSketch1->get_SketchPoints(&skPoints);
	pSketch1->get_SketchLines(&skLines);
	pSketch1->get_Profiles(&skProfiles);

	double dimetrY = (modul * number) / 10;

	point[0] = skPoints->MethodAdd(pTransGeom2->MethodCreatePoint2d((diameter / 2) + 0.1, 0), false);
	point[1] = skPoints->MethodAdd(pTransGeom2->MethodCreatePoint2d((dimetrY + ((modul * 2) / 10)) / 2, 0), false);
	point[2] = skPoints->MethodAdd(pTransGeom2->MethodCreatePoint2d((dimetrY + ((modul * 2) / 10)) / 2, lenght3 / 1.8), false);
	point[3] = skPoints->MethodAdd(pTransGeom2->MethodCreatePoint2d((diameter / 2) + 0.1, lenght3 / 1.8), false);

	lines[0] = skLines->MethodAddByTwoPoints(point[0], point[1]);
	lines[1] = skLines->MethodAddByTwoPoints(point[1], point[2]);
	lines[2] = skLines->MethodAddByTwoPoints(point[2], point[3]);
	lines[3] = skLines->MethodAddByTwoPoints(point[3], point[0]);

	Profile *pProfile;
	skProfiles->raw__AddForSolid(&pProfile);

	RevolveFeatures *ftRevolve;
	ft2->get_RevolveFeatures(&ftRevolve);

	RevolveFeaturePtr revolve = ftRevolve->MethodAddFull(pProfile, wax2->GetItem(2), kNewBodyOperation);

	double step = 3.14 * modul;
	double thickness = step * 0.5;
	double down = (thickness*0.5) / 10;
	double up = (1.75*thickness) / 10;

	double r = (dimetrY + ((modul * 2) / 10)) / 2;

	double rblue = (dimetrY - ((modul * 2.25) / 10)) / 2;

	double Yup = sqrt(pow(r, 2) - pow(up / 2, 2));

	double Ydown = sqrt(pow(rblue, 2) - pow(up / 2, 2));



	PlanarSketch *pSketch2;
	sketches2->raw_Add(wp2->GetItem(2), false, &pSketch2);

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

	point2[0] = skPoints2->MethodAdd(pTransGeom2->MethodCreatePoint2d(up / 2, Yup), false);
	point2[1] = skPoints2->MethodAdd(pTransGeom2->MethodCreatePoint2d(-up / 2, Yup), false);
	point2[2] = skPoints2->MethodAdd(pTransGeom2->MethodCreatePoint2d(down / 2, Ydown), false);
	point2[3] = skPoints2->MethodAdd(pTransGeom2->MethodCreatePoint2d(-down / 2, Ydown), false);

	lines2[0] = skLines2->MethodAddByTwoPoints(point2[0], point2[2]);
	lines2[1] = skLines2->MethodAddByTwoPoints(point2[1], point2[3]);


	arc2[0] = skArc2->MethodAddByThreePoints(point2[0], pTransGeom2->MethodCreatePoint2d(0, r), point2[1]);
	arc2[1] = skArc2->MethodAddByThreePoints(point2[2], pTransGeom2->MethodCreatePoint2d(0, rblue), point2[3]);


	Profile *pProfile2;
	skProfiles2->raw__AddForSolid(&pProfile2);

	ExtrudeFeatures *ftExtrude;
	ft2->get_ExtrudeFeatures(&ftExtrude);

	ExtrudeFeaturePtr extrude = ftExtrude->MethodAddByDistanceExtent(pProfile2, lenght3 / 1.8, kPositiveExtentDirection, kCutOperation);


	ObjectCollection *pCollection;

	pInvApp2->TransientObjects->raw_CreateObjectCollection(vtMissing, &pCollection);

	pCollection->MethodAdd(extrude);

	CircularPatternFeatures *pCircPatFeat;

	ft2->get_CircularPatternFeatures(&pCircPatFeat);

	CircularPatternFeature *circFeat = pCircPatFeat->MethodAdd(pCollection, wax2->GetItem(2), true, number, "360 град", true, kIdenticalCompute);

	// вырез
	PlanarSketch *pSketch3;
	sketches2->raw_Add(wp2->GetItem(3), false, &pSketch3);

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

	point3[0] = skPoints3->MethodAdd(pTransGeom2->MethodCreatePoint2d(0.5, bla - (lenght3 / 4) + 0.5), false);
	point3[1] = skPoints3->MethodAdd(pTransGeom2->MethodCreatePoint2d(0.5, bla + (lenght3 / 4) - 0.5), false);
	point3[2] = skPoints3->MethodAdd(pTransGeom2->MethodCreatePoint2d(-0.5, bla - (lenght3 / 4) + 0.5), false);
	point3[3] = skPoints3->MethodAdd(pTransGeom2->MethodCreatePoint2d(-0.5, bla + (lenght3 / 4) - 0.5), false);

	lines3[0] = skLines3->MethodAddByTwoPoints(point3[0], point3[1]);
	lines3[1] = skLines3->MethodAddByTwoPoints(point3[2], point3[3]);

	arc3[0] = skArc3->MethodAddByThreePoints(point3[0], pTransGeom2->MethodCreatePoint2d(0, bla - (lenght3 / 4)), point3[2]);
	arc3[1] = skArc3->MethodAddByThreePoints(point3[1], pTransGeom2->MethodCreatePoint2d(0, bla + (lenght3 / 4)), point3[3]);

	Profile *pProfile4;

	skProfiles3->raw__AddForSolid(&pProfile4);

	ExtrudeFeatures *ftExtrude3;
	ft2->get_ExtrudeFeatures(&ftExtrude3);

	ExtrudeFeaturePtr extrude3 = ftExtrude3->MethodAddByDistanceExtent(pProfile4, (lenght3 / 2) + 1, kPositiveExtentDirection, kCutOperation);

	

	if (save == 1)
	{
		pPartDoc2->MethodSaveAs("D:\\sborka\\Колесо 1.ipt", true);

		pTrans2 = pTransManager2->MethodEndTransaction();
	}
	if (save == 2)
	{
		pPartDoc2->MethodSaveAs("D:\\sborka\\Колесо 2.ipt", true);

		pTrans2 = pTransManager2->MethodEndTransaction();
	}
}

void DLGStartAssembly::Bearing(double out, double in, double diametr, double lenght, int save, int progress)
{
	//Поиск приложения по ID

	CLSID InvAppClsid;
	HRESULT hRes = CLSIDFromProgID(L"Inventor.Application", &InvAppClsid);
	if (FAILED(hRes)) {
		pInvApp2 = nullptr;
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
		if (!pInvApp2)
			hRes = pInvAppUnk->QueryInterface(__uuidof(Application), (void **)&pInvApp2);

		if (FAILED(hRes)) {
			return;
		}

		pInvApp2->put_Visible(TRUE);
		// Prorgess Bar


		pPartDoc2 = pInvApp2->Documents->MethodAdd(kPartDocumentObject, pInvApp2->FileManager->MethodGetTemplateFile(kPartDocumentObject, kMetricSystemOfMeasure, kGOST_DraftingStandard), true);




		pPartDoc2->DisplayName = _T("Деталь");

		pPartDoc2->get_ComponentDefinition(&pPartComDef2);

		pInvApp2->get_TransientGeometry(&pTransGeom2);
		// Prorgess Bar


		pTransManager2 = pInvApp2->GetTransactionManager();

		Doc2 = CComQIPtr <Document>(pPartDoc2);

		pTrans2 = pTransManager2->MethodStartTransaction(Doc2, _T(""));
		// Prorgess Bar


		pPartComDef2->get_Sketches(&sketches2);
		pPartComDef2->get_Sketches3D(&sketches3D2);

		pPartComDef2->get_WorkPlanes(&wp2);
		pPartComDef2->get_Features(&ft2);
		pPartComDef2->get_WorkAxes(&wax2);
		pPartComDef2->get_WorkPoints(&wpt2);
	}

	// Вращение


	PlanarSketch *pSketch1;
	sketches2->raw_Add(wp2->GetItem(3), false, &pSketch1);

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

	point[0] = skPoints->MethodAdd(pTransGeom2->MethodCreatePoint2d(diametr / 2 + 0.1, 0), false);
	point[1] = skPoints->MethodAdd(pTransGeom2->MethodCreatePoint2d(diametr / 2 + 0.1, lenght / 1.8), false);
	point[2] = skPoints->MethodAdd(pTransGeom2->MethodCreatePoint2d(in / 2, lenght / 1.8), false);
	point[3] = skPoints->MethodAdd(pTransGeom2->MethodCreatePoint2d(in / 2, (lenght / 1.8)*0.9), false);
	point[4] = skPoints->MethodAdd(pTransGeom2->MethodCreatePoint2d(in / 2, (lenght / 1.8)*0.1), false);
	point[5] = skPoints->MethodAdd(pTransGeom2->MethodCreatePoint2d(in / 2, 0), false);

	lines[0] = skLines->MethodAddByTwoPoints(point[0], point[1]);
	lines[1] = skLines->MethodAddByTwoPoints(point[1], point[2]);
	lines[2] = skLines->MethodAddByTwoPoints(point[2], point[3]);
	lines[3] = skLines->MethodAddByTwoPoints(point[4], point[5]);
	lines[4] = skLines->MethodAddByTwoPoints(point[5], point[0]);

	double rarc = (in - diametr) / 4;

	arc[0] = skArc->MethodAddByThreePoints(point[3], pTransGeom2->MethodCreatePoint2d(diametr / 2 + rarc + 0.1, (lenght / 1.8) / 2), point[4]);

	point[6] = skPoints->MethodAdd(pTransGeom2->MethodCreatePoint2d(out / 2, 0), false);
	point[7] = skPoints->MethodAdd(pTransGeom2->MethodCreatePoint2d(out / 2, lenght / 1.8), false);
	point[8] = skPoints->MethodAdd(pTransGeom2->MethodCreatePoint2d((out / 2) - (rarc * 2), lenght / 1.8), false);
	point[9] = skPoints->MethodAdd(pTransGeom2->MethodCreatePoint2d((out / 2) - (rarc * 2), (lenght / 1.8)*0.9), false);
	point[10] = skPoints->MethodAdd(pTransGeom2->MethodCreatePoint2d((out / 2) - (rarc * 2), (lenght / 1.8)*0.1), false);
	point[11] = skPoints->MethodAdd(pTransGeom2->MethodCreatePoint2d((out / 2) - (rarc * 2), 0), false);


	lines[5] = skLines->MethodAddByTwoPoints(point[6], point[7]);
	lines[6] = skLines->MethodAddByTwoPoints(point[7], point[8]);
	lines[7] = skLines->MethodAddByTwoPoints(point[8], point[9]);
	lines[8] = skLines->MethodAddByTwoPoints(point[10], point[11]);
	lines[9] = skLines->MethodAddByTwoPoints(point[11], point[6]);


	arc[1] = skArc->MethodAddByThreePoints(point[9], pTransGeom2->MethodCreatePoint2d(out / 2 - rarc, (lenght / 1.8) / 2), point[10]);


	Profile *pProfile;
	skProfiles->raw__AddForSolid(&pProfile);

	RevolveFeatures *ftRevolve;
	ft2->get_RevolveFeatures(&ftRevolve);

	RevolveFeaturePtr revolve = ftRevolve->MethodAddFull(pProfile, wax2->GetItem(2), kNewBodyOperation);



	PlanarSketch *pSketch2;
	sketches2->raw_Add(wp2->GetItem(1), false, &pSketch2);

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

	point2[0] = skPoints2->MethodAdd(pTransGeom2->MethodCreatePoint2d(Xcoord, Ycoord1), false);
	point2[1] = skPoints2->MethodAdd(pTransGeom2->MethodCreatePoint2d(Xcoord, Ycoord2), false);

	lines2[0] = skLines2->MethodAddByTwoPoints(point2[0], point2[1]);

	arc2[0] = skArc2->MethodAddByThreePoints(point2[0], pTransGeom2->MethodCreatePoint2d((lenght / 1.8)*0.8, centr), point2[1]);

	Profile *pProfile2;
	skProfiles2->raw__AddForSolid(&pProfile2);

	RevolveFeatures *ftRevolve2;
	ft2->get_RevolveFeatures(&ftRevolve2);

	RevolveFeaturePtr revolve2 = ftRevolve2->MethodAddFull(pProfile2, lines2[0], kNewBodyOperation);



	int number = floor(out);

	ObjectCollection *pCollection;

	pInvApp2->TransientObjects->raw_CreateObjectCollection(vtMissing, &pCollection);

	pCollection->MethodAdd(revolve2);

	CircularPatternFeatures *pCircPatFeat;

	ft2->get_CircularPatternFeatures(&pCircPatFeat);

	CircularPatternFeature *circFeat = pCircPatFeat->MethodAdd(pCollection, wax2->GetItem(2), true, number, "360 град", true, kIdenticalCompute);

	// вырез
	PlanarSketch *pSketch3;
	sketches2->raw_Add(wp2->GetItem(3), false, &pSketch3);

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

	point3[0] = skPoints3->MethodAdd(pTransGeom2->MethodCreatePoint2d(0.5, bla - (lenght / 4) + 0.5), false);
	point3[1] = skPoints3->MethodAdd(pTransGeom2->MethodCreatePoint2d(0.5, bla + (lenght / 4) - 0.5), false);
	point3[2] = skPoints3->MethodAdd(pTransGeom2->MethodCreatePoint2d(-0.5, bla - (lenght / 4) + 0.5), false);
	point3[3] = skPoints3->MethodAdd(pTransGeom2->MethodCreatePoint2d(-0.5, bla + (lenght / 4) - 0.5), false);

	lines3[0] = skLines3->MethodAddByTwoPoints(point3[0], point3[1]);
	lines3[1] = skLines3->MethodAddByTwoPoints(point3[2], point3[3]);

	arc3[0] = skArc3->MethodAddByThreePoints(point3[0], pTransGeom2->MethodCreatePoint2d(0, bla - (lenght / 4)), point3[2]);
	arc3[1] = skArc3->MethodAddByThreePoints(point3[1], pTransGeom2->MethodCreatePoint2d(0, bla + (lenght / 4)), point3[3]);

	Profile *pProfile4;

	skProfiles3->raw__AddForSolid(&pProfile4);

	ExtrudeFeatures *ftExtrude3;
	ft2->get_ExtrudeFeatures(&ftExtrude3);

	ExtrudeFeaturePtr extrude3 = ftExtrude3->MethodAddByDistanceExtent(pProfile4, (lenght / 2) + 1, kPositiveExtentDirection, kCutOperation);

	if (save == 1)
	{
		pPartDoc2->MethodSaveAs("D:\\sborka\\Подшипник 1.ipt", true);

		pTrans2 = pTransManager2->MethodEndTransaction();
	}
	if (save == 2)
	{
		pPartDoc2->MethodSaveAs("D:\\sborka\\Подшипник 2.ipt", true);

		pTrans2 = pTransManager2->MethodEndTransaction();
	}
	if (save == 3)
	{
		pPartDoc2->MethodSaveAs("D:\\sborka\\Подшипник 3.ipt", true);

		pTrans2 = pTransManager2->MethodEndTransaction();
	}
	if (save == 4)
	{
		pPartDoc2->MethodSaveAs("D:\\sborka\\Подшипник 4.ipt", true);

		pTrans2 = pTransManager2->MethodEndTransaction();
	}
	if (save == 5)
	{
		pPartDoc2->MethodSaveAs("D:\\sborka\\Подшипник 5.ipt", true);

		pTrans2 = pTransManager2->MethodEndTransaction();
	}
	if (save == 6)
	{
		pPartDoc2->MethodSaveAs("D:\\sborka\\Подшипник 6.ipt", true);

		pTrans2 = pTransManager2->MethodEndTransaction();
	}
}

DLGStartAssembly::DLGStartAssembly(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DIALOG1, pParent)
{

}

DLGStartAssembly::~DLGStartAssembly()
{
}

void DLGStartAssembly::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	CFont *m_pFont = new CFont();
	m_pFont->CreatePointFont(180, _T("Arial"));
	GetDlgItem(IDC_STATIC_INFO)->SetFont(m_pFont, TRUE);
	DDX_Control(pDX, IDC_PROGRESS1, m_progBarCtrl2);
	m_progBarCtrl2.SetRange(1, 100);

	SetTimer(PROGRESS_TIMER, 100,NULL); // TimerID = 1000; TimerTick = 100ms

	Shaft1Build(8, 5, 6, 4, 5, 6, 1, 8);

	Shaft1Build(8, 5, 6, 4, 5, 6, 2, 16);

	Shaft2Build(11, 15, 6, 5, 6, 7, 24);

	//
	GearOne(2, 60, 6, 6, 1, 32);

	GearOne(2, 60, 6, 6, 2, 40);

	GearTwo(2, 2, 120, 6, 6, 1, 48);

	GearTwo(2, 2, 120, 6, 6, 2, 54);

	Bearing(14, 9, 5, 5, 1, 62);

	Bearing(14, 9, 5, 5, 2, 70);

	Bearing(14, 9, 5, 5, 3, 78);

	Bearing(14, 9, 5, 5, 4, 86);

	Bearing(14, 9, 5, 5, 5, 94);

	Bearing(14, 9, 5, 5, 6, 100);

	EndDialog(IDOK);
}



BEGIN_MESSAGE_MAP(DLGStartAssembly, CDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()



void DLGStartAssembly::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: добавьте свой код обработчика сообщений или вызов стандартного
	if (nIDEvent == PROGRESS_TIMER) // is progress timer fired
	{

		int iPos = m_progBarCtrl2.GetPos();
		if (iPos < 100)
		{
			iPos++;
			m_progBarCtrl2.SetPos(iPos);
		}
		else
		{
			KillTimer(PROGRESS_TIMER);
			EndDialog(IDOK);
			// other functionality
		}
	}
	CDialog::OnTimer(nIDEvent);
}
