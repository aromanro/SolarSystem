
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
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CSolarSystemDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS

protected:
	void ParseXmlDocument(MSXML::IXMLDOMDocumentPtr& pDocument);
	void LoadBodyXml(MSXML::IXMLDOMNodePtr& node, MolecularDynamics::Body& body, MolecularDynamics::BodyPosition& bodyPosition, BodyProperties& props);
	static CString GetXmlValue(MSXML::IXMLDOMNodePtr pNode, const bstr_t& name, const CString& def = L"");
	static bool GetXmlBoolValue(MSXML::IXMLDOMNodePtr pNode, const bstr_t& name, bool def = false);
	static double GetXmlDoubleValue(MSXML::IXMLDOMNodePtr pNode, const bstr_t& name, double def = 0.);
	static int GetXmlIntValue(MSXML::IXMLDOMNodePtr pNode, const bstr_t& name, int def = 0);

public:
	HRESULT LoadFromXmlString(const CString& str);
	void LoadXmlFile(const CString& name);

	CSolarSystemView* GetMainView(void);
	void RetrieveData();
	void LoadFile(const CString& fileName);
	void StopThread();
	void StartThread();
};
