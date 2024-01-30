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
		~Sphere() override;

		void Draw() override;
		void DrawInstanced(unsigned int count);
	};

}
