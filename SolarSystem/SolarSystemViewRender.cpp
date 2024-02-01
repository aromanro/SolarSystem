#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "SolarSystem.h"
#endif

#include "SolarSystemDoc.h"
#include "SolarSystemView.h"
#include "MainFrm.h"
#include "Constants.h"

#include <GL/glew.h>
#include <gl/GL.h>      // OpenGL Libraries
#include <gl/GLU.h>     // GLU OpenGL Libraries


#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>


#include "MatrixPush.h"

#include "Cube.h"

#include "SkyBoxCubeMapProgram.h"
#include "SkySphereProgram.h"

#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


static const double lightCoeff = 0.000001;


void CSolarSystemView::MoonHack(const BodyList::iterator& bit, const BodyPropList::iterator& pit, glm::dvec3& pos)
{
	CSolarSystemDoc* doc = GetDocument();

	if (-1 == pit->parentIndex)
		return;

	const MolecularDynamics::Body& parent = doc->m_SolarSystem.m_Bodies[pit->parentIndex];
	const MolecularDynamics::BodyPosition& parentPosition = m_BodiesPosition[pit->parentIndex];
	const BodyProperties& parentProps = doc->m_SolarSystem.m_BodyProperties[pit->parentIndex];

	// should not happen, but be safe
	if (parentProps.isMoon || parentProps.isSun)
		return;

	glm::dvec3 mpos(parentPosition.m_Position.X / AGLU, parentPosition.m_Position.Y / AGLU, parentPosition.m_Position.Z / AGLU);
	const glm::dvec3 fromvec = pos - mpos;
	const double dist = glm::length(fromvec);

	if (dist <= (parent.m_Radius * parentProps.scale + bit->m_Radius * pit->scale) / AGLU)
		pos = mpos + glm::dvec3(fromvec.x * pit->scaleDistance, fromvec.y * pit->scaleDistance, fromvec.z * pit->scaleDistance);
}

void CSolarSystemView::RenderScene()
{
	if (nullptr == program) return;

	CSolarSystemDoc* doc = GetDocument();
	if (!doc) return;

	glm::mat4 mat(perspectiveMatrix * (glm::dmat4)camera);

	program->Use();

	if (shadowProgram) shadowProgram->depthCubemap.Bind();

	Uniforms params(doc->m_SolarSystem, m_BodiesPosition, *program, program->nrlights);

	glUniform3f(program->viewPosLocation, static_cast<float>(camera.GetEyePos().X), static_cast<float>(camera.GetEyePos().Y), static_cast<float>(camera.GetEyePos().Z));
	glUniform1f(program->gammaLoc, (float)(theApp.options.gammaCorrection == 2 ? theApp.options.gammaCorrectionCoeff : 1.));
	glUniformMatrix4fv(program->matLocation, 1, GL_FALSE, value_ptr(mat));

	glUniform1i(program->useAlphaBlend, 0);

	auto bit = doc->m_SolarSystem.m_Bodies.begin();
	auto pit = doc->m_SolarSystem.m_BodyProperties.begin();
	for (auto it = m_BodiesPosition.begin(); it != m_BodiesPosition.end(); ++it, ++pit, ++bit)
		RenderPlanet(it, pit, bit);

	if (spaceship && spaceshipProgram)
	{
		program->UnUse();

		RenderSpaceship(mat);

		// need to have the 'billboard' drawn over the 'spaceship', too, so that's why it's here like that
		program->Use();

		glUniform3f(program->viewPosLocation, static_cast<float>(camera.GetEyePos().X), static_cast<float>(camera.GetEyePos().Y), static_cast<float>(camera.GetEyePos().Z));
		glUniformMatrix4fv(program->matLocation, 1, GL_FALSE, value_ptr(mat));
	}

	if (theApp.options.showBillboard)
		DisplayBilboard();
}

void CSolarSystemView::RenderPlanet(const BodyPositionList::iterator& it, const BodyPropList::iterator& pit, const BodyList::iterator& bit)
{
	glm::dvec3 pos(it->m_Position.X / AGLU, it->m_Position.Y / AGLU, it->m_Position.Z / AGLU);


	// THIS IS A HACK TO NICELY DISPLAY THE SOLAR SYSTEM 
	// if the moon is inside the planet because of the scaling, the distance from the planet to it is scaled up, too

	if (pit->isMoon && pit->scaleDistance != 1.) MoonHack(bit, pit, pos);

	// ****************************************************************************************************************************

	const double scale = bit->m_Radius * pit->scale / AGLU;
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

	SetTextures(pit);

	sphere->Draw();
}

