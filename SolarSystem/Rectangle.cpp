#include "stdafx.h"
#include "Rectangle.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace OpenGL {

	Rectangle::Rectangle(float aspectRatio)
	{
		const int STRIDE_SIZE = 8; // 3 for vertex, 3 for normal, 2 for texture coordinates

		GLfloat vertices[] = {
			-0.5f * aspectRatio,  0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, // top-left
			-0.5f * aspectRatio, -0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			 0.5f * aspectRatio,  0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
			 0.5f * aspectRatio, -0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
		};

		Renderable::Bind();

		setData(vertices, sizeof(vertices));
		// Link vertex attributes
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, STRIDE_SIZE * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, STRIDE_SIZE * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, STRIDE_SIZE * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}


	Rectangle::~Rectangle()
	{
	}

	void Rectangle::Draw()
	{
		// Render rectangle
		VertexBufferRenderable::Bind();

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		//VertexBufferRenderable::UnBind();
	}

}



