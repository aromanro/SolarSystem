#pragma once

#include "OpenGLObject.h"

namespace OpenGL {

	class PixelBuffer : public OpenGLObject
	{
	public:
		explicit PixelBuffer(unsigned int imageBufferSize);
		~PixelBuffer() override;

		void Bind() override;
		void UnBind() override;
		GLuint getType() const override;

		void Upload(const void* buf, unsigned int curBufSize);
		void Draw(GLuint texID, unsigned int width, unsigned int height, bool bgr = true, bool color = true);
	};

}


