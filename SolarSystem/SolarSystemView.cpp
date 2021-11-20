
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

#include "SolarSystem.h"

#include <GL\glew.h>
#include <gl\gl.h>      // OpenGL Libraries
#include <gl\glu.h>     // GLU OpenGL Libraries


#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>


#include "MatrixPush.h"

#include "Cube.h"

#include "SkyBoxCubeMapProgram.h"
#include "SkySphereProgram.h"

#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static const double lightCoeff = 0.000001;


CSolarSystemView::Uniforms::Uniforms(SolarSystemBodies& m_SolarSystem, SolarSystemGLProgram& program, unsigned int nrlights)
{
	// lights

	if (nrlights == 0)
	{
		program.lights[0].lightPos = glm::vec3(-farPlaneDistance, -farPlaneDistance, 0);
	}
	else
	{
		unsigned int light = 0;

		auto pit = m_SolarSystem.m_BodyProperties.begin();
		for (auto it = m_SolarSystem.m_Bodies.begin(); it != m_SolarSystem.m_Bodies.end(); ++it, ++pit)
		{
			if (pit->isSun)
			{
				program.lights[light].lightPos = glm::vec3(it->m_Position.X / AGLU, it->m_Position.Y / AGLU, it->m_Position.Z / AGLU);

				++light;
			}
		}
	}

	glUniform1f(program.farPlaneLoc, (float)farPlaneDistance);

	glUniform1i(program.textureLoc, 0);
	glUniform1i(program.transparentTextureLoc, 1);
	glUniform1i(program.shadowTextureLoc, 2);
	glUniform1i(program.specularTextureLoc, 3);
	glUniform1i(program.normalTextureLoc, 4);

	// this is for shadow
	glUniform1i(program.depthMapLoc, 10);
	glUniform1i(program.calcShadowsLoc, theApp.options.drawShadows ? 1 : 0);
	glUniform3f(program.lightPosLoc, static_cast<float>(program.lights[0].lightPos.x), static_cast<float>(program.lights[0].lightPos.y), static_cast<float>(program.lights[0].lightPos.z));
}



CSolarSystemView::SpaceshipUniforms::SpaceshipUniforms(SolarSystemBodies& m_SolarSystem, OpenGL::SpaceshipProgram& program, unsigned int nrlights)
{
	// lights

	if (nrlights == 0)
	{
		program.lights[0].lightPos = glm::vec3(-farPlaneDistance, -farPlaneDistance, 0);
	}
	else
	{
		unsigned int light = 0;

		auto pit = m_SolarSystem.m_BodyProperties.begin();
		for (auto it = m_SolarSystem.m_Bodies.begin(); it != m_SolarSystem.m_Bodies.end(); ++it, ++pit)
		{
			if (pit->isSun)
			{
				program.lights[light].lightPos = glm::vec3(it->m_Position.X / AGLU, it->m_Position.Y / AGLU, it->m_Position.Z / AGLU);

				++light;
			}
		}
	}


	glUniform1i(program.ambientTextureLocation, 0);
	glUniform1i(program.diffuseTextureLocation, 1);
	glUniform1i(program.specularTextureLocation, 2);
	glUniform1i(program.exponentTextureLocation, 3);
	glUniform1i(program.bumpTextureLocation, 4);
}


CSolarSystemView::ShadowUniforms::ShadowUniforms(SolarSystemBodies& m_SolarSystem, OpenGL::ShadowCubeMapProgram& program, unsigned int nrlights)
{
	// light
	glm::vec3 lightPos;

	if (nrlights == 0) lightPos = glm::vec3(-farPlaneDistance, -farPlaneDistance, 0);
	else {
		auto pit = m_SolarSystem.m_BodyProperties.begin();
		for (auto it = m_SolarSystem.m_Bodies.begin(); it != m_SolarSystem.m_Bodies.end(); ++it, ++pit)
		{
			if (pit->isSun)
			{
				lightPos = glm::vec3(it->m_Position.X / AGLU, it->m_Position.Y / AGLU, it->m_Position.Z / AGLU);

				break;
			}
		}
	}

	program.SetLightAndFarPlanePosition(lightPos);
}


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
#ifdef DISPLAY_SPACESHIP
	//delete triangle;
