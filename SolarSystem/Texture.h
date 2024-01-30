#pragma once
#include "OpenGLObject.h"

namespace OpenGL {

	class Texture :
		public OpenGLObject
	{
	public:
		Texture();
		~Texture() override;


		void Bind() override;
		void Bind(int nr);

		void UnBind() override;
		GLuint getType() const override;

		virtual void setData(const void *data, int width, int height, int nr = 0, int nrBytes = 3);
		void GenerateMipmaps();
	};

	class PixelBuffer;

	class TextureWithPixelBuffer : public Texture
	{
	protected:
		PixelBuffer* pixelBuffer;

		int m_width;
		int m_height;
		int m_nrBytes;
	public:
		TextureWithPixelBuffer();
		~TextureWithPixelBuffer() override;

		void setData(const void* data, int width, int height, int nr = 0, int nrBytes = 3) override;
		void Draw();
	};
}
