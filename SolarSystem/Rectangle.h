#pragma once


#include <GL/glew.h>
#include <gl/gl.h>      // OpenGL Libraries
#include <gl/glu.h>     // GLU OpenGL Libraries

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