#endif	
	delete spaceship;

	ClearProgram();
	ClearShadowProgram();
	ClearSkyProgram();
	ClearSpaceshipProgram();
}



bool CSolarSystemView::SetupShaders()
{
	program = new SolarSystemGLProgram();

	CSolarSystemDoc *doc = GetDocument();
	unsigned int nrlights = 0;

	for (const auto &body : doc->m_SolarSystem.m_BodyProperties)
		if (body.isSun) ++nrlights;

	return program->SetupShaders(nrlights);
}


bool CSolarSystemView::SetupSkyBox()
{
	if (!theApp.options.showSkyBox) return true;

	if (NULL == skyProgram)
	{
		OpenGL::SkySphereProgram* skySphereProgram = new OpenGL::SkySphereProgram();
		skyProgram = skySphereProgram;
		if (!skySphereProgram->SetShaders())
		{
			ClearSkyProgram();
		}
		else if (skySphereProgram->getStatus() == false)
		{
			AfxMessageBox(CString("SkySphere CubeMap compile: ") + CString(skySphereProgram->getStatusMessage()));
			ClearSkyProgram();
		}
		else if (!skySphereProgram->LoadTexture())
		{
			ClearSkyProgram();
		}

		if (skyProgram)
		{
			skyProgram->DetachShaders();
			return true;
		}
	}

	if (NULL == skyProgram)
	{
		OpenGL::SkyBoxCubeMapProgram *skyBoxProgram = new OpenGL::SkyBoxCubeMapProgram();
		skyProgram = skyBoxProgram;
		if (!skyBoxProgram->SetShaders())
		{
			ClearSkyProgram();

			return false;
		}
		else if (skyBoxProgram->getStatus() == false)
		{
			AfxMessageBox(CString("SkyBox CubeMap compile: ") + CString(skyBoxProgram->getStatusMessage()));
			ClearSkyProgram();

			return false;
		}
		else if (!skyBoxProgram->LoadTextures())
		{
			ClearSkyProgram();

			return false;
		}

		skyProgram->DetachShaders();
	}

	return true;
}

bool CSolarSystemView::SetupShadows()
{
	if (!theApp.options.drawShadows) return true;

	if (NULL == shadowProgram)
	{
		shadowProgram = new OpenGL::ShadowCubeMapProgram();
		if (!shadowProgram->SetShaders())
		{
			ClearShadowProgram();

			return false;
		}


		if (!shadowProgram->getStatus())
		{
			AfxMessageBox(CString("Shadow CubeMap compile: ") + CString(shadowProgram->getStatusMessage()));
			ClearShadowProgram();

			return false;
		}

		shadowProgram->DetachShaders();
	}

	return true;
}



bool CSolarSystemView::SetupSpaceship()
{
	if (spaceship == NULL) return false;

	if (NULL == spaceshipProgram)
	{
		if (NULL == program) return false;

		spaceshipProgram = new OpenGL::SpaceshipProgram();
		if (!spaceshipProgram->SetShaders(program->nrlights))
		{
			ClearSpaceshipProgram();

			return false;
		}
	
		if (!spaceshipProgram->getStatus())
		{
			AfxMessageBox(CString("Shadow CubeMap compile: ") + CString(spaceshipProgram->getStatusMessage()));
			ClearShadowProgram();

			return false;
		}

		spaceshipProgram->DetachShaders();

		CSolarSystemDoc* doc = GetDocument();
		if (doc) spaceshipProgram->SetupLights(doc->m_SolarSystem.m_BodyProperties);
	}

	return true;
}

