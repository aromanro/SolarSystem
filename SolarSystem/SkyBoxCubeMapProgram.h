#pragma once

#include "Program.h"
#include "Texture.h"
#include "VertexBufferRenderable.h"

#include "SkyBase.h"

#include <glm.hpp>

namespace OpenGL {

	class SkyBoxCubeMapProgram : public SkyBase
	{
	protected:
		class SkyBox : public VertexBufferRenderable
		{
		public:
			SkyBox();

			virtual void Draw() override;
		};

		class CubeMapTexture : public OpenGLObject
		{
		public:
			CubeMapTexture();
			virtual ~CubeMapTexture();


			virtual void Bind() override;
			virtual void UnBind() override;
			virtual GLuint getType() const override;

			void setDataLeft(const void *data, int width, int height, int bpp = 24);
			void setDataRight(const void *data, int width, int height, int bpp = 24);
			void setDataFront(const void *data, int width, int height, int bpp = 24);
			void setDataBack(const void *data, int width, int height, int bpp = 24);
			void setDataTop(const void *data, int width, int height, int bpp = 24);
			void setDataBottom(const void *data, int width, int height, int bpp = 24);

			bool LoadTextures(const char* left = "Textures\\left.jpg", const char* right = "Textures\\right.jpg", const char* top = "Textures\\top.jpg", const char* bottom = "Textures\\bottom.jpg", const char* front = "Textures\\front.jpg", const char* back = "Textures\\back.jpg");
		};

		CubeMapTexture* cubeMapTextures;
		SkyBox *skyBox;
	public:

		SkyBoxCubeMapProgram();
		virtual ~SkyBoxCubeMapProgram();

		bool LoadTextures(const char* left = "Textures\\left.jpg", const char* right = "Textures\\right.jpg", const char* top = "Textures\\top.jpg", const char* bottom = "Textures\\bottom.jpg", const char* front = "Textures\\front.jpg", const char* back = "Textures\\back.jpg");

		virtual void Draw(const glm::mat4& mat) override;
	};

}