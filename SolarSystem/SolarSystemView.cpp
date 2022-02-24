
// SolarSystemView.cpp : implementation of the CSolarSystemView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "SolarSystem.h"
#endif

#include "SolarSystemDoc.h"
#include "SolarSystemView.h"
#include "MainFrm.h"
#include "MFCToolBarSlider.h"

#include "Constants.h"

#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CSolarSystemView

IMPLEMENT_DYNCREATE(CSolarSystemView, CView)

BEGIN_MESSAGE_MAP(CSolarSystemView, CView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_QUERYNEWPALETTE()
	ON_WM_PALETTECHANGED()
	ON_WM_TIMER()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

// CSolarSystemView construction/destruction

CSolarSystemView::CSolarSystemView()
	: timer(NULL), slowTimer(NULL), program(NULL), sphere(NULL), billboardRectangle(NULL), billboardTexture(NULL), inited(false), skyProgram(NULL), shadowProgram(NULL), spaceshipProgram(NULL),
	keyDown(false), ctrl(false), shift(false), wheelAccumulator(0),
	movement(OpenGL::Camera::Movements::noMove), m_hRC(0), m_hDC(0),
	Width(0), Height(0)
{
}

CSolarSystemView::~CSolarSystemView()
{
	delete sphere;
	delete billboardRectangle;
	delete billboardTexture;
	delete spaceship;

	ClearProgram();
	ClearShadowProgram();
	ClearSkyProgram();
	ClearSpaceshipProgram();
}



void CSolarSystemView::Resize(GLsizei h, GLsizei w)
{
	if (h == 0) return;

	glViewport(0, 0, w, h);

	const double ar = static_cast<double>(w) / h;
	perspectiveMatrix = glm::perspective(cameraAngle, ar, nearPlaneDistance, farPlaneDistance);
	skyboxPerspectiveMatrix = glm::perspective(skyboxAngle, ar, nearPlaneDistance, farPlaneDistance);
}

BOOL CSolarSystemView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= (WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	cs.style &= ~WS_BORDER;
	if (!CView::PreCreateWindow(cs)) return FALSE;

	WNDCLASS wndcls;
	HINSTANCE hInst = AfxGetInstanceHandle();

	const wchar_t *className = L"OpenGLClass";
	if (!(::GetClassInfo(hInst, className, &wndcls)))
	{
		if (::GetClassInfo(hInst, cs.lpszClass, &wndcls))
		{
			wndcls.lpszClassName = className;
			wndcls.style |= CS_OWNDC;
			wndcls.hbrBackground = NULL;

			if (!AfxRegisterClass(&wndcls))
				return FALSE;
		}
		else return FALSE;
	}

	return TRUE;
}

// CSolarSystemView drawing

void CSolarSystemView::OnDraw(CDC* /*pDC*/)
{
	if (inited)
	{
		CSolarSystemDoc* doc = GetDocument();
		if (doc)
		{
			// interpolate between two frames
			auto curTime = std::chrono::system_clock::now();
			if (!doc->stopped && !m_NewBodiesPosition.empty())
			{
				const long long int msDifference = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - frameTime).count();

				const double alpha = static_cast<double>(msDifference) / msFrame;
				const double onemalpha = 1. - alpha;

				m_BodiesPosition.resize(m_NewBodiesPosition.size());
				for (int i = 0; i < m_NewBodiesPosition.size(); ++i)
				{
					m_BodiesPosition[i].m_Position = onemalpha * m_OldBodiesPosition[i].m_Position + alpha * m_NewBodiesPosition[i].m_Position;
					m_BodiesPosition[i].rotation = onemalpha * m_OldBodiesPosition[i].rotation + alpha * m_NewBodiesPosition[i].rotation;
				}
			}
			else
				frameTime = curTime;
		}

		wglMakeCurrent(m_hDC, m_hRC);

		if (theApp.options.drawShadows) RenderShadowScene();
		
		glDrawBuffer(GL_BACK);

		Resize(Height, Width);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// render the skybox first otherwise there will be troubles with alpha blending if the scene renders a billboard
		if (theApp.options.showSkyBox) RenderSky();

		RenderScene();

		//glFlush(); // not really needed, SwapBuffers should take care of things
		SwapBuffers(m_hDC);
		wglMakeCurrent(NULL, NULL);
	}
}



