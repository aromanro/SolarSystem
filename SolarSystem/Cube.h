#pragma once


#include <GL/glew.h>
#include <gl/GL.h>      // OpenGL Libraries
#include <gl/GLU.h>     // GLU OpenGL Libraries

#include "VertexBufferRenderable.h"

namespace OpenGL {

	class Cube : public VertexBufferRenderable
	{
	public:
		Cube();
		virtual ~Cube();

		void Draw() override;
	};


}
