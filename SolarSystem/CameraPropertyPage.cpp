// CameraPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "SolarSystem.h"
#include "CameraPropertyPage.h"
#include "afxdialogex.h"

#include "SolarSystemDoc.h"
#include "MainFrm.h"


// CameraPropertyPage dialog

IMPLEMENT_DYNAMIC(CameraPropertyPage, CMFCPropertyPage)

CameraPropertyPage::CameraPropertyPage()
	: CMFCPropertyPage(IDD_CAMERAPROPERTYPAGE)
{
}

void CameraPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER1, m_Slider1);
	DDX_Control(pDX, IDC_SLIDER2, m_Slider2);
	DDX_Control(pDX, IDC_SLIDER3, m_Slider3);
	DDX_Control(pDX, IDC_STATIC1, m_Static1);
	DDX_Control(pDX, IDC_STATIC2, m_Static2);
	DDX_Control(pDX, IDC_STATIC3, m_Static3);
}


BEGIN_MESSAGE_MAP(CameraPropertyPage, CMFCPropertyPage)
	ON_NOTIFY(TRBN_THUMBPOSCHANGING, IDC_SLIDER1, &CameraPropertyPage::OnTRBNThumbPosChangingSlider1)
	ON_NOTIFY(TRBN_THUMBPOSCHANGING, IDC_SLIDER2, &CameraPropertyPage::OnTRBNThumbPosChangingSlider2)
	ON_NOTIFY(TRBN_THUMBPOSCHANGING, IDC_SLIDER3, &CameraPropertyPage::OnTRBNThumbPosChangingSlider3)
END_MESSAGE_MAP()


// CameraPropertyPage message handlers


BOOL CameraPropertyPage::OnApply()
{
	UpdateData();

	ApplyValues();

	return CMFCPropertyPage::OnApply();
}


void CameraPropertyPage::ApplyValues()
{
	theApp.options.translationSpeed = static_cast<unsigned int>(m_Slider1.GetPos());
	theApp.options.rotationSpeed = static_cast<unsigned int>(m_Slider2.GetPos());
	theApp.options.scrollSpeed = static_cast<unsigned int>(m_Slider3.GetPos());

	theApp.options.Save();

	CMainFrame *mainFrm = dynamic_cast<CMainFrame*>(theApp.m_pMainWnd);
	if (!mainFrm) return;

	CSolarSystemDoc* doc = dynamic_cast<CSolarSystemDoc*>(mainFrm->GetActiveDocument());
	if (!doc) return;

	CSolarSystemView *view = doc->GetMainView();
	if (!view) return;

	view->SetSpeeds(theApp.options.translationSpeed, theApp.options.rotationSpeed);
}




BOOL CameraPropertyPage::OnInitDialog()
{
	CMFCPropertyPage::OnInitDialog();

	// TODO:  Add extra initialization here
	m_Slider1.SetRange(25, 400);
	m_Slider2.SetRange(25, 400);
	m_Slider3.SetRange(10, 400);

	m_Slider1.SetPos(static_cast<int>(theApp.options.translationSpeed));
	m_Slider2.SetPos(static_cast<int>(theApp.options.rotationSpeed));
	m_Slider3.SetPos(static_cast<int>(theApp.options.scrollSpeed));

	CString str;
	str.Format(L"%d%%", theApp.options.translationSpeed);
	m_Static1.SetWindowText(str);

	str.Format(L"%d%%", theApp.options.rotationSpeed);
	m_Static2.SetWindowText(str);

	str.Format(L"%d%%", theApp.options.scrollSpeed);
	m_Static3.SetWindowText(str);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CameraPropertyPage::OnTRBNThumbPosChangingSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// This feature requires Windows Vista or greater.
	// The symbol _WIN32_WINNT must be >= 0x0600.
	NMTRBTHUMBPOSCHANGING *pNMTPC = reinterpret_cast<NMTRBTHUMBPOSCHANGING *>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	CString str;
	str.Format(L"%d%%", pNMTPC->dwPos);
	m_Static1.SetWindowText(str);

	SetModified();
}


void CameraPropertyPage::OnTRBNThumbPosChangingSlider2(NMHDR *pNMHDR, LRESULT *pResult)
{
	// This feature requires Windows Vista or greater.
	// The symbol _WIN32_WINNT must be >= 0x0600.
	NMTRBTHUMBPOSCHANGING *pNMTPC = reinterpret_cast<NMTRBTHUMBPOSCHANGING *>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	CString str;
	str.Format(L"%d%%", pNMTPC->dwPos);
	m_Static2.SetWindowText(str);

	SetModified();
}


void CameraPropertyPage::OnTRBNThumbPosChangingSlider3(NMHDR *pNMHDR, LRESULT *pResult)
{
	// This feature requires Windows Vista or greater.
	// The symbol _WIN32_WINNT must be >= 0x0600.
	NMTRBTHUMBPOSCHANGING *pNMTPC = reinterpret_cast<NMTRBTHUMBPOSCHANGING *>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	CString str;
	str.Format(L"%d%%", pNMTPC->dwPos);
	m_Static3.SetWindowText(str);

	SetModified();
}
