#include "stdafx.h"
#include "SkyBoxCubeMapProgram.h"


#include <gtc\type_ptr.hpp>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define GLSL(src) "#version 330\n" #src


namespace OpenGL {

	SkyBoxCubeMapProgram::SkyBoxCubeMapProgram()
		: skyBox(NULL), cubeMapTextures(NULL)
	{
	}


	SkyBoxCubeMapProgram::~SkyBoxCubeMapProgram()
	{
		delete skyBox;
		delete cubeMapTextures;
	}


	bool SkyBoxCubeMapProgram::LoadTextures(const char* left, const char* right, const char* top, const char* bottom, const char* front, const char* back)
	{
		skyBox = new SkyBox();
		cubeMapTextures = new CubeMapTexture();
		if (!cubeMapTextures->LoadTextures(left,right,top,bottom,front,back))
		{
			delete skyBox;
			delete cubeMapTextures;

			skyBox = NULL;
			cubeMapTextures = NULL;

			return false;
		}

		return true;
	}


	void SkyBoxCubeMapProgram::Draw(const glm::mat4& mat)
	{
		glDepthMask(GL_FALSE);
		glDisable(GL_DEPTH_TEST);
		Use();

		glUniformMatrix4fv(transformMatLoc, 1, GL_FALSE, value_ptr(mat));

		skyBox->Bind();
		cubeMapTextures->Bind();

		skyBox->Draw();

		skyBox->UnBind();
		cubeMapTextures->UnBind();
		UnUse();
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
	}


	SkyBoxCubeMapProgram::SkyBox::SkyBox()
	{
		const GLfloat skyBoxVert[] = {
			// Positions          
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			1.0f,  1.0f, -1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			1.0f, -1.0f,  1.0f
		};

		Renderable::Bind();
		setData((void*)skyBoxVert, sizeof(skyBoxVert));

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

		UnBind();
	}


	void SkyBoxCubeMapProgram::SkyBox::Draw()
	{
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}


	SkyBoxCubeMapProgram::CubeMapTexture::CubeMapTexture()
	{
		glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &ID);
	}

	SkyBoxCubeMapProgram::CubeMapTexture::~CubeMapTexture()
	{
		UnBind();
		glDeleteTextures(1, &ID);
	}

	void SkyBoxCubeMapProgram::CubeMapTexture::Bind()
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(getType(), ID);
	}

	void SkyBoxCubeMapProgram::CubeMapTexture::UnBind()
	{
		glBindTexture(getType(), 0);
	}

	GLuint SkyBoxCubeMapProgram::CubeMapTexture::getType() const
	{
		return GL_TEXTURE_CUBE_MAP;
	}

	void SkyBoxCubeMapProgram::CubeMapTexture::setDataLeft(const void *data, int width, int height, int bpp)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, bpp == 24 ? 3 : 4, width, height, 0, bpp == 24 ? GL_BGR_EXT : GL_BGRA_EXT, GL_UNSIGNED_BYTE, data);
	}

	void SkyBoxCubeMapProgram::CubeMapTexture::setDataRight(const void *data, int width, int height, int bpp)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, bpp == 24 ? 3 : 4, width, height, 0, bpp == 24 ? GL_BGR_EXT : GL_BGRA_EXT, GL_UNSIGNED_BYTE, data);
	}

	void SkyBoxCubeMapProgram::CubeMapTexture::setDataTop(const void *data, int width, int height, int bpp)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, bpp == 24 ? 3 : 4, width, height, 0, bpp == 24 ? GL_BGR_EXT : GL_BGRA_EXT, GL_UNSIGNED_BYTE, data);
	}

	void SkyBoxCubeMapProgram::CubeMapTexture::setDataBottom(const void *data, int width, int height, int bpp)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, bpp == 24 ? 3 : 4, width, height, 0, bpp == 24 ? GL_BGR_EXT : GL_BGRA_EXT, GL_UNSIGNED_BYTE, data);
	}

	void SkyBoxCubeMapProgram::CubeMapTexture::setDataFront(const void *data, int width, int height, int bpp)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, bpp == 24 ? 3 : 4, width, height, 0, bpp == 24 ? GL_BGR_EXT : GL_BGRA_EXT, GL_UNSIGNED_BYTE, data);
	}

	void SkyBoxCubeMapProgram::CubeMapTexture::setDataBack(const void *data, int width, int height, int bpp)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, bpp == 24 ? 3 : 4, width, height, 0, bpp == 24 ? GL_BGR_EXT : GL_BGRA_EXT, GL_UNSIGNED_BYTE, data);
	}

