#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CameraPropertyPage dialog

class CameraPropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CameraPropertyPage)

public:
	CameraPropertyPage();
	virtual ~CameraPropertyPage();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CAMERAPROPERTYPAGE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnApply();
	void ApplyValues();

	CSliderCtrl m_Slider1;
	CSliderCtrl m_Slider2;
	CSliderCtrl m_Slider3;
	virtual BOOL OnInitDialog();
	afx_msg void OnTRBNThumbPosChangingSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTRBNThumbPosChangingSlider2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTRBNThumbPosChangingSlider3(NMHDR *pNMHDR, LRESULT *pResult);
	CStatic m_Static1;
	CStatic m_Static2;
	CStatic m_Static3;
};
