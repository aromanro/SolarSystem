#include "stdafx.h"
#include "Texture.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace OpenGL {

	Texture::Texture()
	{
		glGenTextures(1, &ID);
	}

	Texture::~Texture()
	{
		UnBind();
		glDeleteTextures(1, &ID);
	}

	void Texture::Bind()
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(getType(), ID);	
	}

	void Texture::Bind(int nr)
	{
		glActiveTexture(GL_TEXTURE0 + nr);
		glBindTexture(getType(), ID);
	}

	void Texture::UnBind()
	{
		glBindTexture(getType(), 0);
	}

	GLuint Texture::getType() const
	{
		return GL_TEXTURE_2D;
	}

	void Texture::setData(const void *data, int width, int height, int nr, int nrBytes)
	{
		Bind(nr);

		glTexParameteri(getType(), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(getType(), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexParameteri(getType(), GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(getType(), GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexImage2D(getType(), 0, nrBytes, width, height, 0, 1 == nrBytes ? GL_LUMINANCE : (3 == nrBytes ? GL_BGR_EXT : GL_BGRA_EXT), GL_UNSIGNED_BYTE, data);
	}

	void Texture::GenerateMipmaps()
	{
		glTexParameteri(getType(), GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(getType());
	}



}
