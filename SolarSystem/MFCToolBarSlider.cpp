// MFCToolBarSlider.cpp : implementation file
//

#include "stdafx.h"
#include "SolarSystem.h"
#include "MFCToolBarSlider.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMFCToolBarSlider

IMPLEMENT_SERIAL(CMFCToolBarSlider, CMFCToolBarButton, VERSIONABLE_SCHEMA | 1);


CMFCToolBarSlider::CMFCToolBarSlider()
	: slider(this)
{	
}

CMFCToolBarSlider::CMFCToolBarSlider(UINT uiId, int iImage,	DWORD dwStyle, int iWidth) 
	: CMFCToolBarButton(uiId, iImage),
	slider(this)
{
	slider.style = dwStyle | WS_CHILD | WS_VISIBLE | TBS_NOTICKS | TBS_HORZ;
	slider.width = (iWidth ? iWidth : 120);
}

CMFCToolBarSlider::~CMFCToolBarSlider()
{
	if (GetHwnd()) slider.DestroyWindow();
}


// CMFCToolBarSlider member functions


int CMFCToolBarSlider::GetPos(UINT uiCmd)
{
	CObList listButtons;
	if (CMFCToolBar::GetCommandButtons(uiCmd, listButtons) > 0)
	{
		for (POSITION pos = listButtons.GetHeadPosition(); pos;)
		{
			CMFCToolBarSlider* pSlider = DYNAMIC_DOWNCAST(CMFCToolBarSlider, listButtons.GetNext(pos));
			if (pSlider) return pSlider->slider.position;
		}
	}

	return -1;
}

void CMFCToolBarSlider::SetPos(UINT uiCmd, int position)
{
	CObList listButtons;
	if (CMFCToolBar::GetCommandButtons(uiCmd, listButtons) > 0)
	{
		for (POSITION pos = listButtons.GetHeadPosition(); pos;)
		{
			CMFCToolBarSlider* pSlider = DYNAMIC_DOWNCAST(CMFCToolBarSlider, listButtons.GetNext(pos));
			if (pSlider) pSlider->SetValue(position,FALSE);
		}
	}
}

void CMFCToolBarSlider::SetRange(int iMin, int iMax)
{
	slider.minval = iMin;
	slider.maxval = iMax;

	if (GetHwnd()) slider.SetRange(iMin, iMax, TRUE);

	CObList listButtons;
	if (CMFCToolBar::GetCommandButtons(m_nID, listButtons) > 0)
	{
		for (POSITION pos = listButtons.GetHeadPosition(); pos;)
		{
			CMFCToolBarSlider* pSlider = DYNAMIC_DOWNCAST(CMFCToolBarSlider, listButtons.GetNext(pos));
			if (pSlider && pSlider != this)
			{
				pSlider->slider.minval = iMin;
				pSlider->slider.maxval = iMax;

				if (pSlider->GetHwnd())	pSlider->slider.SetRange(iMin, iMax, TRUE);
			}
		}
	}
}

void CMFCToolBarSlider::SetValue(int iValue, BOOL bNotify)
{
	slider.position = iValue;
	if (GetHwnd()) slider.SetPos(iValue);

	CObList listButtons;
	if (CMFCToolBar::GetCommandButtons(m_nID, listButtons) > 0)
	{
		for (POSITION pos = listButtons.GetHeadPosition(); pos;)
		{
			CMFCToolBarSlider* pSlider = DYNAMIC_DOWNCAST(CMFCToolBarSlider, listButtons.GetNext(pos));
			if (pSlider && pSlider != this)
			{
				pSlider->slider.position = iValue;

				if (pSlider->GetHwnd())	pSlider->slider.SetPos(iValue);
			}
		}
	}

	if (bNotify && GetHwnd())
		slider.GetOwner()->SendMessage(WM_COMMAND, m_nID);
}


