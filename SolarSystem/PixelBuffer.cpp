#include "stdafx.h"
#include "PixelBuffer.h"

namespace OpenGL
{

	PixelBuffer::PixelBuffer(unsigned int imageBufferSize)
	{
		glGenBuffersARB(1, &getID());

		Bind();
		glBufferDataARB(GL_PIXEL_UNPACK_BUFFER, imageBufferSize, 0, GL_STREAM_DRAW);
		UnBind();
	}

	PixelBuffer::~PixelBuffer()
	{
		glDeleteBuffersARB(1, &getID());
	}

	void PixelBuffer::Bind()
	{
		glBindBufferARB(GL_PIXEL_UNPACK_BUFFER, getID());
	}

	void PixelBuffer::UnBind()
	{
		glBindBufferARB(GL_PIXEL_UNPACK_BUFFER, 0);
	}

	GLuint PixelBuffer::getType() const
	{
		return GL_PIXEL_UNPACK_BUFFER_EXT; // whatever?
	}


	void PixelBuffer::Upload(const void* buf, unsigned int curBufSize)
	{
		if (nullptr == buf || 0 == curBufSize)	return;

		Bind();
		glBufferDataARB(GL_PIXEL_UNPACK_BUFFER, curBufSize, 0, GL_STREAM_DRAW);

		GLubyte* ptr = (GLubyte*)glMapBufferARB(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
		if (ptr)
		{
			memcpy(ptr, buf, curBufSize);
			glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER);
		}

		UnBind();
	}

	void PixelBuffer::Draw(GLuint texID, unsigned int width, unsigned int height, bool bgr, bool color)
	{
		glEnable(GL_TEXTURE_2D);
		glActiveTexture(texID);

		Bind();

		if (color)
			glTexImage2D(GL_TEXTURE_2D,
				0,
				GL_RGB,
				width,
				height,
				0,
				bgr ? GL_BGR_EXT : GL_RGB,
				GL_UNSIGNED_BYTE,
				nullptr);
		else
			glTexImage2D(GL_TEXTURE_2D,
				0,
				GL_RGB,
				width,
				height,
				0,
				GL_LUMINANCE,
				GL_UNSIGNED_BYTE,
				nullptr);


		UnBind();
	}

}