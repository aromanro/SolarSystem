#pragma once

#include "VertexBufferRenderable.h"

namespace OpenGL {

	class Sphere : public VertexBufferRenderable
	{
	protected:
		unsigned int m_lats;
		unsigned int m_longs;
	public:
		Sphere(float r = 1., unsigned int lats = 128, unsigned int longs = 128);
		virtual ~Sphere();

		virtual void Draw();
		void DrawInstanced(unsigned int count);
	};

}