// CSolarSystemView diagnostics

#ifdef _DEBUG
void CSolarSystemView::AssertValid() const
{
	CView::AssertValid();
}

void CSolarSystemView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSolarSystemDoc* CSolarSystemView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSolarSystemDoc)));
	return dynamic_cast<CSolarSystemDoc*>(m_pDocument);
}
#endif //_DEBUG


// CSolarSystemView message handlers


int CSolarSystemView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_hDC = ::GetDC(m_hWnd);

	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		24, // 24-bit color depth  
		0,0,0,0,0,0, // color bits ignored
		0, // no alpha buffer 
		0, // shift bit ignored  
		0, // no accumulation buffer 
		0,0,0,0, // accum bits ignored  				
		32, // 32-bit z-buffer   
		0, // no stencil buffer
		0, // no auxiliary buffer 
		PFD_MAIN_PLANE/*PFD_OVERLAY_PLANE*/, // main layer
		0, // reserved  
		0,0,0 // layer masks ignored  
	};

	const int nPixelFormat = ChoosePixelFormat(m_hDC, &pfd);
	SetPixelFormat(m_hDC, nPixelFormat, &pfd);

	// aprox 60 frames/sec
	timer = SetTimer(1, msFrame, NULL);

	slowTimer = SetTimer(2, 1000, NULL);

	return 0;
}


void CSolarSystemView::OnDestroy()
{
	if (inited)
	{
		wglDeleteContext(m_hRC);
		::ReleaseDC(m_hWnd, m_hDC);

		KillTimer(slowTimer);
		KillTimer(timer);
	}

	CView::OnDestroy();
}


void CSolarSystemView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if (inited)
	{
		Height = cy;
		Width = cx;
	}
}


BOOL CSolarSystemView::OnEraseBkgnd(CDC* /*pDC*/)
{
	return FALSE;
}


