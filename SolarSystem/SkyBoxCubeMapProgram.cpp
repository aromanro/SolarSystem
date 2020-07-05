#include "stdafx.h"
#include "SkyBoxCubeMapProgram.h"


#include <gtc\type_ptr.hpp>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define GLSL(src) "#version 330\n" #src


namespace OpenGL {

	SkyBoxCubeMapProgram::SkyBoxCubeMapProgram()
		: skyBox(NULL), cubeMapTextures(NULL), transformMatLoc(-1)
	{
	}


	SkyBoxCubeMapProgram::~SkyBoxCubeMapProgram()
	{
		delete skyBox;
		delete cubeMapTextures;
	}

	bool SkyBoxCubeMapProgram::SetShaders()
	{
		VertexShader vertexShader;

		vertexShader.setSource(GLSL(

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
			AfxMessageBox(CString("CubeMap vertex shader: ") + CString(vertexShader.getStatusMessage()));
			return false;
		}

		FragmentShader fragmentShader;

		fragmentShader.setSource(GLSL(

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
			AfxMessageBox(CString("CubeMap fragment shader: ") + CString(fragmentShader.getStatusMessage()));
			return false;
		}

		Attach(vertexShader);
		Attach(fragmentShader);

		Program::Bind();

		transformMatLoc = glGetUniformLocation(*this, "transformMat");

		return true;
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


	void SkyBoxCubeMapProgram::Draw(glm::mat4& mat)
	{
		glDepthMask(GL_FALSE);
		Use();

		glUniformMatrix4fv(transformMatLoc, 1, GL_FALSE, value_ptr(mat));

		skyBox->Bind();
		cubeMapTextures->Bind();

		skyBox->Draw();

		skyBox->UnBind();
		cubeMapTextures->UnBind();
		UnUse();
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

		vao = new VertexArrayObject();
		vao->Bind();

		vbo = new VertexBufferObject();
		vbo->setData((void*)skyBoxVert, sizeof(skyBoxVert));

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

		UnBind();
	}


	SkyBoxCubeMapProgram::SkyBox::~SkyBox()
	{
		delete vbo;
		delete vao;
	}

	void SkyBoxCubeMapProgram::SkyBox::Draw()
	{
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	void SkyBoxCubeMapProgram::SkyBox::Bind()
	{
		vao->Bind();
		vbo->Bind();
	}

	void SkyBoxCubeMapProgram::SkyBox::UnBind()
	{
		vao->UnBind();
		vbo->UnBind();
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
		glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
	}

	void SkyBoxCubeMapProgram::CubeMapTexture::UnBind()
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	GLuint SkyBoxCubeMapProgram::CubeMapTexture::getType() const
	{
		return GL_TEXTURE_CUBE_MAP;
	}

	void SkyBoxCubeMapProgram::CubeMapTexture::setDataLeft(const void *data, int width, int height)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, 3, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);
	}	

	void SkyBoxCubeMapProgram::CubeMapTexture::setDataRight(const void *data, int width, int height)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, 3, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);
	}

	void SkyBoxCubeMapProgram::CubeMapTexture::setDataTop(const void *data, int width, int height)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, 3, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);
	}

	void SkyBoxCubeMapProgram::CubeMapTexture::setDataBottom(const void *data, int width, int height)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, 3, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);
	}

	void SkyBoxCubeMapProgram::CubeMapTexture::setDataFront(const void *data, int width, int height)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, 3, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);
	}

	void SkyBoxCubeMapProgram::CubeMapTexture::setDataBack(const void *data, int width, int height)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, 3, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);
	}

#define LoadSkinBegin(name) { \
	CImage skin; \
	skin.Load(CString(name)); \
	if (skin.IsNull()) throw new CFileException(); \
	unsigned char* buf = (unsigned char*)skin.GetBits(); \
	if (skin.GetPitch() < 0) \
		buf = (unsigned char*)skin.GetPixelAddress(0, skin.GetHeight() - 1);

#define LoadSkinEnd(funct) funct(buf, skin.GetWidth(), skin.GetHeight()); }

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

		return true;
	}
}