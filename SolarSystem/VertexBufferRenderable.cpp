#include "stdafx.h"
#include "VertexBufferRenderable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace OpenGL {

	VertexBufferRenderable::VertexBufferRenderable()
	{
		Renderable::Bind();
	}


	VertexBufferRenderable::~VertexBufferRenderable()
	{
	}

	void VertexBufferRenderable::Bind()
	{
		Renderable::Bind();
		vbo.Bind();
	}

	void VertexBufferRenderable::UnBind()
	{
		vbo.UnBind();
		Renderable::UnBind();
	}

	void VertexBufferRenderable::setData(void *data, unsigned int len, GLenum type)
	{
		vbo.setData(data, len, type);
	}
}
