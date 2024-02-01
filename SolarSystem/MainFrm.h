
// MainFrm.h : interface of the CMainFrame class
//

#pragma once

#include "MFCToolBarSlider.h"
#include "SolarSystemView.h"

class CMainFrame : public CFrameWndEx
{	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
// Operations
// Overrides
	BOOL PreCreateWindow(CREATESTRUCT& cs) override;
	BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = nullptr, CCreateContext* pContext = nullptr) override;

// Implementation
	CSolarSystemView* getView();
	afx_msg void OnSimulateRun();

#ifdef _DEBUG
	void AssertValid() const override;
	void Dump(CDumpContext& dc) const override;
#endif

	CMFCToolBar m_wndToolBar;

private:  
// control bar embedded members
	CMFCMenuBar       m_wndMenuBar;
	CMFCStatusBar     m_wndStatusBar;
	CMFCToolBarImages m_UserImages;

// Generated message map functions
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnToolbarReset(WPARAM, LPARAM);
	afx_msg void OnSlider();

	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()

	BOOL PreTranslateMessage(MSG* pMsg) override;
	afx_msg void OnViewFullscreen();
	afx_msg void OnUpdateViewFullscreen(CCmdUI *pCmdUI);
	afx_msg void OnUpdateSimulateRun(CCmdUI *pCmdUI);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnFileOpen();
	afx_msg void OnSimulateOptions();
};


