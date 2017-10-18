#pragma once
#include "OpenGLObject.h"

namespace OpenGL {

	class Texture :
		public OpenGLObject
	{
	public:
		Texture();
		virtual ~Texture();


		virtual void Bind();
		virtual void UnBind();
		virtual GLuint getType() const;

		void setData(void *data, int width, int height);
		void GenerateMipmaps();
	};

	

}
