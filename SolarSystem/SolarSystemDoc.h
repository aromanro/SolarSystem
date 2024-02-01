
// SolarSystemDoc.h : interface of the CSolarSystemDoc class
//


#pragma once

#include "SolarSystemBodies.h"
#include "ComputationThread.h"

class CSolarSystemView;

class CSolarSystemDoc : public CDocument
{
protected: // create from serialization only
	CSolarSystemDoc();
	DECLARE_DYNCREATE(CSolarSystemDoc)


	MolecularDynamics::ComputationThread m_Thread;

// Attributes
public:
	SolarSystemBodies m_SolarSystem;

	bool stopped;

	unsigned int nrsteps;
// Operations

	HRESULT LoadFromXmlString(const CString& str);
	void LoadXmlFile(const CString& name);

	CSolarSystemView* GetMainView(void);
	bool RetrieveData();
	void LoadFile(const CString& fileName);
	void StopThread();
	void StartThread();

// Overrides
	BOOL OnNewDocument() override;
	void Serialize(CArchive& ar) override;
#ifdef SHARED_HANDLERS
	void InitializeSearchContent() override;
	void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds) override;
#endif // SHARED_HANDLERS

// Implementation
	~CSolarSystemDoc() override;
#ifdef _DEBUG
	void AssertValid() const override;
	void Dump(CDumpContext& dc) const override;
#endif

// Generated message map functions
private:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS

	void ParseXmlDocument(MSXML::IXMLDOMDocumentPtr& pDocument);
	void LoadBodyXml(MSXML::IXMLDOMNodePtr& node, MolecularDynamics::Body& body, MolecularDynamics::BodyPosition& bodyPosition, BodyProperties& props);
	static CString GetXmlValue(MSXML::IXMLDOMNodePtr pNode, const bstr_t& name, const CString& def = L"");
	static bool GetXmlBoolValue(MSXML::IXMLDOMNodePtr pNode, const bstr_t& name, bool def = false);
	static double GetXmlDoubleValue(MSXML::IXMLDOMNodePtr pNode, const bstr_t& name, double def = 0.);
	static int GetXmlIntValue(MSXML::IXMLDOMNodePtr pNode, const bstr_t& name, int def = 0);
};
