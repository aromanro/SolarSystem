#pragma once
#include "OpenGLObject.h"

namespace OpenGL {

	class Texture :
		public OpenGLObject
	{
	public:
		Texture();
		virtual ~Texture();


		virtual void Bind() override;
		void Bind(int nr);

		virtual void UnBind() override;
		virtual GLuint getType() const override;

		void setData(const void *data, int width, int height, int nr = 0, int nrBytes = 3);
		void GenerateMipmaps();
	};

}