void CSolarSystemView::Setup()
{
	if (inited) return;

	if (!m_hRC)
	{
		m_hRC = wglCreateContext(m_hDC);
		VERIFY(wglMakeCurrent(m_hDC, m_hRC));

		glewInit();
		
		glClearColor(0, 0, 0, 0);

		EnableAntialias();

		glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_DEPTH_RANGE);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LEQUAL);

		glShadeModel(GL_SMOOTH);


		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		if (theApp.options.gammaCorrection) glEnable(GL_FRAMEBUFFER_SRGB);

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		
		glPolygonMode(GL_FRONT, GL_FILL);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glMatrixMode(GL_MODELVIEW);

		CRect rect;
		GetClientRect(rect);
		Height = rect.Height();
		Width = rect.Width();
	}
	else {
		wglMakeCurrent(m_hDC, m_hRC);

		if (theApp.options.gammaCorrection) glEnable(GL_FRAMEBUFFER_SRGB);
		else glDisable(GL_FRAMEBUFFER_SRGB);
	}

	CSolarSystemDoc *doc = GetDocument();

	sphere = new OpenGL::Sphere();
	
	
	const int billboardAspectRatio = 16;
	billboardRectangle = new OpenGL::Rectangle(billboardAspectRatio);

	const int height = 128;
	memoryBitmap.SetSize(static_cast<int>(billboardAspectRatio * height), height);

	if (!font.GetSafeHandle())
	{
		const int fontSize = static_cast<int>(height * 0.8);
		const int fontHeight = -MulDiv(fontSize, CDC::FromHandle(::GetDC(NULL))->GetDeviceCaps(LOGPIXELSY), 72);
		font.CreateFont(fontHeight, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_MODERN, _T("Courier New"));
	}

	billboardTexture = new OpenGL::Texture();
	

#ifdef DISPLAY_SPACESHIP
	//const int translateDown = 0;
	//triangle = new OpenGL::OpenGLTriangle(Vector3D<double>(-5., 5. - translateDown, 0), Vector3D<double>(-5., -5. - translateDown, 0), Vector3D<double>(5., 0 - translateDown, 0));

	if (!theApp.options.spaceshipObjFile.IsEmpty())
	{
		ObjLoader objLoader;
		CT2CA convAnsiString(theApp.options.spaceshipObjFile);
		std::string spaceshipFile(convAnsiString);
		if (objLoader.Load(spaceshipFile))
		{
			spaceship = new OpenGL::ComplexObjectCompositeMaterials(objLoader);
		}
	}

#endif


	if (!SetupShaders()) {
		ClearProgram();
		return;
	}

	program->DetachShaders();

	program->SetupLights(doc->m_SolarSystem.m_BodyProperties);

	if (theApp.options.drawTextures) doc->m_SolarSystem.LoadTextures();

	SetupSkyBox();

	SetupShadows();

	if (spaceship) SetupSpaceship();

	wglMakeCurrent(NULL, NULL);

	SetBillboardText("");

	inited = true;
}

void CSolarSystemView::MoonHack(const BodyPropList::iterator& pit, const BodyList::iterator& it, glm::dvec3& pos)
{
	CSolarSystemDoc *doc = GetDocument();

	if (-1 == pit->parentIndex)
		return;

	const MolecularDynamics::Body& parent = doc->m_SolarSystem.m_Bodies[pit->parentIndex];
	const BodyProperties& parentProps = doc->m_SolarSystem.m_BodyProperties[pit->parentIndex];

	// should not happen, but be safe
	if (parentProps.isMoon || parentProps.isSun) 
		return;

	glm::dvec3 mpos(parent.m_Position.X / AGLU, parent.m_Position.Y / AGLU, parent.m_Position.Z / AGLU);
	const glm::dvec3 fromvec = pos - mpos;
	const double dist = glm::length(fromvec);

	if (dist <= (parent.m_Radius * parentProps.scale + it->m_Radius * pit->scale) / AGLU)
		pos = mpos + glm::dvec3(fromvec.x * pit->scaleDistance, fromvec.y * pit->scaleDistance, fromvec.z * pit->scaleDistance);
}

