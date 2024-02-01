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

IMPLEMENT_DYNAMIC(DisplayPropertyPage, CMFCPropertyPage)

DisplayPropertyPage::DisplayPropertyPage()
	: CMFCPropertyPage(IDD_DISPLAYPROPERTYPAGE)
{
	m_Gamma = theApp.options.gammaCorrection;
	m_GammaCoeff = theApp.options.gammaCorrectionCoeff;

	m_Shadows = (theApp.options.drawShadows ? 1 : 0);
	m_SkyBox = (theApp.options.showSkyBox ? 1 : 0);
	m_Textures = (theApp.options.drawTextures ? 1 : 0);
	m_Billboard = (theApp.options.showBillboard ? 1 : 0);
}


void DisplayPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);

	DDX_Radio(pDX, IDC_RADIO1, m_Gamma);
	DDX_Check(pDX, IDC_CHECK2, m_SkyBox);
	DDX_Check(pDX, IDC_CHECK3, m_Shadows);
	DDX_Check(pDX, IDC_CHECK4, m_Textures);
	DDX_Check(pDX, IDC_CHECK5, m_Billboard);
	DDX_Control(pDX, IDC_EDIT1, m_GammaEdit);
	DDX_Text(pDX, IDC_EDIT1, m_GammaCoeff);
	DDV_MinMaxDouble(pDX, m_GammaCoeff, 1, 2.5);
}


BEGIN_MESSAGE_MAP(DisplayPropertyPage, CMFCPropertyPage)
	ON_BN_CLICKED(IDC_CHECK2, &DisplayPropertyPage::OnBnClickedCheck2)
	ON_BN_CLICKED(IDC_CHECK3, &DisplayPropertyPage::OnBnClickedCheck3)
	ON_BN_CLICKED(IDC_CHECK4, &DisplayPropertyPage::OnBnClickedCheck4)
	ON_BN_CLICKED(IDC_CHECK5, &DisplayPropertyPage::OnBnClickedCheck5)
	ON_BN_CLICKED(IDC_RADIO1, &DisplayPropertyPage::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &DisplayPropertyPage::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO3, &DisplayPropertyPage::OnBnClickedRadio3)
	ON_EN_CHANGE(IDC_EDIT1, &DisplayPropertyPage::OnEnChangeEdit1)
END_MESSAGE_MAP()


// DisplayPropertyPage message handlers


BOOL DisplayPropertyPage::OnApply()
{
	UpdateData();

	ApplyValues();

	return CMFCPropertyPage::OnApply();
}


void DisplayPropertyPage::ApplyValues()
{		
	if (ShouldApply())
	{
		theApp.options.gammaCorrection = m_Gamma;
		theApp.options.gammaCorrectionCoeff = m_GammaCoeff;
		theApp.options.drawShadows = (1 == m_Shadows);
		theApp.options.drawTextures = (1 == m_Textures);
		theApp.options.showSkyBox = (1 == m_SkyBox);
		theApp.options.showBillboard = (1 == m_Billboard);

		theApp.options.Save();

		CMainFrame *mainFrm = dynamic_cast<CMainFrame*>(theApp.m_pMainWnd);

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

void DisplayPropertyPage::OnBnClickedCheck5()
{
	SetModified();
}

bool DisplayPropertyPage::ShouldApply()
{
	return theApp.options.gammaCorrection != m_Gamma || 
		theApp.options.gammaCorrectionCoeff != m_GammaCoeff ||
		theApp.options.drawShadows != (1 == m_Shadows) ||
		theApp.options.drawTextures != (1 == m_Textures) ||
		theApp.options.showSkyBox != (1 == m_SkyBox) ||
		theApp.options.showBillboard != (1 == m_Billboard);
}


void DisplayPropertyPage::OnBnClickedRadio1()
{
	SetModified();

	m_GammaEdit.SetWindowText(L"1");
	m_GammaEdit.SetReadOnly();
}


void DisplayPropertyPage::OnBnClickedRadio2()
{
	SetModified();

	m_GammaEdit.SetWindowText(L"1");
	m_GammaEdit.SetReadOnly();
}


void DisplayPropertyPage::OnBnClickedRadio3()
{
	SetModified();

	m_GammaEdit.SetReadOnly(FALSE);
}


void DisplayPropertyPage::OnEnChangeEdit1()
{
	SetModified();
}


BOOL DisplayPropertyPage::OnInitDialog()
{
	CMFCPropertyPage::OnInitDialog();

	if (2 == m_Gamma)
		m_GammaEdit.SetReadOnly(FALSE);
	else
		m_GammaEdit.SetReadOnly();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}
