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
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedCheck2();
	afx_msg void OnBnClickedCheck3();
	afx_msg void OnBnClickedCheck4();
	int m_Gamma;
	int m_SkyBox;
	int m_Shadows;
	int m_Textures;
private:
	bool ShouldApply();
};