// Initializes the CPalette object
void CSolarSystemView::InitializePalette(void)
{
	PIXELFORMATDESCRIPTOR pfd;
	LOGPALETTE *pPal = NULL;
	BYTE RedRng = 0, GreenRng = 0, BlueRng = 0;

	int nPixelFormat = GetPixelFormat(m_hDC);
	DescribePixelFormat(m_hDC, nPixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

	if (!(pfd.dwFlags & PFD_NEED_PALETTE)) return;

	WORD nColors = static_cast<WORD>(1 << pfd.cColorBits);

	pPal = (LOGPALETTE*)malloc(sizeof(LOGPALETTE) + nColors * sizeof(PALETTEENTRY));

	if (pPal)
	{
		pPal->palVersion = 0x300;
		pPal->palNumEntries = nColors;

		RedRng = static_cast<BYTE>((1 << pfd.cRedBits) - 1);
		GreenRng = static_cast<BYTE>((1 << pfd.cGreenBits) - 1);
		BlueRng = static_cast<BYTE>((1 << pfd.cBlueBits) - 1);

		for (unsigned int i = 0; i < nColors; i++)
		{
			pPal->palPalEntry[i].peRed = static_cast<BYTE>((i >> pfd.cRedShift) & RedRng);
			pPal->palPalEntry[i].peRed = static_cast<unsigned char>(pPal->palPalEntry[i].peRed * 255.0 / RedRng);
			pPal->palPalEntry[i].peGreen = static_cast<BYTE>((i >> pfd.cGreenShift) & GreenRng);
			pPal->palPalEntry[i].peGreen = static_cast<unsigned char>(pPal->palPalEntry[i].peGreen * 255.0 / GreenRng);
			pPal->palPalEntry[i].peBlue = static_cast<BYTE>((i >> pfd.cBlueShift) & BlueRng);
			pPal->palPalEntry[i].peBlue = static_cast<unsigned char>(pPal->palPalEntry[i].peBlue * 255.0 / BlueRng);
			pPal->palPalEntry[i].peFlags = static_cast<unsigned char>(NULL);
		}

		m_GLPalette.CreatePalette(pPal);
		SelectPalette(m_hDC, (HPALETTE)m_GLPalette, FALSE);
		RealizePalette(m_hDC);

		free(pPal);
	}
}


BOOL CSolarSystemView::OnQueryNewPalette()
{
	if ((HPALETTE)m_GLPalette)
	{
		SelectPalette(m_hDC, (HPALETTE)m_GLPalette, FALSE);
		const unsigned int nRet = RealizePalette(m_hDC);
		InvalidateRect(NULL, FALSE);

		return nRet ? TRUE : FALSE;
	}

	return CView::OnQueryNewPalette();
}


void CSolarSystemView::OnPaletteChanged(CWnd* pFocusWnd)
{
	if (((HPALETTE)m_GLPalette != NULL) && (pFocusWnd != this))
	{
		SelectPalette(m_hDC, (HPALETTE)m_GLPalette, FALSE);
		RealizePalette(m_hDC);
		UpdateColors(m_hDC);
		return;
	}

	CView::OnPaletteChanged(pFocusWnd);
}


BOOL CSolarSystemView::PreTranslateMessage(MSG* pMsg)
{
	if (!KeyPressHandler(pMsg)) return CView::PreTranslateMessage(pMsg);

	return TRUE;
}


bool CSolarSystemView::KeyPressHandler(MSG* pMsg)
{
	bool handled = false;

	if (pMsg->message == WM_KEYDOWN)
	{
		keyDown = true;
		ctrl = ((::GetKeyState(VK_CONTROL) & 0x8000) != 0 ? true : false);
		shift = ((::GetKeyState(VK_SHIFT) & 0x8000) != 0 ? true : false);

		// pMSG->wParam contains the key code
		handled = HandleKeyPress(pMsg->wParam);

		Invalidate(0);
	}
	else if (pMsg->message == WM_KEYUP) keyDown = false;

	return handled;
}


bool CSolarSystemView::HandleKeyPress(WPARAM wParam)
{
	bool handled = false;

	switch (wParam)
	{
	case VK_UP:
		HandleUp();
		handled = true;
		break;
	case VK_DOWN:
		HandleDown();
		handled = true;
		break;
	case VK_LEFT:
		HandleLeft();
		handled = true;
		break;
	case VK_RIGHT:
		HandleRight();
		handled = true;
		break;
	case VK_ADD:
	case VK_OEM_PLUS:
	{
		CSolarSystemDoc* doc = GetDocument();
		if (doc->nrsteps < MAX_NRSTEPS)
		{
			++doc->nrsteps;
			CMFCToolBarSlider::SetPos(ID_SLIDER, (int)doc->nrsteps);
		}
	}
	handled = true;
	break;
	case VK_SUBTRACT:
	case VK_OEM_MINUS:
	{
		CSolarSystemDoc* doc = GetDocument();
		if (doc->nrsteps > 1)
		{
			--doc->nrsteps;
			CMFCToolBarSlider::SetPos(ID_SLIDER, (int)doc->nrsteps);
		}
	}
	handled = true;
	break;
	case VK_SPACE:
	{
		CMainFrame* frame = dynamic_cast<CMainFrame*>(theApp.m_pMainWnd);
		frame->OnSimulateRun();
	}
	handled = true;
	break;
	default:
		movement = OpenGL::Camera::Movements::noMove;
	}

	return handled;
}

void CSolarSystemView::HandleUp()
{
	if (ctrl) movement = OpenGL::Camera::Movements::pitchUp;
	else if (shift) movement = OpenGL::Camera::Movements::moveUp;
	else movement = OpenGL::Camera::Movements::moveForward;

	if(ctrl || shift) spaceshipOrientation.RotateUp();
}

void CSolarSystemView::HandleDown()
{
	if (ctrl) movement = OpenGL::Camera::Movements::pitchDown;
	else if (shift) movement = OpenGL::Camera::Movements::moveDown;
	else movement = OpenGL::Camera::Movements::moveBackward;
	
	if (ctrl || shift) spaceshipOrientation.RotateDown();
}

void CSolarSystemView::HandleLeft()
{
	if (ctrl) movement = OpenGL::Camera::Movements::yawLeft;
	else if (shift) movement = OpenGL::Camera::Movements::rollLeft;
	else movement = OpenGL::Camera::Movements::moveLeft;

	if (shift) spaceshipOrientation.RollLeft();
	else spaceshipOrientation.RotateLeft();
}

void CSolarSystemView::HandleRight()
{
	if (ctrl) movement = OpenGL::Camera::Movements::yawRight;
	else if (shift) movement = OpenGL::Camera::Movements::rollRight;
	else movement = OpenGL::Camera::Movements::moveRight;

	if (shift) spaceshipOrientation.RollRight(); 
	else spaceshipOrientation.RotateRight();
}


void CSolarSystemView::OnTimer(UINT_PTR nIDEvent)
{
	if (inited)
	{
		CSolarSystemDoc* doc = GetDocument();
		if (doc)
		{
			if (1 == nIDEvent)
			{
				OnRedrawSceneTimer(doc);
			}
			else
			{
				OnUpdateBillboardTimer(doc);
			}

			RedrawWindow(0, 0, RDW_INTERNALPAINT | RDW_NOERASE | RDW_NOFRAME | RDW_UPDATENOW);
		}
	}

	CView::OnTimer(nIDEvent);
}

void CSolarSystemView::OnRedrawSceneTimer(CSolarSystemDoc* doc)
{
	bool firstTime = false;
	if (m_NewBodiesPosition.empty())
	{
		firstTime = true;
		// first time called
		frameTime = std::chrono::system_clock::now();

		if (doc->RetrieveData())
		{
			m_OldBodiesPosition.swap(doc->m_SolarSystem.m_BodiesPosition);
			m_NewBodiesPosition = m_OldBodiesPosition;
		}
	}

	if (!firstTime)
	{
		auto curTime = std::chrono::system_clock::now();
		long long int msDifference = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - frameTime).count();

		int count = 0; // this counter is here just to ensure it won't stay in this while for too much time (just in case)
		while (msDifference >= msFrame / 2 && ++count < 10)
		{
			if (doc->stopped) break;

			if (doc->RetrieveData())
			{
				msDifference -= msFrame;
				frameTime += std::chrono::milliseconds(msFrame);

				m_OldBodiesPosition.swap(m_NewBodiesPosition);
				m_NewBodiesPosition.swap(doc->m_SolarSystem.m_BodiesPosition);
			}
			else
				std::this_thread::sleep_for(std::chrono::milliseconds(1));

			curTime = std::chrono::system_clock::now();
			msDifference = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - frameTime).count();
		}
	}

	camera.Tick();
	if (keyDown) camera.Move(movement);

	spaceshipOrientation.Tick();
}

