#pragma once

#include "EmbeddedSlider.h"

// CMFCToolBarSlider command target

#define MAX_NRSTEPS 300

class CMFCToolBarSlider : public CMFCToolBarButton
{
public:
	CEmbeddedSlider slider;

	CMFCToolBarSlider();
	CMFCToolBarSlider(UINT uiId, int iImage = -1, DWORD dwStyle = 0, int iWidth = 0);
	~CMFCToolBarSlider() override;

	virtual void OnDraw(CDC* /*pDC*/, const CRect& /*rect*/, CMFCToolBarImages* /*pImages*/, BOOL /*bHorz*/ = TRUE, BOOL /*bCustomizeMode*/ = FALSE, BOOL /*bHighlight*/ = FALSE, BOOL /*bDrawBorder*/ = TRUE, BOOL /*bGrayDisabledButtons*/ = TRUE);

	virtual HWND GetHwnd();
	virtual BOOL CanBeStretched() const;
	virtual BOOL HaveHotBorder() const;
	virtual void OnShow(BOOL bShow);
	virtual void OnMove();
	virtual void OnSize(int iSize);
	virtual SIZE OnCalculateSize(CDC* pDC, const CSize& sizeDefault, BOOL bHorz);
	virtual void OnChangeParentWnd(CWnd* pWndParent);

	virtual void Serialize(CArchive& ar);
	virtual void CopyFrom(const CMFCToolBarButton& src);


	static int GetPos(UINT uiCmd);
	static void SetPos(UINT uiCmd, int pos);

	void SetRange(int iMin, int iMax);
	void SetValue(int iValue, BOOL bNotify = TRUE);

	DECLARE_SERIAL(CMFCToolBarSlider);
};


