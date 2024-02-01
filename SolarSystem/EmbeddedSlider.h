#pragma once

class CMFCToolBarSlider;

// CEmbeddedSlider

class CEmbeddedSlider : public CSliderCtrl
{
	DECLARE_DYNAMIC(CEmbeddedSlider)
public:
	explicit CEmbeddedSlider(CMFCToolBarSlider* toolbarBtn);

	void RetrievePos();

	int width;
	DWORD style;
	int minval;
	int maxval;
	int position;

private:
	void RefreshPos(int oldPos);

	CMFCToolBarSlider *toolbarButton;

	DECLARE_MESSAGE_MAP()

	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
};


