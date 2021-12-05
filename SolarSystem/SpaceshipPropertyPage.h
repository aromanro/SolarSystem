#pragma once
#include <afxpropertypage.h>
class SpaceshipPropertyPage :
	public CMFCPropertyPage
{
	DECLARE_DYNAMIC(SpaceshipPropertyPage)

public:
	SpaceshipPropertyPage();
	virtual ~SpaceshipPropertyPage();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SPACESHIPPROPERTYPAGE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnApply();
	void ApplyValues();

private:
	bool ShouldApply();
};

