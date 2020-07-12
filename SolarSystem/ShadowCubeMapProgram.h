#pragma once

#include "Program.h"
#include "Texture.h"

#include "OpenGLObject.h"


#include <GL\glew.h>
#include <gl\gl.h>      // OpenGL Libraries
#include <gl\glu.h>     // GLU OpenGL Libraries


#include <glm.hpp>

#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>

namespace OpenGL {

	class ShadowCubeMapProgram : public Program
	{
	protected:
		// vertex shader parameters
		GLint matLocation;
		GLint shadowMatPos;
		GLint lightPosLoc;
		GLint farPlaneLoc;

		void getUniformsLocations();
	public:
		ShadowCubeMapProgram();
		virtual ~ShadowCubeMapProgram();

		bool SetShaders();

		GLint getMatLocation() const { return matLocation; }

		class CubeMapTexture : public OpenGLObject
		{
		public:
			CubeMapTexture();
			virtual ~CubeMapTexture();


			virtual void Bind() override;
			virtual void UnBind() override;
			virtual GLuint getType() const override;
		};

		FrameBufferObject depthMapFBO;
		CubeMapTexture depthCubemap;
		void SetLightAndFarPlanePosition(glm::vec3& lightPos);
	};


}
