#include "stdafx.h"
#include "SolarSystemGLProgram.h"

#include "SolarSystem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define GLSL(src) "#version 330\n" #src


SolarSystemGLProgram::SolarSystemGLProgram()
	: nrlights(0), matLocation(0), modelMatLocation(0), transpInvModelMatLocation(0), 
	colorLocation(0), useTextLocation(0), useTransparentTextLocation(0), useShadowTextLocation(0), useSpecularTextLocation(0), useNormalTextLocation(0), alphaInTransparentTexture(0), isSunLocation(0), useAlphaBlend(0), viewPosLocation(0), farPlaneLoc(0),
	lightPosLoc(0), calcShadowsLoc(0), textureLoc(0), transparentTextureLoc(0), shadowTextureLoc(0), specularTextureLoc(0), normalTextureLoc(0), depthMapLoc(0)
{
}


SolarSystemGLProgram::~SolarSystemGLProgram()
{
}


void SolarSystemGLProgram::SetupLights(BodyPropList& BodyProperties)
{
//	nrlights = 0;
	lights.clear();


	for (const auto &body : BodyProperties)
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

void SolarSystemGLProgram::getUniformsLocations()
{
	// vertex shader parameters
	matLocation = glGetUniformLocation(getID(), "transformMat");
	modelMatLocation = glGetUniformLocation(getID(), "modelMat");
	transpInvModelMatLocation = glGetUniformLocation(getID(), "transpInvModelMat");

	// fragment shader uniform parameters
	colorLocation = glGetUniformLocation(getID(), "theColor");
	useTextLocation = glGetUniformLocation(getID(), "UseTexture");
	useTransparentTextLocation = glGetUniformLocation(getID(), "UseTransparentTexture");
	useShadowTextLocation = glGetUniformLocation(getID(), "UseShadowTexture");
	useSpecularTextLocation = glGetUniformLocation(getID(), "UseSpecularTexture");
	useNormalTextLocation = glGetUniformLocation(getID(), "UseNormalTexture");

	alphaInTransparentTexture = glGetUniformLocation(getID(), "alphaInTransparentTexture");

	isSunLocation = glGetUniformLocation(getID(), "IsSun");
	useAlphaBlend = glGetUniformLocation(getID(), "UseAlphaBlend");
	viewPosLocation = glGetUniformLocation(getID(), "viewPos");

	farPlaneLoc = glGetUniformLocation(getID(), "farPlane");
	lightPosLoc = glGetUniformLocation(getID(), "lightPos");
	calcShadowsLoc = glGetUniformLocation(getID(), "calcShadows");

	textureLoc = glGetUniformLocation(getID(), "Texture");
	transparentTextureLoc = glGetUniformLocation(getID(), "transparentTexture");
	shadowTextureLoc = glGetUniformLocation(getID(), "shadowTexture");
	specularTextureLoc = glGetUniformLocation(getID(), "specularTexture");
	normalTextureLoc = glGetUniformLocation(getID(), "normalTexture");

	depthMapLoc = glGetUniformLocation(getID(), "depthMap");
}

bool SolarSystemGLProgram::SetupShaders(unsigned int nrLights)
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


bool SolarSystemGLProgram::SetupVertexShader()
{
	OpenGL::VertexShader vertexShader;

	vertexShader.setSource(GLSL(
		layout(location = 0) in vec3 position;
		layout(location = 1) in vec3 normal;
		layout(location = 2) in vec2 texCoord;
		layout(location = 3) in vec3 tangent;

		uniform mat4 transformMat;
		uniform mat4 modelMat;
		uniform mat3 transpInvModelMat;

		out vec2 TexCoord;
		out vec3 FragPos;
		out vec3 Normal;
		out vec3 Tangent;

		void main()
		{
			gl_Position = transformMat * modelMat * vec4(position.x, position.y, position.z, 1.0);
			TexCoord = texCoord;
			FragPos = vec3(modelMat * vec4(position, 1.0f));			
			
			Normal = normalize(transpInvModelMat * normal);
			Tangent = normalize(transpInvModelMat * tangent);
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


bool SolarSystemGLProgram::SetupFragmentShader()
{
	OpenGL::FragmentShader fragmentShader;

	CString shaderSrc;
	shaderSrc.Format(CString(GLSL(
		\n#define NRLIGHTS %d\n

		struct Light {
			vec3 lightDir;
			float atten;
		};
		uniform Light Lights[NRLIGHTS];

		uniform vec4 theColor;
		
		uniform int UseTexture;
		uniform int UseTransparentTexture;
		uniform int UseShadowTexture;
		uniform int UseSpecularTexture;
		uniform int UseNormalTexture;

		uniform int alphaInTransparentTexture;

		uniform int IsSun;
		uniform int UseAlphaBlend;
		uniform vec3 viewPos;
		uniform float farPlane;
		uniform vec3 lightPos;

		in vec2 TexCoord;
		in vec3 FragPos;
		in vec3 Normal;
		in vec3 Tangent;

		out vec4 outputColor;

		uniform sampler2D Texture;
		uniform sampler2D transparentTexture;
		uniform sampler2D shadowTexture;
		uniform sampler2D specularTexture;
		uniform sampler2D normalTexture;

		uniform int calcShadows;
		uniform samplerCube depthMap;

		// Returns a random number - this is found in several places on internet, either in the vec2 or vec3 variant
		// but with different numbers...
		float random(in vec3 seed, in int i)
		{
			return fract(sin(dot(vec4(seed, i), vec4(23.661, 2999.6053, 7.6703, 7229.4241))) * 8747.4231);
		}

		float CalcShadow(in float cosAngle, in float viewDist)
		{
			// Get vector between fragment position and light position
			vec3 fragToLight = FragPos - lightPos;
			// Get current linear depth as the length between the fragment and light position
			float currentDepth = length(fragToLight);

			float shadow = 0.0;
			float closestDepth;

			float bias = clamp(0.05 * tan(acos(cosAngle)), 0, 0.05);

			int samples = 16;

			float diskRadius = (1.0 + 50.0 * (viewDist / farPlane)) / 8.0;
			for (int i = 0; i < samples; ++i)
			{
				// most of fragments are either deep in the shadow or in the light
				// bail out if six samples say it's either shadow or light
				// this avoids having 16 samples for each fragment
				// for most of them only 6 samples should be enough
				if (6 == i) {
					if (shadow > 5.9) return 0.5;
					else if (shadow < 0.1) return 0.0;
				}

				// pick three random values for directions in space
				float dir1 = random(FragPos, samples * i);
				float dir2 = random(FragPos, samples * i + 1);
				float dir3 = random(FragPos, samples * i + 2);

				// pick three random signs for them
				dir1 *= (random(FragPos, samples * i + 3) > 0.5 ? -1 : 1);
				dir2 *= (random(FragPos, samples * i + 4) > 0.5 ? -1 : 1);
				dir3 *= (random(FragPos, samples * i + 5) > 0.5 ? -1 : 1);

				// make a random vector out of them
				vec3 radial = vec3(dir1, dir2, dir3);

				// sample from a sphere with radius at most diskRadius
				if (length(radial) != 0) radial = normalize(radial) * diskRadius * random(FragPos, i * samples + 6);

				closestDepth = texture(depthMap, fragToLight + radial).r * farPlane;
				if (currentDepth - bias > closestDepth)	shadow += 1.0;
			}

			shadow = shadow * 0.5 / float(samples);

			return shadow > 0.4 ? shadow : 0;
		}

		vec3 CalcLight(in vec3 lightDir, in vec3 viewDir, in vec3 normal, in vec3 color, in vec4 transparentColor)
		{
			// Diffuse shading
			float proj = dot(normal, lightDir);
			float diff = max(proj, 0.0);

			// Specular shading
			vec3 halfwayDir = normalize(lightDir + viewDir);
			float val = max(dot(normal, halfwayDir), 0.0);

			vec3 firstLayerColor;
			vec3 transparentLayerColor;

			// this is only for normal mapping - it's applied on the usual texture and on the one used for shadow, but not on the transparent one
			if (1 == UseNormalTexture) // don't use normal mapping if on the shadow side
			{
				// re-orhogonalize
				vec3 tangent = normalize(Tangent - dot(Tangent, normal) * normal);
				vec3 bitangent = normalize(cross(normal, tangent));

				mat3 TBN = mat3(tangent, bitangent, normal);

				vec3 normalMapped = texture(normalTexture, TexCoord).rgb * 2. - 1.;
				normalMapped = normalize(TBN * normalMapped);

				// this makes bump mapping go away faster beyond terminator, otherwise it looks odd
				if (proj < 0)
					normalMapped = (1. + proj) * normalMapped - proj * normal;
				
				float diffMapped = max(dot(normalMapped, lightDir), 0.0);
				float valMapped = max(dot(normalMapped, halfwayDir), 0.0);

				if (1 == UseSpecularTexture)
				{
					float spec3 = pow(valMapped, 48);

					float specProc = texture(specularTexture, TexCoord)[0];
					float oneMinusProc = 1. - specProc;
					firstLayerColor = (oneMinusProc * diffMapped + specProc * spec3) * color;
				}
				else
				{
					float specMapped = pow(valMapped, 32);
					firstLayerColor = (0.7 * diffMapped + 0.3 * specMapped) * color;
				}
			}
			else
			{
				if (1 == UseSpecularTexture)
				{
					float spec3 = pow(val, 48);

					float specProc = texture(specularTexture, TexCoord)[0];
					float oneMinusProc = 1. - specProc;
					firstLayerColor = (oneMinusProc * diff + specProc * spec3) * color;
				}
				else
				{
					float spec = pow(val, 32);
					firstLayerColor = (0.7 * diff + 0.3 * spec) * color;
				}
			}

			float spec2 = pow(val, 16); // different 'specular' for the transparent color - typically clouds
			transparentLayerColor = (0.8 * diff + 0.2 * spec2) * transparentColor.xyz;

			float alphaColor;
			float alphaTransparent;

			if (1 == UseTexture && 1 == UseTransparentTexture) 
			{
				if (1 == alphaInTransparentTexture)
				{
					alphaTransparent = transparentColor[3];
					alphaColor = 1. - alphaTransparent;
				}
				else
				{
					alphaColor = 0.5;
					alphaTransparent = 0.5;
				}
			}
			else
			{
				alphaColor = 1.;
				alphaTransparent = 0.;
			}

			// Combine results								
			return alphaColor * firstLayerColor + alphaTransparent * transparentLayerColor;
		}

		void main()
		{
			vec4 color = (UseTexture == 1 ? texture(Texture, TexCoord) : theColor);

			vec3 light;

			if (1 == IsSun)
				light = color.xyz;
			else
			{
				vec3 viewVec = viewPos - FragPos;
				vec3 viewDir = normalize(viewVec);
				vec3 normal = normalize(Normal);

				// shadow
				float shadow = 0.0;
				if (1 == calcShadows)
				{
					float cosAngle = clamp(dot(normal, viewDir), 0.0, 1.0);
					shadow = CalcShadow(cosAngle, length(viewVec));
				}

				if (1 == UseShadowTexture)
				{
					float proj = dot(normalize(Lights[0].lightDir), normal);
					if (shadow > 0. || proj < 0)
					{
						vec4 shadowColor = texture(shadowTexture, TexCoord);

						// just a trick to make the lights in the shadow texture light up
						// this way the lights at night on Earth can be seen much better
						int lightUp = 0;
						if (shadowColor.x > 0.6)
							++lightUp;
						if (shadowColor.y > 0.6)
							++lightUp;
						if (shadowColor.z > 0.6)
							++lightUp;
						if (lightUp >= 2)
							shadowColor = shadowColor * 10;
											
						// without this 'trick' the contrast at terminator was too big
						// at the terminator the angle between light and normal is 90 degrees, so the projection is 0
						// then it goes negative up to -1 and so on
						// this progressively mixes the normal layer with the shadow one
						// I'm sure a better way could be found, but for now it's good enough for me
						proj = abs(proj);
						color = (1. - proj) * color + proj * shadowColor; // if I don't do this for shadow, too, the eclipse is not ok at the terminator
					}
				}

				// use transparent layer?
				// this is only for adding a clouds layer on Earth

				float alphaColor;
				float alphaTransparent;

				vec4 transparentColor;
				if (1 == UseTransparentTexture)
				{
					transparentColor = texture(transparentTexture, TexCoord);
					if (1 == alphaInTransparentTexture)
					{
						alphaTransparent = transparentColor[3];
						alphaColor = 1. - alphaTransparent;
					}
					else
					{
						alphaColor = 0.5;
						alphaTransparent = 0.5;
					}
				}
				else
				{
					alphaColor = 1.;
					alphaTransparent = 0.;
					transparentColor = color;
				}

				vec4 mixedColor = alphaColor * color + alphaTransparent * transparentColor;

				// ambient
				light = 0.1 * mixedColor.xyz;

				for (int i = 0; i < NRLIGHTS; ++i)
				{
					if (i > 0) shadow = 0;

					light += Lights[i].atten * (1.0 - shadow) * CalcLight(normalize(Lights[i].lightDir), viewDir, normal, color.xyz, transparentColor);
				}

				light = clamp(light, 0, 1);
			}

			// TODO: implement better gamma correction than the existing one
			//float gamma = 2.2;
			//light = pow(light, vec3(1.0 / gamma));

			// also allow alpha blending, useful if I'll add a billboard
			//outputColor = vec4(light, UseAlphaBlend == 1 ? theColor[3] : 1); // use this if you want to show the whole billboard, but transparent						
			
			// use this if you only want to show the alpha blended text (no visible billboard, just the transparent text)
			int transparent = (color[0] == 0 && color[1] == 0 && color[2] == 0) ? 1 : 0;
			outputColor = vec4(light, UseAlphaBlend == 1 ? (transparent == 1 ? 0 : theColor[3]) : 1); 
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
