#pragma once
#include <afxpropertypage.h>

#include "NumberEdit.h"

class SpaceshipPropertyPage :
	public CMFCPropertyPage
{
	DECLARE_DYNAMIC(SpaceshipPropertyPage)

public:
	SpaceshipPropertyPage();
	~SpaceshipPropertyPage() override;

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SPACESHIPPROPERTYPAGE };
#endif

protected:
	void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	BOOL OnApply() override;
	void ApplyValues();

private:
	CNumberEdit m_ScaleEdit;
	CNumberEdit m_RotateXEdit;
	CNumberEdit m_RotateYEdit;
	CNumberEdit m_RotateZEdit;
	CNumberEdit m_TranslateEdit;

	CString spaceshipObjFile;
	
	double scale;
	double rotateX;
	double rotateY;
	double rotateZ;
	double translate;


	bool ShouldApply();

	afx_msg void OnEnChangeEdit();
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
};

