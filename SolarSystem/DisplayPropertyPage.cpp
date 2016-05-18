// DisplayPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "SolarSystem.h"
#include "DisplayPropertyPage.h"
#include "afxdialogex.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// DisplayPropertyPage dialog

IMPLEMENT_DYNAMIC(DisplayPropertyPage, CPropertyPage)

DisplayPropertyPage::DisplayPropertyPage()
	: CPropertyPage(IDD_DISPLAYPROPERTYPAGE)
{
	m_Gamma = (theApp.options.gammaCorrection ? 1 : 0);
	m_Shadows = (theApp.options.drawShadows ? 1 : 0);
	m_SkyBox = (theApp.options.showSkyBox ? 1 : 0);
	m_Textures = (theApp.options.drawTextures ? 1 : 0);
}

DisplayPropertyPage::~DisplayPropertyPage()
{
}

void DisplayPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_CHECK1, m_Gamma);
	DDX_Check(pDX, IDC_CHECK2, m_SkyBox);
	DDX_Check(pDX, IDC_CHECK3, m_Shadows);
	DDX_Check(pDX, IDC_CHECK4, m_Textures);
}


BEGIN_MESSAGE_MAP(DisplayPropertyPage, CPropertyPage)
	ON_BN_CLICKED(IDC_CHECK1, &DisplayPropertyPage::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_CHECK2, &DisplayPropertyPage::OnBnClickedCheck2)
	ON_BN_CLICKED(IDC_CHECK3, &DisplayPropertyPage::OnBnClickedCheck3)
	ON_BN_CLICKED(IDC_CHECK4, &DisplayPropertyPage::OnBnClickedCheck4)
END_MESSAGE_MAP()


// DisplayPropertyPage message handlers


BOOL DisplayPropertyPage::OnApply()
{
	UpdateData();

	ApplyValues();

	return CPropertyPage::OnApply();
}


void DisplayPropertyPage::ApplyValues()
{		
	if (ShouldApply())
	{
		theApp.options.gammaCorrection = (1 == m_Gamma);
		theApp.options.drawShadows = (1 == m_Shadows);
		theApp.options.drawTextures = (1 == m_Textures);
		theApp.options.showSkyBox = (1 == m_SkyBox);

		theApp.options.Save();

		CMainFrame *mainFrm = (CMainFrame*)theApp.m_pMainWnd;

		if (mainFrm)
		{
			CSolarSystemView* view = mainFrm->getView();
			if (view)
			{
				view->Reset();
			}
		}
	}
}


void DisplayPropertyPage::OnBnClickedCheck1()
{
	SetModified();
}


void DisplayPropertyPage::OnBnClickedCheck2()
{
	SetModified();
}


void DisplayPropertyPage::OnBnClickedCheck3()
{
	SetModified();
}


void DisplayPropertyPage::OnBnClickedCheck4()
{
	SetModified();
}


bool DisplayPropertyPage::ShouldApply()
{
	return theApp.options.gammaCorrection != (1 == m_Gamma ? true : false) || 
		theApp.options.drawShadows != (1 == m_Shadows ? true : false) ||
		theApp.options.drawTextures != (1 == m_Textures ? true : false) ||
		theApp.options.showSkyBox != (1 == m_SkyBox ? true : false);
}