void CSolarSystemView::SetTextures(const BodyPropList::iterator& pit)
{
	static const double scale = 1. / 255.;

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
			glUniform4f(program->colorLocation, static_cast<float>(GetRValue(pit->color) * scale), static_cast<float>(GetGValue(pit->color) * scale), static_cast<float>(GetBValue(pit->color) * scale), 1.);
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
		program->UnsetTextures(pit->color);
}


void CSolarSystemView::RenderShadowScene()
{
	if (nullptr == shadowProgram) return;

	CSolarSystemDoc* doc = GetDocument();
	if (!doc) return;

	shadowProgram->Use();

	shadowProgram->depthMapFBO.Bind();

	glDrawBuffer(GL_NONE);

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glCullFace(GL_FRONT);

	ShadowUniforms params(doc->m_SolarSystem, m_BodiesPosition, *shadowProgram, program->nrlights);

	auto bit = doc->m_SolarSystem.m_Bodies.begin();
	auto pit = doc->m_SolarSystem.m_BodyProperties.begin();
	for (auto it = m_BodiesPosition.begin(); it != m_BodiesPosition.end(); ++it, ++pit, ++bit)
	{
		if (pit->isSun) continue; // Suns don't drop a shadow, don't render them

		glm::dmat4 modelMatHP(1);
		glm::dvec3 pos(it->m_Position.X / AGLU, it->m_Position.Y / AGLU, it->m_Position.Z / AGLU);

		// THIS IS A HACK TO NICELY DISPLAY THE SOLAR SYSTEM 
		// if the moon is inside the planet because of the scaling, the distance from the planet to it is scaled up, too

		if (pit->isMoon && pit->scaleDistance != 1.) MoonHack(bit, pit, pos);

		// ****************************************************************************************************************************
		modelMatHP = glm::translate(modelMatHP, pos);
		const double scale = bit->m_Radius * pit->scale / AGLU;
		modelMatHP = glm::scale(modelMatHP, glm::dvec3(scale, scale, scale));

		const glm::mat4 modelMat(modelMatHP);
		glUniformMatrix4fv(shadowProgram->getMatLocation(), 1, GL_FALSE, value_ptr(modelMat));

		sphere->Draw();
	}

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

		// unfortunately it looks like not all obj files are nice
		glDisable(GL_CULL_FACE);

		SpaceshipUniforms params(doc->m_SolarSystem, m_BodiesPosition, *spaceshipProgram, spaceshipProgram->nrlights);

		glUniformMatrix4fv(spaceshipProgram->matLocation, 1, GL_FALSE, value_ptr(mat));

		const double cameraX = camera.GetEyePos().X;
		const double cameraY = camera.GetEyePos().Y;
		const double cameraZ = camera.GetEyePos().Z;
		const glm::dvec3 cameraVector = glm::dvec3(cameraX, cameraY, cameraZ);

		// the translation vector for the spaceship from the camera position, in the looking direction:
		const Vector3D<double> forward = camera.getNormalizedForward();
		glm::dvec3 pos(forward.X, forward.Y, forward.Z);
		pos *= theApp.options.translate;

		glUniform3f(spaceshipProgram->viewPosLocation, static_cast<float>(cameraX), static_cast<float>(cameraY), static_cast<float>(cameraZ));
		glUniform1f(spaceshipProgram->gammaLoc, (float)(theApp.options.gammaCorrection == 2 ? theApp.options.gammaCorrectionCoeff : 1.));


		// the camera matrix
		glm::dmat4 precisionMat(camera.getMatrixDouble());
		// undo translation here, so we end up with the rotation only matrix, to have the same rotation as the camera:
		precisionMat[3][0] = 0;
		precisionMat[3][1] = 0;
		precisionMat[3][2] = 0;
		// now invert it to have a matrix to 'undo' the rotation:
		precisionMat = glm::transpose(precisionMat);

		const double scale = theApp.options.scale;

		// spaceship position is at the camera position, translated in front of it
		const glm::dvec3 spaceshipPos = cameraVector + pos;

		spaceshipOrientation.ComputeRotations();

		const glm::dmat4 modelMatHP =
			glm::rotate( // around x 
				glm::rotate( // around y
					glm::rotate( // around z
						//*****************************************************************************************************
						glm::rotate( // around x 
							glm::rotate( // around y
								glm::rotate( // around z
									glm::scale(glm::translate(glm::dmat4(1.), spaceshipPos), glm::dvec3(scale, scale, scale)) * precisionMat,
									spaceshipOrientation.rotationZ * M_PI / 180., // the angle to rotate around z
									glm::dvec3(0, 0, 1)), // around z axis
								spaceshipOrientation.rotationY * M_PI / 180., // the angle to rotate around y axis
								glm::dvec3(0, 1, 0)), // around y axis
							spaceshipOrientation.rotationX * M_PI / 180., // the angle to rotate around x axis
							glm::dvec3(1, 0, 0)), // around x axis
						//*****************************************************************************************************
						theApp.options.rotateZ * M_PI / 180., // the angle to rotate around z
						glm::dvec3(0, 0, 1)), // around z axis
					theApp.options.rotateY * M_PI / 180., // the angle to rotate around y axis
					glm::dvec3(0, 1, 0)), // around y axis
				theApp.options.rotateX * M_PI / 180., // the angle to rotate around x axis
				glm::dvec3(1, 0, 0)); // around x axis

		const glm::mat4 modelMat(modelMatHP);
		const glm::mat3 transpInvModelMat(glm::transpose(glm::inverse(modelMatHP)));

		glUniformMatrix4fv(spaceshipProgram->modelMatLocation, 1, GL_FALSE, value_ptr(modelMat));
		glUniformMatrix3fv(spaceshipProgram->transpInvModelMatLocation, 1, GL_FALSE, value_ptr(transpInvModelMat));


		for (unsigned int i = 0; i < (spaceshipProgram->nrlights == 0 ? 1 : spaceshipProgram->nrlights); ++i)
		{
			glm::dvec3 lightDir = spaceshipProgram->lights[i].lightPos - spaceshipPos;

			// linear instead of quadratic and using a coefficient that provides enough light even for Pluto
			const float atten = static_cast<float>(1. / (1. + lightCoeff * glm::length(lightDir)));

			lightDir = glm::normalize(lightDir);
			glUniform3f(spaceshipProgram->lights[i].lightDirPos, static_cast<float>(lightDir.x), static_cast<float>(lightDir.y), static_cast<float>(lightDir.z));
			glUniform1f(spaceshipProgram->lights[i].attenPos, atten);
		}

		spaceship->Draw(*spaceshipProgram);

		spaceshipProgram->UnUse();

		glEnable(GL_CULL_FACE);
	}
}