#define LoadSkinBegin(name) { \
	CImage skin; \
	skin.Load(CString(name)); \
	if (skin.IsNull()) throw new CFileException(); \
	unsigned char* buf = (unsigned char*)skin.GetBits(); \
	int bpp = skin.GetBPP(); \
	if (skin.GetPitch() < 0) \
		buf = (unsigned char*)skin.GetPixelAddress(0, skin.GetHeight() - 1); \
	else \
		buf = static_cast<unsigned char*>(skin.GetBits());


#define LoadSkinEnd(funct) funct(buf, skin.GetWidth(), skin.GetHeight(), bpp); }

	bool SkyBoxCubeMapProgram::CubeMapTexture::LoadTextures(const char* left, const char* right, const char* top, const char* bottom, const char* front, const char* back)
	{
		Bind();

		try {
			LoadSkinBegin(left);
			LoadSkinEnd(setDataLeft);
			LoadSkinBegin(right);
			LoadSkinEnd(setDataRight);
			LoadSkinBegin(top);
			LoadSkinEnd(setDataTop);
			LoadSkinBegin(bottom);
			LoadSkinEnd(setDataBottom);
			LoadSkinBegin(front);
			LoadSkinEnd(setDataFront);
			LoadSkinBegin(back);
			LoadSkinEnd(setDataBack);
		}
		catch (CFileException *e)
		{
			e->Delete();
			try
			{
				// try once again by replacing '.jpg' with '.png'
				{
					CString name(left);
					name.MakeLower();
					name.Replace(_T(".jpg"), _T(".png"));

					LoadSkinBegin(name);
					LoadSkinEnd(setDataLeft);
				}
				{
					CString name(right);
					name.MakeLower();
					name.Replace(_T(".jpg"), _T(".png"));

					LoadSkinBegin(name);
					LoadSkinEnd(setDataRight);
				}
				{
					CString name(top);
					name.MakeLower();
					name.Replace(_T(".jpg"), _T(".png"));

					LoadSkinBegin(name);
					LoadSkinEnd(setDataTop);
				}
				{
					CString name(bottom);
					name.MakeLower();
					name.Replace(_T(".jpg"), _T(".png"));

					LoadSkinBegin(name);
					LoadSkinEnd(setDataBottom);
				}
				{
					CString name(front);
					name.MakeLower();
					name.Replace(_T(".jpg"), _T(".png"));

					LoadSkinBegin(name);
					LoadSkinEnd(setDataFront);
				}
				{
					CString name(back);
					name.MakeLower();
					name.Replace(_T(".jpg"), _T(".png"));

					LoadSkinBegin(name);
					LoadSkinEnd(setDataBack);
				}
			}
			catch (CFileException* e)
			{
				e->Delete();

				return false;
			}
			catch (...)
			{
				return false;
			}
		}
		catch (...)
		{
			return false;
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);

		return true;
	}

	bool SkyBoxCubeMapProgram::SetShaders()
	{
		VertexShader vertexShader;

		vertexShader.setSource(GLSL(
			precision highp float;

			layout(location = 0) in vec3 position;
			out vec3 TexCoords;

			uniform mat4 transformMat;

			void main()
			{
				vec4 pos = transformMat * vec4(position, 1.0);
				gl_Position = pos.xyww;
				TexCoords = position;
			}

		));


		if (vertexShader.getStatus() == false)
		{
			AfxMessageBox(CString("SkyBox vertex shader: ") + CString(vertexShader.getStatusMessage()));
			return false;
		}

		FragmentShader fragmentShader;

		fragmentShader.setSource(GLSL(
			precision highp float;

			in vec3 TexCoords;
			out vec4 outputColor;

			uniform samplerCube Texture;

			void main()
			{
				outputColor = texture(Texture, TexCoords);
			}

		));


		if (fragmentShader.getStatus() == false)
		{
			AfxMessageBox(CString("SkyBox fragment shader: ") + CString(fragmentShader.getStatusMessage()));
			return false;
		}

		Attach(vertexShader);
		Attach(fragmentShader);

		SkyBase::Bind();

		transformMatLoc = glGetUniformLocation(*this, "transformMat");

		return true;
	}
}