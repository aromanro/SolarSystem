#pragma once


// DisplayPropertyPage dialog

class DisplayPropertyPage : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(DisplayPropertyPage)
public:
	DisplayPropertyPage();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DISPLAYPROPERTYPAGE };
#endif

	int m_Gamma;
	int m_SkyBox;
	int m_Shadows;
	int m_Textures;
	int m_Billboard;
private:
	void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	BOOL OnInitDialog() override;
	bool ShouldApply();
	BOOL OnApply() override;
	void ApplyValues();
	afx_msg void OnBnClickedCheck2();
	afx_msg void OnBnClickedCheck3();
	afx_msg void OnBnClickedCheck4();
	afx_msg void OnBnClickedCheck5();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnEnChangeEdit1();
	CEdit m_GammaEdit;
	double m_GammaCoeff;
};
