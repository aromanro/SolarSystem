#include "stdafx.h"
#include "SolarSystem.h"
#include "SpaceshipPropertyPage.h"
#include "afxdialogex.h"

#include "MainFrm.h"

#include <filesystem>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// SpaceshipPropertyPage dialog

IMPLEMENT_DYNAMIC(SpaceshipPropertyPage, CMFCPropertyPage)

SpaceshipPropertyPage::SpaceshipPropertyPage()
	: CMFCPropertyPage(IDD_SPACESHIPPROPERTYPAGE)
{
	scale = theApp.options.scale;
	translate = theApp.options.translate;
	rotateX = theApp.options.rotateX;
	rotateY = theApp.options.rotateY;
	rotateZ = theApp.options.rotateZ;

	spaceshipObjFile = theApp.options.spaceshipObjFile;
}

SpaceshipPropertyPage::~SpaceshipPropertyPage()
{
}

void SpaceshipPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_EDIT1, m_ScaleEdit);
	DDX_Control(pDX, IDC_EDIT2, m_RotateXEdit);
	DDX_Control(pDX, IDC_EDIT3, m_RotateYEdit);
	DDX_Control(pDX, IDC_EDIT4, m_RotateZEdit);
	DDX_Control(pDX, IDC_EDIT5, m_TranslateEdit);

	DDX_Text(pDX, IDC_EDIT1, scale);
	DDX_Text(pDX, IDC_EDIT2, rotateX);
	DDX_Text(pDX, IDC_EDIT3, rotateY);
	DDX_Text(pDX, IDC_EDIT4, rotateZ);
	DDX_Text(pDX, IDC_EDIT5, translate);

	DDX_Text(pDX, IDC_EDIT6, spaceshipObjFile);
}


BEGIN_MESSAGE_MAP(SpaceshipPropertyPage, CMFCPropertyPage)
	ON_EN_CHANGE(IDC_EDIT1, &SpaceshipPropertyPage::OnEnChangeEdit)
	ON_EN_CHANGE(IDC_EDIT2, &SpaceshipPropertyPage::OnEnChangeEdit)
	ON_EN_CHANGE(IDC_EDIT3, &SpaceshipPropertyPage::OnEnChangeEdit)
	ON_EN_CHANGE(IDC_EDIT4, &SpaceshipPropertyPage::OnEnChangeEdit)
	ON_EN_CHANGE(IDC_EDIT5, &SpaceshipPropertyPage::OnEnChangeEdit)
	ON_EN_CHANGE(IDC_EDIT6, &SpaceshipPropertyPage::OnEnChangeEdit)
	ON_BN_CLICKED(IDC_BUTTON1, &SpaceshipPropertyPage::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &SpaceshipPropertyPage::OnBnClickedButton2)
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
		theApp.options.scale = scale;
		theApp.options.translate = translate;
		theApp.options.rotateX = rotateX;
		theApp.options.rotateY = rotateY;
		theApp.options.rotateZ = rotateZ;
		theApp.options.spaceshipObjFile = spaceshipObjFile;

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
	if (theApp.options.scale != scale || theApp.options.translate != translate 
		|| theApp.options.rotateX != rotateX || theApp.options.rotateY != rotateY || theApp.options.rotateZ != rotateZ
		|| theApp.options.spaceshipObjFile != spaceshipObjFile)
		return true;
	// TODO: check if there is new data in here and return true if yes, false otherwise
	return false;
}



void SpaceshipPropertyPage::OnEnChangeEdit()
{
	SetModified();
}




void SpaceshipPropertyPage::OnBnClickedButton1()
{
	CFileDialog dlg(TRUE);
	dlg.m_ofn.lpstrFilter = L"Obj Files (*.obj)\0*.obj\0All Files (*.*)\0*.*\0\0";
	dlg.m_ofn.lpstrTitle = L"Select Obj File";
	
	CString dir;
	if (spaceshipObjFile.IsEmpty())
		dir = L"c:";
	else
	{
		std::filesystem::path dirName(spaceshipObjFile.GetString());

		dir = dirName.remove_filename().string().c_str();
	}

	dlg.m_pOFN->lpstrInitialDir = dir.GetString();

	if (dlg.DoModal() == IDOK)
	{
		spaceshipObjFile = dlg.GetPathName();
		UpdateData(FALSE);
		SetModified();
	}
}


void SpaceshipPropertyPage::OnBnClickedButton2()
{
	spaceshipObjFile.Empty();
	UpdateData(FALSE);
	SetModified();
}