void CMFCToolBarSlider::OnDraw(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages,	BOOL bHorz, BOOL bCustomizeMode, BOOL bHighlight, BOOL bDrawBorder, BOOL bGrayDisabledButtons) 
{
	if(!GetHwnd())
		CMFCToolBarButton::OnDraw(pDC, rect, pImages, bHorz, bCustomizeMode, bHighlight, bDrawBorder, bGrayDisabledButtons);
}

HWND CMFCToolBarSlider::GetHwnd()
{
	return slider.GetSafeHwnd();
}

BOOL CMFCToolBarSlider::CanBeStretched() const
{
	return TRUE;
}

BOOL CMFCToolBarSlider::HaveHotBorder() const
{
	return FALSE;
}

void CMFCToolBarSlider::OnShow(BOOL bShow)
{
	if (GetHwnd())
	{
		if (bShow)
		{
			slider.ShowWindow(SW_SHOWNOACTIVATE);
			OnMove();
		}
		else
			slider.ShowWindow(SW_HIDE);
	}

}


void CMFCToolBarSlider::OnMove()
{
	CMFCToolBarButton::OnMove();

	if (GetHwnd() && (slider.GetStyle() & WS_VISIBLE))
	   slider.SetWindowPos(NULL, m_rect.left + 1, m_rect.top + 1, m_rect.Width() - 2, m_rect.Height() - 2, SWP_NOZORDER | SWP_NOACTIVATE);
}

void CMFCToolBarSlider::OnSize(int iSize)
{
	CMFCToolBarButton::OnSize(iSize);

	slider.width = iSize;
	m_rect.right = m_rect.left + slider.width;

	OnMove();
}

SIZE CMFCToolBarSlider::OnCalculateSize(CDC* /*pDC*/, const CSize& /*sizeDefault*/, BOOL bHorz)
{
	if (!IsVisible())
	{
		if (GetHwnd()) slider.ShowWindow(SW_HIDE);

		return CSize(0, 0);
	}

	if (GetHwnd() && !IsHidden())
	{
		slider.ShowWindow(SW_SHOWNOACTIVATE);
		slider.ModifyStyle((DWORD)(bHorz ? TBS_VERT : TBS_HORZ), (DWORD)(bHorz ? TBS_HORZ : TBS_VERT));
	}

	return CSize(bHorz ? slider.width : 24, bHorz ? 24 : slider.width);
}

void CMFCToolBarSlider::OnChangeParentWnd(CWnd* pWndParent)
{
	if (GetHwnd())
	{
		CWnd* pWndParentCurr = slider.GetParent();

		if (pWndParent && pWndParentCurr &&	pWndParentCurr->GetSafeHwnd() == pWndParent->GetSafeHwnd())
			return;

		slider.DestroyWindow();
	}

	if (NULL == pWndParent || NULL == pWndParent->GetSafeHwnd())
		return;

	slider.RetrievePos();

	if (!slider.Create(slider.style, m_rect, pWndParent, m_nID))
		return;

	slider.SetOwner(pWndParent->GetTopLevelFrame());
}


void CMFCToolBarSlider::Serialize(CArchive& ar)
{
	CMFCToolBarButton::Serialize(ar);

	if (ar.IsLoading())
	{
		ar >> slider.position;
		ar >> slider.minval;
		ar >> slider.maxval;
		ar >> slider.width;
		ar >> slider.style;

		SetRange(slider.minval , slider.maxval);

		slider.RetrievePos();
	}
	else
	{
		ar << slider.position;
		ar << slider.minval;
		ar << slider.maxval;
		ar << slider.width;
		ar << slider.style;
	}
}

void CMFCToolBarSlider::CopyFrom(const CMFCToolBarButton& src)
{
	CMFCToolBarButton::CopyFrom(src);

	const CMFCToolBarSlider& srcslider = (const CMFCToolBarSlider&)src;

	slider.minval = srcslider.slider.minval;
	slider.maxval = srcslider.slider.maxval;
	slider.style = srcslider.slider.style;
	slider.width = srcslider.slider.width;

	if (GetHwnd()) slider.SetRange(slider.minval, slider.maxval);

	slider.RetrievePos();
}

