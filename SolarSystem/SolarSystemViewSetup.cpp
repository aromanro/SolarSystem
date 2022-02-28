#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "SolarSystem.h"
#endif

#include "SolarSystemDoc.h"
#include "SolarSystemView.h"
#include "Constants.h"

#include <GL\glew.h>
#include <gl\gl.h>      // OpenGL Libraries
#include <gl\glu.h>     // GLU OpenGL Libraries

#include "SkyBoxCubeMapProgram.h"
#include "SkySphereProgram.h"

#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CSolarSystemView::Uniforms::Uniforms(SolarSystemBodies& m_SolarSystem, BodyPositionList& m_BodiesPosition, SolarSystemGLProgram& program, unsigned int nrlights)
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
		for (auto it = m_BodiesPosition.begin(); it != m_BodiesPosition.end(); ++it, ++pit)
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



CSolarSystemView::SpaceshipUniforms::SpaceshipUniforms(SolarSystemBodies& m_SolarSystem, BodyPositionList& m_BodiesPosition, OpenGL::SpaceshipProgram& program, unsigned int nrlights)
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
		for (auto it = m_BodiesPosition.begin(); it != m_BodiesPosition.end(); ++it, ++pit)
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


CSolarSystemView::ShadowUniforms::ShadowUniforms(SolarSystemBodies& m_SolarSystem, BodyPositionList& m_BodiesPosition, OpenGL::ShadowCubeMapProgram& program, unsigned int nrlights)
{
	// light
	glm::vec3 lightPos;

	if (nrlights == 0) lightPos = glm::vec3(-farPlaneDistance, -farPlaneDistance, 0);
	else {
		auto pit = m_SolarSystem.m_BodyProperties.begin();
		for (auto it = m_BodiesPosition.begin(); it != m_BodiesPosition.end(); ++it, ++pit)
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

bool CSolarSystemView::SetupShaders()
{
	program = new SolarSystemGLProgram();

	CSolarSystemDoc* doc = GetDocument();
	unsigned int nrlights = 0;

	for (const auto& body : doc->m_SolarSystem.m_BodyProperties)
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
		OpenGL::SkyBoxCubeMapProgram* skyBoxProgram = new OpenGL::SkyBoxCubeMapProgram();
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

	m_NewBodiesPosition.clear();
	m_OldBodiesPosition.clear();

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

		if (1 == theApp.options.gammaCorrection) glEnable(GL_FRAMEBUFFER_SRGB);

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glPolygonMode(GL_FRONT, GL_FILL);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glMatrixMode(GL_MODELVIEW);

		glEnable(GL_MULTISAMPLE);

		CRect rect;
		GetClientRect(rect);
		Height = rect.Height();
		Width = rect.Width();
	}
	else {
		wglMakeCurrent(m_hDC, m_hRC);

		if (1 == theApp.options.gammaCorrection) glEnable(GL_FRAMEBUFFER_SRGB);
		else glDisable(GL_FRAMEBUFFER_SRGB);
	}

	CSolarSystemDoc* doc = GetDocument();

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

	if (!theApp.options.spaceshipObjFile.IsEmpty())
	{
		ObjLoader objLoader;
		CT2CA convAnsiString(theApp.options.spaceshipObjFile);
		std::string spaceshipFile(convAnsiString);
		if (objLoader.Load(spaceshipFile))
			spaceship = new OpenGL::ComplexObjectCompositeMaterials(objLoader);
	}

	if (!SetupShaders()) {
		ClearProgram();
		return;
	}

	program->DetachShaders();

	program->SetupLights(doc->m_SolarSystem.m_BodyProperties);

	if (theApp.options.drawTextures)
		doc->m_SolarSystem.LoadTextures();

	SetupSkyBox();

	SetupShadows();

	if (spaceship) SetupSpaceship();

	wglMakeCurrent(NULL, NULL);

	SetBillboardText("");

	inited = true;
}


void CSolarSystemView::Reset()
{
	delete sphere;
	sphere = NULL;

	delete billboardRectangle;
	billboardRectangle = NULL;

	delete billboardTexture;
	billboardTexture = NULL;

	delete spaceship;
	spaceship = NULL;

	ClearProgram();
	ClearShadowProgram();
	ClearSkyProgram();
	ClearSpaceshipProgram();

	inited = false;

	Setup();
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
