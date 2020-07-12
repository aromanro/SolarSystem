#include "stdafx.h"
#include "SkyBase.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define GLSL(src) "#version 330\n" #src

namespace OpenGL {

	SkyBase::SkyBase()
	: transformMatLoc(-1)
	{
	}

	SkyBase::~SkyBase()
	{
	}

	bool SkyBase::SetShaders()
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
			AfxMessageBox(CString("Sky vertex shader: ") + CString(vertexShader.getStatusMessage()));
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
			AfxMessageBox(CString("Sky fragment shader: ") + CString(fragmentShader.getStatusMessage()));
			return false;
		}

		Attach(vertexShader);
		Attach(fragmentShader);

		SkyBase::Bind();

		transformMatLoc = glGetUniformLocation(*this, "transformMat");

		return true;
	}

}
