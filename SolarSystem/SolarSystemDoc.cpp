
// SolarSystemDoc.cpp : implementation of the CSolarSystemDoc class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "SolarSystem.h"
#endif

#include "SolarSystemDoc.h"
#include "SolarSystemView.h"

#include "Constants.h"

#include <propkey.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CSolarSystemDoc

IMPLEMENT_DYNCREATE(CSolarSystemDoc, CDocument)

BEGIN_MESSAGE_MAP(CSolarSystemDoc, CDocument)
END_MESSAGE_MAP()


// CSolarSystemDoc construction/destruction

CSolarSystemDoc::CSolarSystemDoc()
	: stopped(false), nrsteps(1)
{
	// TODO: add one-time construction code here

}

CSolarSystemDoc::~CSolarSystemDoc()
{
	StopThread();
}

BOOL CSolarSystemDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)	

	LoadFile(CString(L"SolarSystem.xml"));

	GetMainView()->Setup();

	StartThread();

	return TRUE;
}


CSolarSystemView* CSolarSystemDoc::GetMainView(void)
{
	POSITION pos = GetFirstViewPosition();
	while (pos)
	{
		CView* pView = GetNextView(pos);
		if (pView->IsKindOf(RUNTIME_CLASS(CSolarSystemView)))
			return dynamic_cast<CSolarSystemView*>(pView);
	}

	return NULL;
}


// CSolarSystemDoc serialization

void CSolarSystemDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CSolarSystemDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CSolarSystemDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data. 
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CSolarSystemDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CSolarSystemDoc diagnostics

#ifdef _DEBUG
void CSolarSystemDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSolarSystemDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CSolarSystemDoc commands


HRESULT CSolarSystemDoc::LoadFromXmlString(const CString& str)
{
	if (str.IsEmpty()) return E_FAIL;

	MSXML::IXMLDOMDocumentPtr pDocument;

	HRESULT hResult = pDocument.CreateInstance(__uuidof(DOMDocument));

	if (FAILED(hResult)) return hResult;

	try
	{
		pDocument->async = VARIANT_FALSE;

		if (VARIANT_FALSE == pDocument->loadXML(bstr_t(str)))
			return E_FAIL;

		ParseXmlDocument(pDocument);
	}
	catch (_com_error &e)
	{
		return e.Error();
	}

	return S_OK;
}


void CSolarSystemDoc::ParseXmlDocument(MSXML::IXMLDOMDocumentPtr& pDocument)
{
	m_SolarSystem.m_Bodies.clear();

	{
		MSXML::IXMLDOMNodeListPtr pSolarSystem = pDocument->getElementsByTagName(L"SolarSystem");
		if (pSolarSystem)
		{
			pSolarSystem->reset();
			MSXML::IXMLDOMNodePtr pRecordNode = pSolarSystem->nextNode();
			m_Thread.m_timestep = static_cast<unsigned int>(GetXmlIntValue(pRecordNode, L"TimeStep", 300));
		}
	}

	MSXML::IXMLDOMNodeListPtr pRecordsList = pDocument->getElementsByTagName(L"Body");

	if (pRecordsList)
	{
		pRecordsList->reset();

		MSXML::IXMLDOMNodePtr pRecordNode = pRecordsList->nextNode();

		//for each group in the xml file
		while (pRecordNode)
		{
			m_SolarSystem.m_Bodies.emplace_back(MolecularDynamics::Body());
			m_SolarSystem.m_BodyProperties.emplace_back(BodyProperties());

			LoadBodyXml(pRecordNode, m_SolarSystem.m_Bodies.back(), m_SolarSystem.m_BodyProperties.back());
			
			pRecordNode = pRecordsList->nextNode();
		}
	}

	m_SolarSystem.m_Bodies.shrink_to_fit();
	m_SolarSystem.m_BodyProperties.shrink_to_fit();
}


