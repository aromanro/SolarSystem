// EmbeddedSlider.cpp : implementation file
//

#include "stdafx.h"
#include "SolarSystem.h"
#include "EmbeddedSlider.h"

#include "MFCToolBarSlider.h"

#include "SolarSystemDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CEmbeddedSlider

IMPLEMENT_DYNAMIC(CEmbeddedSlider, CSliderCtrl)

CEmbeddedSlider::CEmbeddedSlider(CMFCToolBarSlider *toolbarBtn)
	: toolbarButton(toolbarBtn),
	style(WS_CHILD | WS_VISIBLE | TBS_HORZ | TBS_NOTICKS),
	width(120), position(1), minval(1), maxval(MAX_NRSTEPS)
{
}

CEmbeddedSlider::~CEmbeddedSlider()
{
}


BEGIN_MESSAGE_MAP(CEmbeddedSlider, CSliderCtrl)
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEHOVER()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()



// CEmbeddedSlider message handlers

void CEmbeddedSlider::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	int pos = GetPos();

	CSliderCtrl::OnKeyDown(nChar, nRepCnt, nFlags);

	RefreshPos(pos);
}


void CEmbeddedSlider::OnLButtonUp(UINT nFlags, CPoint point)
{
	CSliderCtrl::OnLButtonUp(nFlags, point);

	if (toolbarButton && IsWindowVisible())
		toolbarButton->SetValue(GetPos(), TRUE);
}


void CEmbeddedSlider::OnMouseMove(UINT nFlags, CPoint point)
{
	int pos = GetPos();

	CSliderCtrl::OnMouseMove(nFlags, point);

	RefreshPos(pos);
}


void CEmbeddedSlider::OnMouseHover(UINT nFlags, CPoint point)
{
	int pos = GetPos();

	CSliderCtrl::OnMouseHover(nFlags, point);

	RefreshPos(pos);
}


void CEmbeddedSlider::OnLButtonDown(UINT nFlags, CPoint point)
{
	int pos = GetPos();

	CSliderCtrl::OnLButtonDown(nFlags, point);

	RefreshPos(pos);
}


void CEmbeddedSlider::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	int pos = GetPos();

	CSliderCtrl::OnLButtonDblClk(nFlags, point);

	RefreshPos(pos);
}


void CEmbeddedSlider::RefreshPos(int oldPos)
{
	if (toolbarButton && oldPos != GetPos())
		toolbarButton->SetValue(GetPos(), TRUE);
}



int CEmbeddedSlider::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CSliderCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetRange(minval, maxval);
	RetrievePos();

	return 0;
}


void CEmbeddedSlider::RetrievePos()
{
	CWnd* pWndMain = AfxGetMainWnd();
	if (pWndMain)
	{
		CSolarSystemDoc* pDoc = (CSolarSystemDoc*)((CFrameWnd*)pWndMain)->GetActiveDocument();
		if (pDoc) {
			position = (int)pDoc->nrsteps;
			if (GetSafeHwnd()) SetPos(position);
		}
	}
}


void CEmbeddedSlider::OnSetFocus(CWnd* pOldWnd)
{
	CSliderCtrl::OnSetFocus(pOldWnd);

	// don't let it steal focus
	pOldWnd->SetFocus();
}