void CSolarSystemView::RenderScene()
{
	if (NULL == program) return;

	CSolarSystemDoc *doc = GetDocument();
	if (!doc) return;

	glm::mat4 mat(perspectiveMatrix * (glm::dmat4)camera);

	program->Use();

	if (shadowProgram) shadowProgram->depthCubemap.Bind();

	Uniforms params(doc->m_SolarSystem, *program, program->nrlights);

	glUniform3f(program->viewPosLocation, static_cast<float>(camera.eyePos.X / AGLU), static_cast<float>(camera.eyePos.Y / AGLU), static_cast<float>(camera.eyePos.Z / AGLU));
	glUniformMatrix4fv(program->matLocation, 1, GL_FALSE, value_ptr(mat));

	glUniform1i(program->useAlphaBlend, 0);

	auto pit = doc->m_SolarSystem.m_BodyProperties.begin();	

	for (auto it = doc->m_SolarSystem.m_Bodies.begin(); it != doc->m_SolarSystem.m_Bodies.end(); ++it, ++pit)
	{
		glm::dvec3 pos(it->m_Position.X / AGLU, it->m_Position.Y / AGLU, it->m_Position.Z / AGLU);


		// THIS IS A HACK TO NICELY DISPLAY THE SOLAR SYSTEM 
		// if the moon is inside the planet because of the scaling, the distance from the planet to it is scaled up, too

		if (pit->isMoon && pit->scaleDistance != 1.) MoonHack(pit, it, pos);

		// ****************************************************************************************************************************

		const double scale = it->m_Radius * pit->scale / AGLU;
		const glm::dmat4 modelMatHP = glm::rotate(glm::rotate(glm::scale(glm::translate(glm::dmat4(1), pos), glm::dvec3(scale, scale, scale)), pit->tilt * M_PI / 180., glm::dvec3(0, 1, 0)), it->rotation, glm::dvec3(0, 0, 1));

		const glm::mat4 modelMat(modelMatHP);
		const glm::mat3 transpInvModelMat(glm::transpose(glm::inverse(modelMatHP)));

		glUniformMatrix4fv(program->modelMatLocation, 1, GL_FALSE, value_ptr(modelMat));
		glUniformMatrix3fv(program->transpInvModelMatLocation, 1, GL_FALSE, value_ptr(transpInvModelMat));
		glUniform1i(program->isSunLocation, pit->isSun ? 1 : 0);
		
		if (!pit->isSun)
		{
			for (unsigned int i = 0; i < (program->nrlights == 0 ? 1 : program->nrlights); ++i)
			{
				glm::dvec3 lightDir = program->lights[i].lightPos - pos;

				// linear instead of quadratic and using a coefficient that provides enough light even for Pluto
				const float atten = static_cast<float>(1. / (1. + lightCoeff * glm::length(lightDir)));

				lightDir = glm::normalize(lightDir);
				glUniform3f(program->lights[i].lightDirPos, static_cast<float>(lightDir.x), static_cast<float>(lightDir.y), static_cast<float>(lightDir.z));
				glUniform1f(program->lights[i].attenPos, atten);
			}
		}

		if (theApp.options.drawTextures)
		{
			if (pit->texture)
			{
				pit->texture->Bind();
				glUniform1i(program->useTextLocation, 1);
			}
			else
			{
				glUniform1i(program->useTextLocation, 0);
				glUniform4f(program->colorLocation, static_cast<float>(GetRValue(pit->color) / 255.), static_cast<float>(GetGValue(pit->color) / 255.), static_cast<float>(GetBValue(pit->color) / 255.), 1.);
			}

			if (pit->transparentTexture)
			{
				pit->transparentTexture->Bind(1);
				glUniform1i(program->useTransparentTextLocation, 1);

				if (pit->transparentTextureAlpha)
					glUniform1i(program->alphaInTransparentTexture, 1);
				else
					glUniform1i(program->alphaInTransparentTexture, 0);
			}
			else
			{
				glUniform1i(program->useTransparentTextLocation, 0);
				glUniform1i(program->alphaInTransparentTexture, 0);
			}

			if (pit->shadowTexture)
			{
				pit->shadowTexture->Bind(2);
				glUniform1i(program->useShadowTextLocation, 1);
			}
			else
				glUniform1i(program->useShadowTextLocation, 0);


			if (pit->specularTexture)
			{
				pit->specularTexture->Bind(3);
				glUniform1i(program->useSpecularTextLocation, 1);
			}
			else
				glUniform1i(program->useSpecularTextLocation, 0);

			if (pit->normalTexture)
			{
				pit->normalTexture->Bind(4);
				glUniform1i(program->useNormalTextLocation, 1);
			}
			else
				glUniform1i(program->useNormalTextLocation, 0);
		}
		else
		{
			glUniform4f(program->colorLocation, static_cast<float>(GetRValue(pit->color) / 255.), static_cast<float>(GetGValue(pit->color) / 255.), static_cast<float>(GetBValue(pit->color) / 255.), 1.);
			glUniform1i(program->useTextLocation, 0);
			glUniform1i(program->useTransparentTextLocation, 0);
			glUniform1i(program->alphaInTransparentTexture, 0);
			glUniform1i(program->useShadowTextLocation, 0);
			glUniform1i(program->useSpecularTextLocation, 0);
			glUniform1i(program->useNormalTextLocation, 0);
		}

		sphere->Draw();
	}

	if (spaceship && spaceshipProgram)
	{
		program->UnUse();

		RenderSpaceship(mat);

		// need to have the 'billboard' drawn over the 'spaceship', too, so that's why it's here like that
		program->Use();
	}

	if (theApp.options.showBillboard)
		DisplayBilboard();
}

