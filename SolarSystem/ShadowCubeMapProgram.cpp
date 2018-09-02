#include "stdafx.h"
#include "ShadowCubeMapProgram.h"
#include "Constants.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define GLSL(src) "#version 330\n" #src

namespace OpenGL {

	ShadowCubeMapProgram::CubeMapTexture::CubeMapTexture()
	{
		glActiveTexture(GL_TEXTURE1);

		glGenTextures(1, &ID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		for (GLuint i = 0; i < 6; ++i)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		UnBind();
	}

	ShadowCubeMapProgram::CubeMapTexture::~CubeMapTexture()
	{
		UnBind();
		glDeleteTextures(1, &ID);
	}

	void ShadowCubeMapProgram::CubeMapTexture::Bind()
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
	}

	void ShadowCubeMapProgram::CubeMapTexture::UnBind()
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	GLuint ShadowCubeMapProgram::CubeMapTexture::getType() const
	{
		return GL_TEXTURE_CUBE_MAP;
	}

	ShadowCubeMapProgram::ShadowCubeMapProgram()
		: matLocation(0), shadowMatPos(0), lightPosLoc(0), farPlaneLoc(0)
	{
		depthMapFBO.Bind();
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);

		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) AfxMessageBox(L"Depth Frame Buffer not complete");
		
		depthMapFBO.UnBind();		
	}


	ShadowCubeMapProgram::~ShadowCubeMapProgram()
	{
	}

	void ShadowCubeMapProgram::getUniformsLocations()
	{
		// vertex shader parameters
		matLocation = glGetUniformLocation(getID(), "modelMat");

		// geometry shader parameters	
		shadowMatPos = glGetUniformLocation(getID(), "shadowMatrices");

		// fragment shader parameters
		lightPosLoc = glGetUniformLocation(getID(), "lightPos");

		farPlaneLoc = glGetUniformLocation(getID(), "farPlane");
	}

	bool ShadowCubeMapProgram::SetShaders()
	{
		VertexShader vertexShader;

		vertexShader.setSource(GLSL(

			layout(location = 0) in vec3 position;

			uniform mat4 modelMat;

			void main()
			{
				gl_Position = modelMat * vec4(position, 1.0);
			}

		));


		if (vertexShader.getStatus() == false)
		{
			AfxMessageBox(CString("Shadow CubeMap vertex shader: ") + CString(vertexShader.getStatusMessage()));
			return false;
		}

		GeometryShader geometryShader;

		geometryShader.setSource(GLSL(

			layout(triangles) in;
			layout(triangle_strip, max_vertices = 18) out;

			uniform mat4 shadowMatrices[6];

			out vec4 FragPos;

			void main()
			{
				for (gl_Layer = 0; gl_Layer < 6; ++gl_Layer)
				{	
					for (int vert = 0; vert < 3; ++vert)
					{
						FragPos = gl_in[vert].gl_Position;
						gl_Position = shadowMatrices[gl_Layer] * FragPos;
						EmitVertex();
					}
					EndPrimitive();
				}
			}

		));

		if (geometryShader.getStatus() == false)
		{
			AfxMessageBox(CString("Shadow CubeMap geometry shader: ") + CString(geometryShader.getStatusMessage()));
			return false;
		}

		FragmentShader fragmentShader;

		fragmentShader.setSource(GLSL(

			in vec4 FragPos;

			uniform vec3 lightPos;
			uniform float farPlane;

			void main()
			{
				gl_FragDepth = length(FragPos.xyz - lightPos) / farPlane;
			}

		));


		if (fragmentShader.getStatus() == false)
		{
			AfxMessageBox(CString("Shadow CubeMap fragment shader: ") + CString(fragmentShader.getStatusMessage()));
			return false;
		}

		Attach(vertexShader);
		Attach(geometryShader);
		Attach(fragmentShader);

		Program::Bind();

		getUniformsLocations();

		return true;
	}
}

void OpenGL::ShadowCubeMapProgram::SetLightAndFarPlanePosition(glm::vec3& lightPosition)
{
	std::vector<glm::mat4> shadowTransforms;
	shadowTransforms.reserve(6);

	const glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, (float)nearPlaneDistance, (float)farPlaneDistance);

	shadowTransforms.emplace_back(shadowProj *	glm::lookAt(lightPosition, lightPosition + glm::vec3(1, 0, 0), glm::vec3(0, -1, 0)));
	shadowTransforms.emplace_back(shadowProj *	glm::lookAt(lightPosition, lightPosition + glm::vec3(-1, 0, 0), glm::vec3(0, -1, 0)));
	shadowTransforms.emplace_back(shadowProj *	glm::lookAt(lightPosition, lightPosition + glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)));
	shadowTransforms.emplace_back(shadowProj *	glm::lookAt(lightPosition, lightPosition + glm::vec3(0, -1, 0), glm::vec3(0, 0, -1)));
	shadowTransforms.emplace_back(shadowProj *	glm::lookAt(lightPosition, lightPosition + glm::vec3(0, 0, 1), glm::vec3(0, -1, 0)));
	shadowTransforms.emplace_back(shadowProj *	glm::lookAt(lightPosition, lightPosition + glm::vec3(0, 0, -1), glm::vec3(0, -1, 0)));
	
	glUniformMatrix4fv(shadowMatPos, 6, GL_FALSE, glm::value_ptr(shadowTransforms[0]));

	glUniform3f(lightPosLoc, lightPosition.x, lightPosition.y, lightPosition.z);

	glUniform1f(farPlaneLoc, (float)farPlaneDistance);
}
