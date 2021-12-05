
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "SolarSystem.h"

#include "MainFrm.h"
#include "SolarSystemDoc.h"
#include "SolarSystemView.h"
#include "resource.h"

#include "Options.h"
#include "OptionsPropertySheet.h"
#include "DisplayPropertyPage.h"
#include "CameraPropertyPage.h"
#include "SpaceshipPropertyPage.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_COMMAND(ID_VIEW_FULLSCREEN, &CMainFrame::OnViewFullscreen)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FULLSCREEN, &CMainFrame::OnUpdateViewFullscreen)
	ON_COMMAND(ID_SIMULATE_RUN, &CMainFrame::OnSimulateRun)
	ON_UPDATE_COMMAND_UI(ID_SIMULATE_RUN, &CMainFrame::OnUpdateSimulateRun)
	ON_REGISTERED_MESSAGE(AFX_WM_RESETTOOLBAR, OnToolbarReset)
	ON_COMMAND(ID_SLIDER, OnSlider)
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(ID_FILE_OPEN, &CMainFrame::OnFileOpen)
	ON_COMMAND(ID_SIMULATE_OPTIONS, &CMainFrame::OnSimulateOptions)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	theApp.m_nAppLook = static_cast<UINT>(theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2008));
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("Failed to create menubar\n");
		return -1;      // fail to create
	}

	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// prevent the menu bar from taking the focus on activation
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(static_cast<UINT>(theApp.m_bHiColorIcons ? IDR_MAINFRAME_256 : IDR_MAINFRAME)))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	CString strToolBarName;
	BOOL bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
	ASSERT(bNameValid);
	m_wndToolBar.SetWindowText(strToolBarName);

	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);
	m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

	// Allow user-defined toolbars operations:
	InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: Delete these five lines if you don't want the toolbar and menubar to be dockable
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);
	DockPane(&m_wndToolBar);


	// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);
	// enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);
	// set the visual manager and style based on persisted value
	OnApplicationLook(theApp.m_nAppLook);

	// Enable toolbar and docking window menu replacement
	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// enable quick (Alt+drag) toolbar customization
	CMFCToolBar::EnableQuickCustomization();

	if (CMFCToolBar::GetUserImages() == NULL)
	{
		// load user-defined toolbar images
		if (m_UserImages.Load(_T(".\\UserImages.bmp")))
		{
			CMFCToolBar::SetUserImages(&m_UserImages);
		}
	}

	// enable menu personalization (most-recently used commands)
	// TODO: define your own basic commands, ensuring that each pulldown menu has at least one basic command.
	CList<UINT, UINT> lstBasicCommands;

	lstBasicCommands.AddTail(ID_FILE_OPEN);
	lstBasicCommands.AddTail(ID_APP_EXIT);
	lstBasicCommands.AddTail(ID_APP_ABOUT);
	lstBasicCommands.AddTail(ID_VIEW_STATUS_BAR);
	lstBasicCommands.AddTail(ID_VIEW_TOOLBAR);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2003);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_VS_2005);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLUE);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_SILVER);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLACK);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_AQUA);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_WINDOWS_7);

	lstBasicCommands.AddTail(ID_SIMULATE_OPTIONS);
	lstBasicCommands.AddTail(ID_SIMULATE_RUN);
	lstBasicCommands.AddTail(ID_VIEW_FULLSCREEN);

	CMFCToolBar::SetBasicCommands(lstBasicCommands);

	EnableFullScreenMode(ID_VIEW_FULLSCREEN);
	EnableFullScreenMainMenu(FALSE);
	
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE | WS_SYSMENU;

	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* scan menus */);
	pDlgCust->EnableUserDefinedToolbars();
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

	return lres;
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
	}

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), (int)theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}


BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	// base class does the real work

	if (!CFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}


	// enable customization button for all user toolbars
	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	for (int i = 0; i < iMaxUserToolbars; i ++)
	{
		CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		if (pUserToolbar != NULL)
		{
			pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
		}
	}

	return TRUE;
}



BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	CSolarSystemView* view = getView();
	if (view)
	{
		const bool handled = view->KeyPressHandler(pMsg);
		if (handled) return TRUE;
	}

	return CFrameWndEx::PreTranslateMessage(pMsg);
}


void CMainFrame::OnViewFullscreen()
{
	ShowFullScreen();

	if (IsFullScreen())
	{
		CDockingManager* dock = GetDockingManager();
		CObList lstBars;
		dock->GetPaneList(lstBars);

		POSITION pos = lstBars.GetHeadPosition();
		while (pos != NULL)
		{
			const CWnd* wnd = (CWnd*)lstBars.GetNext(pos);
			if (wnd->IsKindOf(RUNTIME_CLASS(CMFCToolBar)) && !wnd->IsKindOf(RUNTIME_CLASS(CMFCStatusBar)) && !wnd->IsKindOf(RUNTIME_CLASS(CMFCMenuBar)) && wnd->GetSafeHwnd() != m_wndToolBar.GetSafeHwnd())
				wnd->GetParent()->ShowWindow(SW_HIDE);			
		}
	}
}