void CSolarSystemView::RenderShadowScene()
{
	if (NULL == shadowProgram) return;

	CSolarSystemDoc *doc = GetDocument();
	if (!doc) return;

	shadowProgram->Use();

	shadowProgram->depthMapFBO.Bind();

	glDrawBuffer(GL_NONE);

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glCullFace(GL_FRONT);

	ShadowUniforms params(doc->m_SolarSystem, *shadowProgram, program->nrlights);

	auto pit = doc->m_SolarSystem.m_BodyProperties.begin();
	for (auto it = doc->m_SolarSystem.m_Bodies.begin(); it != doc->m_SolarSystem.m_Bodies.end(); ++it, ++pit)
	{
		if (pit->isSun) continue; // Suns don't drop a shadow, don't render them

		glm::dmat4 modelMatHP(1);
		glm::dvec3 pos(it->m_Position.X / AGLU, it->m_Position.Y / AGLU, it->m_Position.Z / AGLU);

		// THIS IS A HACK TO NICELY DISPLAY THE SOLAR SYSTEM 
		// if the moon is inside the planet because of the scaling, the distance from the planet to it is scaled up, too

		if (pit->isMoon && pit->scaleDistance != 1.) MoonHack(pit, it, pos);

		// ****************************************************************************************************************************
		modelMatHP = glm::translate(modelMatHP, pos);
		const double scale = it->m_Radius * pit->scale / AGLU;
		modelMatHP = glm::scale(modelMatHP, glm::dvec3(scale, scale, scale));

		const glm::mat4 modelMat(modelMatHP);
		glUniformMatrix4fv(shadowProgram->getMatLocation(), 1, GL_FALSE, value_ptr(modelMat));

		sphere->Draw();
	}

	glFlush();

	glCullFace(GL_BACK);
	shadowProgram->depthMapFBO.UnBind();
	shadowProgram->UnUse();
}



void CSolarSystemView::RenderSky()
{
	if (skyProgram)
	{
		// remove translation from the camera matrix		
		glm::dmat4 matHP(glm::dmat3((glm::dmat4)camera));
		matHP = skyboxPerspectiveMatrix * matHP;
		matHP = glm::scale(matHP, glm::dvec3(2. * farPlaneDistance, 2. * farPlaneDistance, 2. * farPlaneDistance));

		const glm::mat4 mat(matHP);
		skyProgram->Draw(mat);
	}
}

