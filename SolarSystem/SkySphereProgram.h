#pragma once


#include "Program.h"
#include "Texture.h"
#include "VertexBufferRenderable.h"

#include "SkyBase.h"

#include <glm.hpp>

namespace OpenGL {

	class SkySphereProgram : public SkyBase
	{
	protected:
		class SkySphere : public VertexBufferRenderable
		{
		protected:
			unsigned int m_lats;
			unsigned int m_longs;
		public:
			SkySphere(float r = 1., unsigned int lats = 64, unsigned int longs = 64);

			virtual void Draw() override;
		};

		Texture* sphereTexture;
		SkySphere* skySphere;

		GLint textureLoc;

		bool LoadTextureData(const CString& filename);
	public:
		SkySphereProgram();
		virtual ~SkySphereProgram();

		bool LoadTexture(const char* name = "Textures\\skysphere.jpg");

		virtual void Draw(const glm::mat4& mat) override;
		virtual bool SetShaders() override;
	};

}

