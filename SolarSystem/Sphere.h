#pragma once

#include "VertexBufferRenderable.h"

namespace OpenGL {

	class Sphere : public VertexBufferRenderable
	{
	protected:
		unsigned int m_lats;
		unsigned int m_longs;
	public:
		Sphere(float r = 1., unsigned int lats = 64, unsigned int longs = 64);
		virtual ~Sphere();

		virtual void Draw();
	};

}