void CSolarSystemView::RenderSpaceship(glm::mat4& mat)
{
	if (spaceship && spaceshipProgram)
	{
		//glm::mat4 mat(perspectiveMatrix * (glm::dmat4)camera);
		CSolarSystemDoc* doc = GetDocument();
		if (!doc) return;

		spaceshipProgram->Use();

		SpaceshipUniforms params(doc->m_SolarSystem, *spaceshipProgram, spaceshipProgram->nrlights);

		const glm::dvec3 pos(0, 0, -0.150);


		const glm::dvec3 spaceshipPos = glm::dvec3(camera.eyePos.X / AGLU, camera.eyePos.Y / AGLU, camera.eyePos.Z / AGLU) + pos;
		glUniform3f(spaceshipProgram->viewPosLocation, static_cast<float>(camera.eyePos.X / AGLU), static_cast<float>(camera.eyePos.Y / AGLU), static_cast<float>(camera.eyePos.Z / AGLU));

		for (unsigned int i = 0; i < (spaceshipProgram->nrlights == 0 ? 1 : spaceshipProgram->nrlights); ++i)
		{
			glm::dvec3 lightDir = spaceshipProgram->lights[i].lightPos - spaceshipPos;

			// linear instead of quadratic and using a coefficient that provides enough light even for Pluto
			const float atten = static_cast<float>(1. / (1. + lightCoeff * glm::length(lightDir)));

			lightDir = glm::normalize(lightDir);
			glUniform3f(spaceshipProgram->lights[i].lightDirPos, static_cast<float>(lightDir.x), static_cast<float>(lightDir.y), static_cast<float>(lightDir.z));
			glUniform1f(spaceshipProgram->lights[i].attenPos, atten);
		}

		glUniformMatrix4fv(spaceshipProgram->matLocation, 1, GL_FALSE, value_ptr(mat));

		glm::dmat4 precisionMat(camera.getMatrixDouble());
		precisionMat = glm::inverse(precisionMat);

		precisionMat = glm::translate(precisionMat, pos);

		const double scale = 0.005f;
		precisionMat = glm::scale(precisionMat, glm::dvec3(scale, scale, scale));

		const glm::mat4 modelMat(precisionMat);

		precisionMat = glm::transpose(glm::inverse(precisionMat));
		glm::mat3 transpInvModelMat(precisionMat);

		glUniformMatrix4fv(spaceshipProgram->modelMatLocation, 1, GL_FALSE, value_ptr(modelMat));
		glUniformMatrix3fv(spaceshipProgram->transpInvModelMatLocation, 1, GL_FALSE, value_ptr(transpInvModelMat));

		spaceship->Draw(*spaceshipProgram);

		spaceshipProgram->UnUse();
	}
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
		switch (pMsg->wParam)
		{
		case VK_UP:
			if (ctrl) movement = OpenGL::Camera::Movements::pitchUp;
			else if (shift) movement = OpenGL::Camera::Movements::moveUp;
			else movement = OpenGL::Camera::Movements::moveForward;
			handled = true;
			break;
		case VK_DOWN:
			if (ctrl) movement = OpenGL::Camera::Movements::pitchDown;
			else if (shift) movement = OpenGL::Camera::Movements::moveDown;
			else movement = OpenGL::Camera::Movements::moveBackward;
			handled = true;
			break;
		case VK_LEFT:
			if (ctrl) movement = OpenGL::Camera::Movements::yawLeft;
			else if (shift) movement = OpenGL::Camera::Movements::rollLeft;
			else movement = OpenGL::Camera::Movements::moveLeft;
			handled = true;
			break;
		case VK_RIGHT:
			if (ctrl) movement = OpenGL::Camera::Movements::yawRight;
			else if (shift) movement = OpenGL::Camera::Movements::rollRight;
			else movement = OpenGL::Camera::Movements::moveRight;
			handled = true;
			break;
		case VK_ADD:
		case VK_OEM_PLUS:
		{
			CSolarSystemDoc *doc = GetDocument();
			if (doc->nrsteps < MAX_NRSTEPS)
			{
				++doc->nrsteps;
				CMFCToolBarSlider::SetPos(ID_SLIDER, static_cast<int>(doc->nrsteps));
			}
		}
		handled = true;
		break;
		case VK_SUBTRACT:
		case VK_OEM_MINUS:
		{
			CSolarSystemDoc *doc = GetDocument();
			if (doc->nrsteps > 1)
			{
				--doc->nrsteps;
				CMFCToolBarSlider::SetPos(ID_SLIDER, static_cast<int>(doc->nrsteps));
			}
		}
		handled = true;
		break;
		case VK_SPACE:
		{
			CMainFrame *frame = dynamic_cast<CMainFrame*>(theApp.m_pMainWnd);
			frame->OnSimulateRun();
		}
		handled = true;
		break;
		default:
			movement = OpenGL::Camera::Movements::noMove;
		}

		Invalidate(0);
	}
	else if (pMsg->message == WM_KEYUP) keyDown = false;

	return handled;
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
				doc->RetrieveData();
			
				camera.Tick();
				if (keyDown) camera.Move(movement);				
			}
			else
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

			Invalidate(0);
		}
	}

	CView::OnTimer(nIDEvent);
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


