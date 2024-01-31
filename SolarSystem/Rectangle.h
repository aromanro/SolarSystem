#pragma once


#include <GL/glew.h>
#include <gl/GL.h>      // OpenGL Libraries
#include <gl/GLU.h>     // GLU OpenGL Libraries

#include "VertexBufferRenderable.h"

namespace OpenGL {

	class Rectangle : public VertexBufferRenderable
	{
	public:
		explicit Rectangle(float aspectRatio = 1.0f);
		~Rectangle() override;

		void Draw() override;
	};

}