void CSolarSystemView::OnUpdateBillboardTimer(CSolarSystemDoc* doc)
{
	const unsigned long long int seconds = static_cast<unsigned long long int>(doc->m_SolarSystem.m_simulationTime);
	unsigned long long int hours = seconds / 3600;
	unsigned long long int days = hours / 24;
	const unsigned long long int years = days / 365;
	days %= 365;
	hours %= 24;

	std::string str;
	if (years)
	{
		str = std::to_string(years);
		if (1 == years)
			str += " year";
		else
			str += " years";
	}

	if (days)
	{
		if (str.size())
			str += " ";

		str += std::to_string(days);
		if (1 == days)
			str += " day";
		else
			str += " days";
	}

	if (hours)
	{
		if (str.size())
			str += " ";

		str += std::to_string(hours);
		if (1 == hours)
			str += " hour";
		else
			str += " hours";
	}

	SetBillboardText(str.c_str());
}



BOOL CSolarSystemView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	wheelAccumulator += zDelta;

	int distanceTicks = abs(wheelAccumulator);

	if (distanceTicks >= WHEEL_DELTA) {
		distanceTicks /= WHEEL_DELTA;
		const bool forward = (wheelAccumulator > 0 ? true : false);
		wheelAccumulator %= WHEEL_DELTA;

		camera.ProgressiveMove(forward ? OpenGL::Camera::Movements::moveForward : OpenGL::Camera::Movements::moveBackward, distanceTicks, true);
	}

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}