void CSolarSystemDoc::LoadBodyXml(MSXML::IXMLDOMNodePtr& node, MolecularDynamics::Body& body, BodyProperties& props)
{
	props.isSun = GetXmlBoolValue(node, L"IsSun", false);
	props.isMoon = GetXmlBoolValue(node, L"IsMoon", false);

	props.tilt = GetXmlDoubleValue(node, L"Tilt", 0);
	props.color = RGB(GetXmlIntValue(node,L"R",0), GetXmlIntValue(node, L"G", 0), GetXmlIntValue(node, L"B", 0));

	props.imgFile = GetXmlValue(node, L"skin", L"");
	props.transparentFile = GetXmlValue(node, L"transparentSkin", L"");
	props.shadowFile = GetXmlValue(node, L"shadowSkin", L"");
	props.specularFile = GetXmlValue(node, L"specularSkin", L"");

	props.scale = GetXmlDoubleValue(node, L"Scale", 1.);
	props.scaleDistance = GetXmlDoubleValue(node, L"ScaleDistance", 1.);

	body.rotationPeriod = GetXmlDoubleValue(node, L"RotationPeriod", 1) * 24 * 3600;

	body.m_Mass = GetXmlDoubleValue(node, L"Mass", 0);
	body.m_Radius = GetXmlDoubleValue(node, L"Radius", 0);

	body.m_Position.X = GetXmlDoubleValue(node, L"XPosition", 0);
	body.m_Position.Y = GetXmlDoubleValue(node, L"YPosition", 0);
	body.m_Position.Z = GetXmlDoubleValue(node, L"ZPosition", 0);


	body.m_Velocity.X = GetXmlDoubleValue(node, L"XVelocity", 0);
	body.m_Velocity.Y = GetXmlDoubleValue(node, L"YVelocity", 0);
	body.m_Velocity.Z = GetXmlDoubleValue(node, L"ZVelocity", 0);

}

CString CSolarSystemDoc::GetXmlValue(MSXML::IXMLDOMNodePtr pNode, const bstr_t& name, const CString& def)
{
	CString result(def);

	MSXML::IXMLDOMNodePtr node = pNode->attributes->getNamedItem(name);
	if (node != NULL)
		result = CString((BSTR)node->text);

	return result;
}

bool CSolarSystemDoc::GetXmlBoolValue(MSXML::IXMLDOMNodePtr pNode, const bstr_t& name, bool def)
{
	CString result = GetXmlValue(pNode, name);
	result = result.Trim().MakeLower();
	if (result == L"1" || result == L"yes" || result == L"true") return true;

	return def;
}

double CSolarSystemDoc::GetXmlDoubleValue(MSXML::IXMLDOMNodePtr pNode, const bstr_t& name, double def)
{
	CString result = GetXmlValue(pNode, name);

	result = result.Trim();

	if (result.IsEmpty()) return def;

	return _wtof(result);
}


int CSolarSystemDoc::GetXmlIntValue(MSXML::IXMLDOMNodePtr pNode, const bstr_t& name, int def)
{
	CString result = GetXmlValue(pNode, name);

	result = result.Trim();

	if (result.IsEmpty()) return def;

	return _wtoi(result);
}


void CSolarSystemDoc::LoadXmlFile(const CString& name)
{
	CString Result;

	try {
		CStdioFile File(name, CFile::modeRead | CFile::typeText);
		CString Line;

		while (File.ReadString(Line) == TRUE)
		{
			Result += Line + L"\n";
		}

		LoadFromXmlString(Result);
	}
	catch (...) {}
}


void CSolarSystemDoc::RetrieveData()
{
	m_SolarSystem.m_Bodies = m_Thread.GetBodies();
	m_SolarSystem.m_simulationTime = m_Thread.simulationTime;
}


void CSolarSystemDoc::LoadFile(const CString& fileName)
{
	m_SolarSystem.clear();

	LoadXmlFile(fileName);

	auto pit = m_SolarSystem.m_BodyProperties.begin();
	for (auto it = m_SolarSystem.m_Bodies.begin(); it != m_SolarSystem.m_Bodies.end(); ++it, ++pit)
	{
		glm::mat4 modelMat(1);
		glm::vec3 pos = glm::vec3(it->m_Position.X / AGLU, it->m_Position.Y / AGLU, it->m_Position.Z / AGLU);

		// THIS IS A HACK TO NICELY DISPLAY THE SOLAR SYSTEM 
		// if the moon is inside the planet because of the scaling, the distance from the planet to it is scaled up, too

		if (pit->isMoon && pit->scaleDistance != 1.)
		{

			auto pmit = m_SolarSystem.m_BodyProperties.begin();

			int index = 0;
			for (auto mit = m_SolarSystem.m_Bodies.begin(); mit != m_SolarSystem.m_Bodies.end(); ++mit, ++pmit, ++index)
			{
				if (pmit->isMoon || pmit->isSun) continue; // ignore a collision with another moon or with the sun

				const glm::vec3 mpos = glm::vec3(mit->m_Position.X / AGLU, mit->m_Position.Y / AGLU, mit->m_Position.Z / AGLU);
				const glm::vec3 fromvec = pos - mpos;
				const double dist = glm::length(fromvec);

				if (dist <= 1.05 * (mit->m_Radius * pmit->scale + it->m_Radius * pit->scale) / AGLU)
				{
					pit->parentIndex = index;
					break;
				}
			}
		}
	}
}

void CSolarSystemDoc::StartThread()
{
	m_Thread.SetBodies(m_SolarSystem.m_Bodies);
	m_Thread.StartThread();
}

void CSolarSystemDoc::StopThread()
{
	m_Thread.EndThread();
}
