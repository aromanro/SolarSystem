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
		glm::vec3 lightPos;
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
	GLint depthMapLoc;
protected:
	void getUniformsLocations();
	bool SetupVertexShader();
	bool SetupFragmentShader();
};