void CMainFrame::OnUpdateViewFullscreen(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(IsFullScreen());
	pCmdUI->Enable();
}


void CMainFrame::OnSimulateRun()
{
	CWnd* pWndMain = AfxGetMainWnd();

	if (pWndMain)
	{
		CSolarSystemDoc* pDoc = dynamic_cast<CSolarSystemDoc*>(dynamic_cast<CFrameWnd*>(pWndMain)->GetActiveDocument());

		pDoc->stopped = !pDoc->stopped;
	}
}


void CMainFrame::OnUpdateSimulateRun(CCmdUI *pCmdUI)
{
	CWnd* pWndMain = AfxGetMainWnd();

	if (pWndMain)
	{
		const CSolarSystemDoc* pDoc = dynamic_cast<CSolarSystemDoc*>(dynamic_cast<CFrameWnd*>(pWndMain)->GetActiveDocument());

		pCmdUI->Enable();
		pCmdUI->SetCheck(!pDoc->stopped);
	}
}


afx_msg LRESULT CMainFrame::OnToolbarReset(WPARAM wp, LPARAM)
{
	const UINT uiToolBarId = static_cast<UINT>(wp);
	if (uiToolBarId == IDR_MAINFRAME || uiToolBarId == IDR_MAINFRAME_256)
	{
		CMFCToolBarSlider btnSlider(ID_SLIDER);		
		CWnd* pWndMain = AfxGetMainWnd();
		if (pWndMain)
		{
			const CSolarSystemDoc* pDoc = dynamic_cast<CSolarSystemDoc*>(dynamic_cast<CFrameWnd*>(pWndMain)->GetActiveDocument());
			if (pDoc) btnSlider.SetPos(ID_SLIDER, static_cast<int>(pDoc->nrsteps));
		}

		m_wndToolBar.ReplaceButton(ID_SLIDER, btnSlider);
	}

	return 0;
}



void CMainFrame::OnSlider()
{
	CWnd* pWndMain = AfxGetMainWnd();

	if (!pWndMain) return;

	if (GetAsyncKeyState(VK_LBUTTON) == 0)
	{
		const int npos = CMFCToolBarSlider::GetPos(ID_SLIDER);

		CSolarSystemDoc* pDoc = dynamic_cast<CSolarSystemDoc*>(dynamic_cast<CFrameWnd*>(pWndMain)->GetActiveDocument());
		
		pDoc->nrsteps = static_cast<unsigned int>(npos);
	}
}



BOOL CMainFrame::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	CSolarSystemView* view = getView();
	if (view) view->OnMouseWheel(nFlags, zDelta, pt);

	return 0;
}


void CMainFrame::OnFileOpen()
{
	TCHAR szFilters[] = _T("Xml Files (*.xml)|*.xml|All Files (*.*)|*.*||");

	CFileDialog dlg(TRUE,_T("xml"), _T("*.xml"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);

	if (IDOK == dlg.DoModal())
	{
		CString pathName = dlg.GetPathName();

		CWnd* pWndMain = AfxGetMainWnd();

		if (pWndMain)
		{
			CSolarSystemDoc* pDoc = dynamic_cast<CSolarSystemDoc*>(dynamic_cast<CFrameWnd*>(pWndMain)->GetActiveDocument());
		
			if (pDoc)
			{
				const bool saveStopped = pDoc->stopped;
				pDoc->stopped = true; // stop requesting data from computation thread
				pDoc->StopThread();
			
				pDoc->LoadFile(pathName);

				POSITION pos = pDoc->GetFirstViewPosition();
				if (pos)
				{
					CSolarSystemView* view = dynamic_cast<CSolarSystemView*>(pDoc->GetNextView(pos));
					view->Reset();
				}

				pDoc->StartThread();

				CString fileName = dlg.GetFileName();
				pWndMain->SetWindowText(CString(L"Solar System - ") + fileName);

				pDoc->stopped = saveStopped;
			}
		}
	}
	

}


void CMainFrame::OnSimulateOptions()
{
	OptionsPropertySheet* sheet = new OptionsPropertySheet(L"Options");
	DisplayPropertyPage page1;
	CameraPropertyPage page2;
	SpaceshipPropertyPage page3;

	sheet->AddPage(&page1);
	sheet->AddPage(&page2);
	sheet->AddPage(&page3);

	sheet->DoModal();
	delete sheet;
}


CSolarSystemView* CMainFrame::getView()
{
	CWnd* pWndMain = AfxGetMainWnd();

	if (pWndMain)
	{
		const CDocument* pDoc = dynamic_cast<CFrameWnd*>(pWndMain)->GetActiveDocument();
		if (pDoc)
		{
			POSITION pos = pDoc->GetFirstViewPosition();
			if (pos)
			{
				return dynamic_cast<CSolarSystemView*>(pDoc->GetNextView(pos));
			}
		}
	}

	return nullptr;
}
