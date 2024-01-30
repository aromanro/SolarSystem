#include "stdafx.h"
#include "OpenGLObject.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace OpenGL {

	OpenGLObject::~OpenGLObject()
	{
	}

	VertexArrayObject::VertexArrayObject()
	{
		glGenVertexArrays(1, &ID);
	}

	VertexArrayObject::~VertexArrayObject()
	{
		glBindVertexArray(0);
		glDeleteVertexArrays(1, &ID);
	}

	void VertexArrayObject::Bind()
	{
		glBindVertexArray(ID);
	}

	void VertexArrayObject::UnBind()
	{
		glBindVertexArray(0);
	}

	GLuint VertexArrayObject::getType() const
	{
		return GL_VERTEX_ARRAY;
	}

	
	BufferObject::BufferObject()
	{
		glGenBuffers(1, &ID);
	}

	BufferObject::~BufferObject()
	{
		glBindBuffer(getType(), 0);
		glDeleteBuffers(1, &ID);
	}

	void BufferObject::Bind()
	{
		glBindBuffer(getType(), ID);
	}

	void BufferObject::UnBind()
	{
		glBindBuffer(getType(), 0);
	}

	GLuint BufferObject::getType() const
	{
		return GL_BUFFER;
	}

	void BufferObject::setData(const void *data, unsigned int len, GLenum type)
	{
		Bind();
		glBufferData(getType(), len, data, type);
	}

	void BufferObject::setSubData(const void *data, unsigned int len, unsigned int index)
	{
		glBufferSubData(getType(), index, len, data);
	}

	GLuint VertexBufferObject::getType() const
	{
		return GL_ARRAY_BUFFER;
	}

	GLuint ElementBufferObject::getType() const
	{
		return GL_ELEMENT_ARRAY_BUFFER;
	}

	GLuint UniformBufferObject::getType() const
	{
		return GL_UNIFORM_BUFFER;
	}


	FrameBufferObject::FrameBufferObject()
	{
		glGenFramebuffers(1, &ID);
	}

	FrameBufferObject::~FrameBufferObject()
	{
		glBindFramebuffer(getType(), 0);
		glDeleteFramebuffers(1, &ID);
	}

	void FrameBufferObject::Bind()
	{
		glBindFramebuffer(getType(), ID);
	}

	void FrameBufferObject::UnBind()
	{
		glBindFramebuffer(getType(), 0);
	}

	GLuint FrameBufferObject::getType() const
	{
		return GL_FRAMEBUFFER;
	}

}
