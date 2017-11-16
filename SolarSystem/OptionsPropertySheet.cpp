// OptionsPropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "SolarSystem.h"
#include "OptionsPropertySheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// OptionsPropertySheet

IMPLEMENT_DYNAMIC(OptionsPropertySheet, CMFCPropertySheet)

OptionsPropertySheet::OptionsPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CMFCPropertySheet(nIDCaption, pParentWnd, iSelectPage), hIcon(0)
{

}

OptionsPropertySheet::OptionsPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CMFCPropertySheet(pszCaption, pParentWnd, iSelectPage), hIcon(0)
{

}

OptionsPropertySheet::~OptionsPropertySheet()
{
	if (hIcon) DestroyIcon(hIcon);
}


BEGIN_MESSAGE_MAP(OptionsPropertySheet, CMFCPropertySheet)
END_MESSAGE_MAP()


// OptionsPropertySheet message handlers


BOOL OptionsPropertySheet::OnInitDialog()
{
	BOOL bResult = CMFCPropertySheet::OnInitDialog();

	hIcon = static_cast<HICON>(::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0));

	SetIcon(hIcon, FALSE);

	return bResult;
}