void CSolarSystemView::DisplayBilboard()
{
	glm::dmat4 precisionMat(camera.getMatrixDouble());
	// cancel out the translation of the camera, but preserve rotation:
	precisionMat[3][0] = 0;
	precisionMat[3][1] = 0;
	precisionMat[3][2] = 0;

	// put the billboard in front of the camera
	const Vector3D<double> forward = camera.getNormalizedForward();
	glm::dvec3 pos(forward.X, forward.Y, forward.Z);
	pos *= 0.101;

	// but shifted downwards a little
	const Vector3D<double> up = camera.getNormalizedUp();
	pos -= 0.038 * glm::dvec3(up.X, up.Y, up.Z);

	const glm::dvec3 cameraVector = glm::dvec3(camera.GetEyePos().X, camera.GetEyePos().Y, camera.GetEyePos().Z);
	const glm::dvec3 billboardPos = cameraVector + pos;

	const double scale = 0.0025f;

	const glm::dmat4 modelMatHP = glm::scale(glm::translate(glm::dmat4(1.), billboardPos), glm::dvec3(scale, scale, scale)) * glm::transpose(precisionMat);
	const glm::mat4 modelMat(modelMatHP);
	const glm::mat3 transpInvModelMat(glm::transpose(glm::inverse(modelMatHP)));

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
