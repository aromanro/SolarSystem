#pragma once

#include <glm.hpp>

#include "Program.h"
#include "SolarSystemBodies.h"

class SolarSystemGLProgram :
	public OpenGL::Program
{
public:
	SolarSystemGLProgram();
	virtual ~SolarSystemGLProgram();

	bool SetupShaders(unsigned int nrlights = 1);

	void SetupLights(BodyPropList& BodyProperties);

	struct Light {
		GLint lightDirPos = 0;
		GLint attenPos = 0;
		glm::dvec3 lightPos;
	};

	std::vector<Light> lights;

	unsigned int nrlights;

	// vertex shader parameters
	GLint matLocation;
	GLint modelMatLocation;
	GLint transpInvModelMatLocation;

	// fragment shader uniform parameters
	GLint colorLocation;
	GLint useTextLocation;
	GLint useTransparentTextLocation;
	GLint useShadowTextLocation;
	GLint useSpecularTextLocation;
	GLint useNormalTextLocation;

	GLint alphaInTransparentTexture;

	GLint isSunLocation;
	GLint useAlphaBlend;
	GLint viewPosLocation;
	GLint farPlaneLoc;
	GLint lightPosLoc;
	GLint calcShadowsLoc;
	GLint textureLoc;
	GLint transparentTextureLoc;
	GLint shadowTextureLoc;
	GLint specularTextureLoc;
	GLint normalTextureLoc;
	GLint depthMapLoc;

	GLint gammaLoc;

	// pass the color to be used instead of textures
	void UnsetTextures(COLORREF color)
	{
		static const double scale = 1. / 255.;

		glUniform4f(colorLocation, static_cast<float>(GetRValue(color) * scale), static_cast<float>(GetGValue(color) * scale), static_cast<float>(GetBValue(color) * scale), 1.);
		glUniform1i(useTextLocation, 0);
		glUniform1i(useTransparentTextLocation, 0);
		glUniform1i(alphaInTransparentTexture, 0);
		glUniform1i(useShadowTextLocation, 0);
		glUniform1i(useSpecularTextLocation, 0);
		glUniform1i(useNormalTextLocation, 0);
	}

protected:
	void getUniformsLocations();
	bool SetupVertexShader();
	bool SetupFragmentShader();
};

