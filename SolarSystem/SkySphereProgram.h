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
			SkySphere(float r = 1., unsigned int lats = 32, unsigned int longs = 32);

			void Draw() override;
		};

		Texture* sphereTexture;
		SkySphere* skySphere;

		GLint textureLoc;

		bool LoadTextureData(const CString& filename);

	public:
		SkySphereProgram();
		~SkySphereProgram() override;

		bool LoadTexture(const char* name = "Textures\\skysphere.jpg");

		void Draw(const glm::mat4& mat) override;
		bool SetShaders() override;
	};

}

