#include "stdafx.h"
#include "SpaceshipProgram.h"


#include "SolarSystem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define GLSL(src) "#version 330\n" #src


namespace OpenGL {

	SpaceshipProgram::SpaceshipProgram()
		: nrlights(0), matLocation(0), modelMatLocation(0), transpInvModelMatLocation(0)
	{	
	}

	SpaceshipProgram::~SpaceshipProgram()
	{

	}

	void SpaceshipProgram::SetupLights(BodyPropList& BodyProperties)
	{
		//	nrlights = 0;
		lights.clear();


		for (const auto& body : BodyProperties)
			if (body.isSun) {
				lights.emplace_back(Light());
			}

		if (0 == nrlights) lights.emplace_back(Light());

		// lights uniforms positions

		if (nrlights == 0)
		{
			lights[0].attenPos = glGetUniformLocation(getID(), "Lights[0].lightDir");
			lights[0].lightDirPos = glGetUniformLocation(getID(), "Lights[0].atten");
		}
		else
		{
			USES_CONVERSION;

			for (unsigned int light = 0; light < nrlights; ++light)
			{
				CString param;

				param.Format(L"Lights[%d].lightDir", light);
				lights[light].lightDirPos = glGetUniformLocation(getID(), W2A(param));

				param.Format(L"Lights[%d].atten", light);
				lights[light].attenPos = glGetUniformLocation(getID(), W2A(param));
			}
		}
	}



	bool SpaceshipProgram::SetShaders(unsigned int nrLights)
	{
		nrlights = nrLights;

		if (!SetupVertexShader()) return false;
		else if (!SetupFragmentShader()) return false;

		Bind();

		if (getStatus() == false)
		{
			AfxMessageBox(CString("Shaders compile: ") + CString(getStatusMessage()));
			theApp.PostThreadMessage(WM_QUIT, 0, 0);
			return false;
		}

		getUniformsLocations();

		return true;
	}

	void SpaceshipProgram::getUniformsLocations()
	{
		// vertex shader parameters
		matLocation = glGetUniformLocation(getID(), "transformMat");
		modelMatLocation = glGetUniformLocation(getID(), "modelMat");
		transpInvModelMatLocation = glGetUniformLocation(getID(), "transpInvModelMat");

		// fragment shader uniform parameters

		illuminationLocation = glGetUniformLocation(getID(), "illumination");

		ambientColorLocation = glGetUniformLocation(getID(), "ambientColor");  //Ka
		diffuseColorLocation = glGetUniformLocation(getID(), "diffuseColor");  //Kd
		specularColorLocation = glGetUniformLocation(getID(), "specularColor"); //Ks

		exponentLocation = glGetUniformLocation(getID(), "exponent");

		useAmbientTextureLocation = glGetUniformLocation(getID(), "useAmbientTexture");
		useDiffuseTextureLocation = glGetUniformLocation(getID(), "useDiffuseTexture");
		useSpecularTextureLocation = glGetUniformLocation(getID(), "useSpecularTexture");
		useExponentTextureLocation = glGetUniformLocation(getID(), "useExponentTexture");
		useBumpTextureLocation = glGetUniformLocation(getID(), "useBumpTexture");

		ambientTextureLocation = glGetUniformLocation(getID(), "ambientTexture"); // map_Ka
		diffuseTextureLocation = glGetUniformLocation(getID(), "diffuseTexture"); // map_Kd
		specularTextureLocation = glGetUniformLocation(getID(), "specularTexture"); // map_Ks

		exponentTextureLocation = glGetUniformLocation(getID(), "exponentTexture"); // map_Ns
		bumpTextureLocation = glGetUniformLocation(getID(), "bumpTexture"); // map_bump or bump
	}

	bool SpaceshipProgram::SetupVertexShader()
	{
		OpenGL::VertexShader vertexShader;

		vertexShader.setSource(GLSL(
			precision highp float;

			layout(location = 0) in vec3 position;
			layout(location = 1) in vec3 normal;
			layout(location = 2) in vec2 texCoord;
			//layout(location = 3) in vec3 tangent;

			uniform mat4 transformMat;
			uniform mat4 modelMat;
			uniform mat3 transpInvModelMat;

			out vec2 TexCoord;
			out vec3 FragPos;
			out vec3 Normal;
			//out vec3 Tangent;
			//out vec3 Bitangent;

			void main()
			{
				gl_Position = transformMat * modelMat * vec4(position.x, position.y, position.z, 1.0);
				TexCoord = texCoord;
				FragPos = vec3(modelMat * vec4(position, 1.0f));

				Normal = normalize(transpInvModelMat * normal);
				//Tangent = normalize(transpInvModelMat * tangent);
			}
		));

		if (vertexShader.getStatus() == false)
		{
			AfxMessageBox(CString("Vertex shader: ") + CString(vertexShader.getStatusMessage()));
			theApp.PostThreadMessage(WM_QUIT, 0, 0);
			return false;
		}

		Attach(vertexShader);

		return true;
	}

	bool SpaceshipProgram::SetupFragmentShader()
	{
		OpenGL::FragmentShader fragmentShader;

		// just white for now, but it will become way more complex
		CString shaderSrc;
		shaderSrc.Format(CString(GLSL(
			\n#define NRLIGHTS % d\n

			precision highp float;

			struct Light {
				vec3 lightDir;
				float atten;
			};
			uniform Light Lights[NRLIGHTS];

			uniform int illumination;

			uniform vec3 ambientColor;  //Ka
			uniform vec3 diffuseColor;  //Kd
			uniform vec3 specularColor; //Ks

			uniform float exponent;

			uniform int useAmbientTexture;
			uniform int useDiffuseTexture;
			uniform int useSpecularTexture;
			uniform int useExponentTexture;
			uniform int useBumpTexture;

			uniform sampler2D ambientTexture; // map_Ka
			uniform sampler2D diffuseTexture; // map_Kd
			uniform sampler2D specularTexture; // map_Ks

			uniform sampler2D exponentTexture; // map_Ns
			uniform sampler2D bumpTexture; // map_bump or bump

			out vec4 outputColor;
			
		    in vec2 TexCoord;
			in vec3 FragPos;
			in vec3 Normal;

			void main()
			{
				vec4 color;
				
				if (useDiffuseTexture == 1) color = texture(diffuseTexture, TexCoord) * vec4(diffuseColor, 1.0f);
				else color = vec4(diffuseColor, 1.0f);

				if (useAmbientTexture == 1) color = color + texture(ambientTexture, TexCoord) * vec4(ambientColor, 1.0f);
				else color = color + vec4(diffuseColor, 1.0f);


				outputColor = color;
			}
		)), nrlights == 0 ? 1 : nrlights);

		USES_CONVERSION;
		fragmentShader.setSource(W2A(shaderSrc));

		if (fragmentShader.getStatus() == false)
		{
			AfxMessageBox(CString("Fragment shader: ") + CString(fragmentShader.getStatusMessage()));
			theApp.PostThreadMessage(WM_QUIT, 0, 0);
			return false;
		}

		Attach(fragmentShader);

		return true;
	}


}
