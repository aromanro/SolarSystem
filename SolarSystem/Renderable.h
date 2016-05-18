#pragma once
#include "OpenGLObject.h"

namespace OpenGL {

	class Renderable :
		public VertexArrayObject
	{
	public:
		Renderable();
		virtual ~Renderable();

		virtual void Draw() = 0;
	};

}


