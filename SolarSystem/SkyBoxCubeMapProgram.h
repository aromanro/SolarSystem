#pragma once

#include "Program.h"
#include "Texture.h"

#include <glm.hpp>

namespace OpenGL {

	class SkyBoxCubeMapProgram : public Program
	{
	protected:
		class SkyBox
		{
		protected:
			VertexArrayObject *vao;
			VertexBufferObject *vbo;
		public:
			SkyBox();
			~SkyBox();

			void Draw();

			void Bind();
			void UnBind();
		};


		class CubeMapTexture : public OpenGLObject
		{
		public:
			CubeMapTexture();
			virtual ~CubeMapTexture();


			virtual void Bind();
			virtual void UnBind();
			virtual GLuint getType() const;

			void setDataLeft(const void *data, int width, int height);
			void setDataRight(const void *data, int width, int height);
			void setDataFront(const void *data, int width, int height);
			void setDataBack(const void *data, int width, int height);
			void setDataTop(const void *data, int width, int height);
			void setDataBottom(const void *data, int width, int height);

			bool LoadTextures(const char* left = "Textures\\left.jpg", const char* right = "Textures\\right.jpg", const char* top = "Textures\\top.jpg", const char* bottom = "Textures\\bottom.jpg", const char* front = "Textures\\front.jpg", const char* back = "Textures\\back.jpg");
		};

		CubeMapTexture* cubeMapTextures;
		SkyBox *skyBox;
		GLint transformMatLoc;
	public:

		SkyBoxCubeMapProgram();
		virtual ~SkyBoxCubeMapProgram();

		bool SetShaders();
		bool LoadTextures(const char* left = "Textures\\left.jpg", const char* right = "Textures\\right.jpg", const char* top = "Textures\\top.jpg", const char* bottom = "Textures\\bottom.jpg", const char* front = "Textures\\front.jpg", const char* back = "Textures\\back.jpg");

		void Draw(glm::mat4& mat);
	};

}