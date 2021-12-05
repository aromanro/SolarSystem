#include "stdafx.h"
#include "SolarSystem.h"
#include "SpaceshipPropertyPage.h"
#include "afxdialogex.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// SpaceshipPropertyPage dialog

IMPLEMENT_DYNAMIC(SpaceshipPropertyPage, CMFCPropertyPage)

SpaceshipPropertyPage::SpaceshipPropertyPage()
	: CMFCPropertyPage(IDD_SPACESHIPPROPERTYPAGE)
{
	// TODO: Copy values from options into the local ones
}

SpaceshipPropertyPage::~SpaceshipPropertyPage()
{
}

void SpaceshipPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);

	// TODO: DDX for controls - data
}


BEGIN_MESSAGE_MAP(SpaceshipPropertyPage, CMFCPropertyPage)
END_MESSAGE_MAP()


// SpaceshipPropertyPage message handlers


BOOL SpaceshipPropertyPage::OnApply()
{
	UpdateData();

	ApplyValues();

	return CMFCPropertyPage::OnApply();
}


void SpaceshipPropertyPage::ApplyValues()
{
	if (ShouldApply())
	{
		// TODO: copy the values set here into options

		theApp.options.Save();

		CMainFrame* mainFrm = dynamic_cast<CMainFrame*>(theApp.m_pMainWnd);

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



bool SpaceshipPropertyPage::ShouldApply()
{
	// TODO: check if there is new data in here and return true if yes, false otherwise
	return false;
}