void CSolarSystemView::Reset()
{
	delete sphere;
	sphere = NULL;

	delete billboardRectangle;
	billboardRectangle = NULL;

	delete billboardTexture;
	billboardTexture = NULL;

#ifdef DISPLAY_SPACESHIP
	//delete triangle;
	//triangle = NULL;
#endif

	delete spaceship;
	spaceship = NULL;
	
	ClearProgram();
	ClearShadowProgram();
	ClearSkyProgram();
	ClearSpaceshipProgram();

	inited = false;

	Setup();
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
	Vector3D<double> forward = (camera.lookAt - camera.eyePos).Normalize();
	Vector3D<double> towards = GetTowardsVector(point, forward);

	double angle = acos(towards * forward);
	int numticks = static_cast<int>(angle / camera.GetRotateAngle());

	camera.RotateTowards(angle - numticks * camera.GetRotateAngle(), towards);
	camera.ProgressiveRotate(towards, numticks);

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


void CSolarSystemView::EnableAntialias()
{
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_DITHER);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_MULTISAMPLE_ARB);
}


void CSolarSystemView::DisableAntialias()
{
	glDisable(GL_LINE_SMOOTH);
	glDisable(GL_POLYGON_SMOOTH);
	glDisable(GL_DITHER);
	glDisable(GL_POINT_SMOOTH);
	glDisable(GL_MULTISAMPLE_ARB);
}


void CSolarSystemView::DisplayBilboard()
{
	glm::dmat4 precisionMat(camera.getMatrixDouble());
	precisionMat = glm::inverse(precisionMat);

	const glm::dvec3 pos(0, -0.038, -0.101);

	precisionMat = glm::translate(precisionMat, pos);

	const double scale = 0.0025f;
	precisionMat = glm::scale(precisionMat, glm::dvec3(scale, scale, scale));
	
	const glm::mat4 modelMat(precisionMat);

	precisionMat = glm::transpose(glm::inverse(precisionMat));
	glm::mat3 transpInvModelMat(precisionMat);

	glUniformMatrix4fv(program->modelMatLocation, 1, GL_FALSE, value_ptr(modelMat));
	glUniformMatrix3fv(program->transpInvModelMatLocation, 1, GL_FALSE, value_ptr(transpInvModelMat));

	glUniform1i(program->isSunLocation, 1); // don't use lightning on it

	glUniform4f(program->colorLocation, 0.0f, 0.0f, 1.0f, 0.6f); // blue with alpha blending for now

	billboardTexture->Bind();
	glUniform1i(program->useTextLocation, 1); // use texture
	glUniform1i(program->useTransparentTextLocation, 0); // but no transparent texture on top of it

	glUniform1i(program->useAlphaBlend, 1);

	glUniform1i(program->alphaInTransparentTexture, 0);
	glUniform1i(program->useShadowTextLocation, 0);
	glUniform1i(program->useSpecularTextLocation, 0);
	glUniform1i(program->useNormalTextLocation, 0);

	DisableAntialias(); // otherwise a diagonal line is shown over the rectangle sometimes, with alpha blending on

	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);

	billboardRectangle->Draw();

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	glDisable(GL_BLEND);

	EnableAntialias();
}


void CSolarSystemView::SetBillboardText(const char* text)
{
	if (!billboardTexture) return;
	
	memoryBitmap.WriteText(text, font);

	wglMakeCurrent(m_hDC, m_hRC);
	memoryBitmap.SetIntoTexture(*billboardTexture);
	wglMakeCurrent(NULL, NULL);
}
