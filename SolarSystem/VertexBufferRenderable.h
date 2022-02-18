#pragma once
#include "Renderable.h"

namespace OpenGL {

	class VertexBufferRenderable :
		public Renderable
	{
	protected:
		VertexBufferObject vbo;

	public:
		VertexBufferRenderable();
		virtual ~VertexBufferRenderable();

		virtual void Draw() = 0;

		void Bind() override;
		void UnBind() override;

		void setData(void *data, unsigned int len, GLenum type = GL_STATIC_DRAW);
	};


}
