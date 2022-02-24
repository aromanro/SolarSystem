#include "stdafx.h"
#include "SkySphereProgram.h"

#include "Constants.h"

#include <gtc\type_ptr.hpp>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define GLSL(src) "#version 330\n" #src


namespace OpenGL {

	SkySphereProgram::SkySphere::SkySphere(float r, unsigned int lats, unsigned int longs)
		: m_lats(lats), m_longs(longs)
	{
		std::vector<GLfloat> buffer;
		static const int STRIDE_SIZE = 5; // 3 for vertex, 2 for texture coordinates
		static const int STEP_SIZE = 2 * STRIDE_SIZE; // 2 vertices at once

		unsigned int length = (lats + 1) * (longs + 1) * STEP_SIZE;
		buffer.resize(length);

		for (size_t i = 0; i < lats; ++i) {
			const double lat0 = M_PI * (-0.5 + static_cast<double>(i) / lats);
			const double z0 = r * sin(lat0);
			const double r0 = r * cos(lat0);

			const double lat1 = M_PI * (-0.5 + (i + 1.) / lats);
			const double z1 = r * sin(lat1);
			const double r1 = r * cos(lat1);

			for (size_t j = 0; j <= longs; ++j) {
				const double longitude = 2. * M_PI * static_cast<double>(j) / longs;

				const double c = cos(longitude);
				const double s = sin(longitude);

				double x0 = r0 * c;
				double y0 = r0 * s;

				double XTex = static_cast<double>(j) / longs;
				double YTex0 = static_cast<double>(i) / lats;
				double YTex1 = (static_cast<double>(i) + 1.) / lats;

				double x1 = r1 * c;
				double y1 = r1 * s;

				// first vertex

				// vertex
				const size_t base = i * (longs + 1ULL) * STEP_SIZE + j * STEP_SIZE;
				buffer[base] = static_cast<float>(x1);
				buffer[base + 1] = static_cast<float>(y1);
				buffer[base + 2] = static_cast<float>(z1);

				// texture coordinate
				buffer[base + 3] = static_cast<float>(XTex);
				buffer[base + 4] = static_cast<float>(YTex1);

				// second vertex

				// vertex 
				buffer[base + STRIDE_SIZE] = static_cast<float>(x0);
				buffer[base + STRIDE_SIZE + 1] = static_cast<float>(y0);
				buffer[base + STRIDE_SIZE + 2] = static_cast<float>(z0);

				// texture coordinate
				buffer[base + STRIDE_SIZE + 3] = static_cast<float>(XTex);
				buffer[base + STRIDE_SIZE + 4] = static_cast<float>(YTex0);
			}
		}

		Renderable::Bind();
		setData(buffer.data(), length * sizeof(GLfloat));

		// vertices
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, STRIDE_SIZE * sizeof(GLfloat), (GLvoid*)0);

		// texture coordinates
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, STRIDE_SIZE * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

		Renderable::UnBind();
	}

	void SkySphereProgram::SkySphere::Draw()
	{
		VertexBufferRenderable::Bind();

		for (unsigned int i = 0; i < m_lats; ++i)
			glDrawArrays(GL_TRIANGLE_STRIP, static_cast<GLint>(i * (m_longs + 1) * 2), static_cast<GLsizei>(2 * (m_longs + 1)));
	}


	SkySphereProgram::SkySphereProgram()
		: skySphere(NULL), sphereTexture(NULL), textureLoc(0)
	{
	}


	SkySphereProgram::~SkySphereProgram()
	{
		delete skySphere;
		delete sphereTexture;
	}

	bool SkySphereProgram::LoadTextureData(const CString& filename)
	{
		CImage skin;
		skin.Load(CString(filename));
		if (skin.IsNull()) throw new CFileException();

		unsigned char* buf = (unsigned char*)skin.GetBits();
		int bpp = skin.GetBPP();
		if (skin.GetPitch() < 0)
			buf = (unsigned char*)skin.GetPixelAddress(0, skin.GetHeight() - 1);
		else
			buf = static_cast<unsigned char*>(skin.GetBits());

		sphereTexture->setData(buf, skin.GetWidth(), skin.GetHeight(), 0, bpp == 24 ? 3 : 4);

		return true;
	}




	bool SkySphereProgram::LoadTexture(const char* filename)
	{
		skySphere = new SkySphere();
		sphereTexture = new Texture();

		try {
			LoadTextureData(CString(filename));
		}
		catch (CFileException* e)
		{
			e->Delete();
			try
			{
				CString name(filename);
				name.MakeLower();
				name.Replace(_T(".jpg"), _T(".png"));
				LoadTextureData(CString(name));
			}
			catch (CFileException* e)
			{
				e->Delete();

				delete skySphere;
				delete sphereTexture;

				skySphere = NULL;
				sphereTexture = NULL;

				return false;
			}
			catch (...)
			{
				delete skySphere;
				delete sphereTexture;

				skySphere = NULL;
				sphereTexture = NULL;

				return false;
			}
		}
		catch (...)
		{
			delete skySphere;
			delete sphereTexture;

			skySphere = NULL;
			sphereTexture = NULL;

			return false;
		}

		return true;
	}


	void SkySphereProgram::Draw(const glm::mat4& mat)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glPolygonMode(GL_BACK, GL_FILL);
		glDepthMask(GL_FALSE);
		glDisable(GL_DEPTH_TEST);
		Use();

		glUniform1i(glGetUniformLocation(getID(), "Texture"), 0);

		glUniform1i(textureLoc, 0);
		glUniformMatrix4fv(transformMatLoc, 1, GL_FALSE, value_ptr(mat));

		skySphere->Bind();
		sphereTexture->Bind();

		skySphere->Draw();

		skySphere->UnBind();
		sphereTexture->UnBind();
		UnUse();
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glCullFace(GL_BACK);
		glPolygonMode(GL_FRONT, GL_FILL);
	}

	bool SkySphereProgram::SetShaders()
	{
		VertexShader vertexShader;

		vertexShader.setSource(GLSL(
			layout(location = 0) in vec3 position;
			layout(location = 1) in vec2 texCoord;
			out vec2 TexCoords;

			uniform mat4 transformMat;

			void main()
			{
				vec4 pos = transformMat * vec4(position, 1.0);
				gl_Position = pos.xyww;
				TexCoords = texCoord;
			}

		));


		if (vertexShader.getStatus() == false)
		{
			AfxMessageBox(CString("SkySphere vertex shader: ") + CString(vertexShader.getStatusMessage()));
			return false;
		}

		FragmentShader fragmentShader;

		fragmentShader.setSource(GLSL(
			in vec2 TexCoords;
			out vec4 outputColor;
			uniform sampler2D Texture;

			void main()
			{
				outputColor = texture(Texture, TexCoords);
			}

		));


		if (fragmentShader.getStatus() == false)
		{
			AfxMessageBox(CString("SkySphere fragment shader: ") + CString(fragmentShader.getStatusMessage()));
			return false;
		}

		Attach(vertexShader);
		Attach(fragmentShader);

		SkyBase::Bind();

		transformMatLoc = glGetUniformLocation(*this, "transformMat");
		textureLoc = glGetUniformLocation(getID(), "Texture");

		return true;
	}
}