Vector3D<double> CSolarSystemView::GetTowardsVector(CPoint& point, const Vector3D<double>& forward)
{
	CRect rect;
	GetClientRect(rect);

	const double pixelSize = 2.*nearPlaneDistance*tan(cameraAngle / 2.) / rect.Height();

	point.x -= rect.Width() / 2;
	point.y -= rect.Height() / 2;
	point.y = -point.y;

	const Vector3D<double> right = (forward % camera.up).Normalize();
	const Vector3D<double> up = (right % forward).Normalize();

	return (nearPlaneDistance * forward + pixelSize * point.x * right + pixelSize * point.y * up).Normalize();
}


void CSolarSystemView::OnLButtonDown(UINT nFlags, CPoint point)
{
	const Vector3D<double> forward = camera.getNormalizedForward();
	const Vector3D<double> up = camera.getNormalizedUp();
	const Vector3D<double> left = up % forward;

	const Vector3D<double> towards = GetTowardsVector(point, forward);

	const double frontComponent = towards * forward;
	const double angle = acos(frontComponent);
	const int numticks = static_cast<int>(angle / camera.GetRotateAngle());

	camera.RotateTowards(angle - numticks * camera.GetRotateAngle(), towards);
	camera.ProgressiveRotate(towards, numticks);

	// do something with the spaceship as well
	spaceshipOrientation.ComputeRotations();

	const double upComponent = towards * up;
	const double leftComponent = towards * left;

	Vector3D<double> verticalPlaneVector(frontComponent, upComponent, 0.);
	verticalPlaneVector = verticalPlaneVector.Normalize();
	const double pitchAngle = asin(verticalPlaneVector.Y) / M_PI * 180.;
	spaceshipOrientation.RotateUpDown(pitchAngle);

	Vector3D<double> cameraPlaneVector(upComponent, leftComponent, 0.);
	cameraPlaneVector = cameraPlaneVector.Normalize();
	const double rollAngle = asin(cameraPlaneVector.Y) / M_PI * 180.;
	spaceshipOrientation.RollLeftRight(rollAngle / 2.); // don't roll so hard

	Vector3D<double> horizontalPlaneVector(frontComponent, leftComponent, 0.);
	horizontalPlaneVector = horizontalPlaneVector.Normalize();
	const double yawAngle = asin(horizontalPlaneVector.Y) / M_PI * 180.;
	spaceshipOrientation.RotateLeftRight(yawAngle);

	CView::OnLButtonDown(nFlags, point);
}


void CSolarSystemView::ClearProgram()
{
	delete program;
	program = NULL;
}


void CSolarSystemView::ClearSkyProgram()
{
	delete skyProgram;
	skyProgram = NULL;
}


void CSolarSystemView::ClearShadowProgram()
{
	delete shadowProgram;
	shadowProgram = NULL;
}

void CSolarSystemView::ClearSpaceshipProgram()
{
	delete spaceshipProgram;
	spaceshipProgram = NULL;
}

void CSolarSystemView::SetSpeeds(double translate, double rotate)
{
	camera.SetSpeeds(translate, rotate);
}


void CSolarSystemView::SetBillboardText(const char* text)
{
	if (!billboardTexture) return;
	
	memoryBitmap.WriteText(text, font);

	wglMakeCurrent(m_hDC, m_hRC);
	memoryBitmap.SetIntoTexture(*billboardTexture);
	wglMakeCurrent(NULL, NULL);
}
