#pragma once



// OptionsPropertySheet

class OptionsPropertySheet : public CMFCPropertySheet
{
	DECLARE_DYNAMIC(OptionsPropertySheet)

public:
	OptionsPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	OptionsPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~OptionsPropertySheet();

protected:
	HICON hIcon;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};


