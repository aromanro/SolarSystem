#pragma once


#include <GL\glew.h>
#include <gl\gl.h>      // OpenGL Libraries
#include <gl\glu.h>     // GLU OpenGL Libraries

#include "VertexBufferRenderable.h"

namespace OpenGL {

	class Rectangle : public VertexBufferRenderable
	{
	public:
		Rectangle(float aspectRatio = 1.0f);
		virtual ~Rectangle();

		virtual void Draw();
	};

}

