#pragma once
#include "OpenGLObject.h"

namespace OpenGL {

	class Renderable :
		public VertexArrayObject
	{
	public:
		Renderable();
	    ~Renderable() override;

		virtual void Draw() = 0;
	};

}


