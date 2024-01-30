#pragma once


#include <GL/glew.h>
#include <gl/gl.h>      // OpenGL Libraries
#include <gl/glu.h>     // GLU OpenGL Libraries

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
