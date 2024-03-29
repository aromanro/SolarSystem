
// SolarSystemView.h : interface of the CSolarSystemView class
//

#pragma once

#include <GL/glew.h>
#include <gl/GL.h>      // OpenGL Libraries
#include <gl/GLU.h>     // GLU OpenGL Libraries

#include <glm.hpp>

#include <ctime>
#include <chrono>

#include "Camera.h"

#include "Program.h"
#include "SkyBase.h"
#include "ShadowCubeMapProgram.h"
#include "Sphere.h"
#include "Rectangle.h"

#include "SolarSystemBodies.h"
#include "SolarSystemGLProgram.h"

#include "SpaceshipProgram.h"

#include "MemoryBitmap.h"

#include "ObjLoader.h"
#include "ComplexObject.h"

#include "SpaceshipOrientation.h"

class CSolarSystemDoc;


class CSolarSystemView : public CView
{
protected: // create from serialization only
	CSolarSystemView();
	DECLARE_DYNCREATE(CSolarSystemView)

// Attributes
public:
	CSolarSystemDoc* GetDocument() const;

private:
	HGLRC m_hRC;     // Rendering Context
	HDC m_hDC;       // Device Context
	CPalette m_GLPalette;   // Logical Palette
	UINT_PTR timer;
	UINT_PTR slowTimer;

	int Width, Height;

	int wheelAccumulator;

	OpenGL::Camera camera;

	glm::dmat4 perspectiveMatrix;
	glm::dmat4 skyboxPerspectiveMatrix;

	SolarSystemGLProgram *program;

	OpenGL::Sphere *sphere;

	OpenGL::Rectangle* billboardRectangle;
	OpenGL::Texture* billboardTexture;
	MemoryBitmap memoryBitmap;
	CFont font;

	OpenGL::SkyBase *skyProgram;
	OpenGL::ShadowCubeMapProgram *shadowProgram;

	OpenGL::SpaceshipProgram *spaceshipProgram;
	OpenGL::ComplexObjectCompositeMaterials* spaceship = nullptr;

	SpaceshipOrientation spaceshipOrientation;


	BodyPositionList m_BodiesPosition; // interpolated

	BodyPositionList m_NewBodiesPosition; // current step
	BodyPositionList m_OldBodiesPosition; // previous step

	std::chrono::time_point<std::chrono::system_clock> frameTime;

	class Uniforms {
	public:
		Uniforms(SolarSystemBodies& m_SolarSystem, BodyPositionList& m_BodiesPosition, SolarSystemGLProgram& program, unsigned int nrlights);
	};

	class ShadowUniforms {
	public:
		ShadowUniforms(SolarSystemBodies& m_SolarSystem, BodyPositionList& m_BodiesPosition, OpenGL::ShadowCubeMapProgram& program, unsigned int nrlights);
	};
	
	class SpaceshipUniforms {
	public:
		SpaceshipUniforms(SolarSystemBodies& m_SolarSystem, BodyPositionList& m_BodiesPosition, OpenGL::SpaceshipProgram& program, unsigned int nrlights);
	};

	
	bool keyDown;
	bool ctrl;
	bool shift;
	OpenGL::Camera::Movements movement;

	bool inited;

	void InitializePalette(void);

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CSolarSystemView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
// Generated message map functions
	DECLARE_MESSAGE_MAP()

private:
	void RenderScene();
	void RenderPlanet(const BodyPositionList::iterator& it, const BodyPropList::iterator& pit, const BodyList::iterator& bit);
	void SetTextures(const BodyPropList::iterator& pit);

	void RenderShadowScene();
	void RenderSky();

	void RenderSpaceship(glm::mat4& mat);

	void Resize(GLsizei h, GLsizei w);
	
	void MoonHack(const BodyList::iterator& bit, const BodyPropList::iterator& pit, glm::dvec3& pos);

	Vector3D<double> GetTowardsVector(CPoint& point, const Vector3D<double>& forward);

	bool SetupShaders();
	bool SetupSkyBox();
	bool SetupShadows();
	bool SetupSpaceship();


public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnQueryNewPalette();
	afx_msg void OnPaletteChanged(CWnd* pFocusWnd);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	bool KeyPressHandler(MSG* pMsg);
	afx_msg void OnTimer(UINT_PTR nIDEvent);


	void Setup();
	void Reset();

	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	void ClearProgram();
	void ClearSkyProgram();
	void ClearShadowProgram();
	void ClearSpaceshipProgram();

	void SetSpeeds(double translate, double rotate);
	void EnableAntialias();
	void DisableAntialias();
	void DisplayBilboard();
	void SetBillboardText(const char* text);

protected:
	bool HandleKeyPress(WPARAM wParam);
	void HandleUp();
	void HandleDown();
	void HandleLeft();
	void HandleRight();

	void OnRedrawSceneTimer(CSolarSystemDoc* doc);
	void OnUpdateBillboardTimer(CSolarSystemDoc* doc);
};

#ifndef _DEBUG  // debug version in SolarSystemView.cpp
inline CSolarSystemDoc* CSolarSystemView::GetDocument() const
   { return reinterpret_cast<CSolarSystemDoc*>(m_pDocument); }
#endif

