#pragma once


// DisplayPropertyPage dialog

class DisplayPropertyPage : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(DisplayPropertyPage)

public:
	DisplayPropertyPage();
	virtual ~DisplayPropertyPage();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DISPLAYPROPERTYPAGE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnApply();
	void ApplyValues();
	afx_msg void OnBnClickedCheck2();
	afx_msg void OnBnClickedCheck3();
	afx_msg void OnBnClickedCheck4();

	int m_Gamma;
	int m_SkyBox;
	int m_Shadows;
	int m_Textures;
	int m_Billboard;
private:
	bool ShouldApply();
public:
	afx_msg void OnBnClickedCheck5();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnEnChangeEdit1();
	CEdit m_GammaEdit;
	double m_GammaCoeff;
	virtual BOOL OnInitDialog();
};
