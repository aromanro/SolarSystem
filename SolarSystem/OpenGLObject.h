#pragma once

#include <GL\glew.h>
#include <gl\gl.h>      // OpenGL Libraries
#include <gl\glu.h>     // GLU OpenGL Libraries


namespace OpenGL {

	class OpenGLObject
	{
	protected:
		GLuint ID;
	public:
		OpenGLObject();
		virtual ~OpenGLObject();

		virtual void Bind() = 0;
		virtual void UnBind() = 0;
		virtual GLuint getType() = 0;

		GLuint getID() const { return ID; }
		operator GLuint() const { return getID(); }
	};


	class VertexArrayObject : public OpenGLObject
	{
	public:
		VertexArrayObject();
		virtual ~VertexArrayObject();

		virtual void Bind();
		virtual void UnBind();
		virtual GLuint getType();
	};


	class BufferObject : public OpenGLObject {
	public:
		BufferObject();
		virtual ~BufferObject();

		virtual void Bind();
		virtual void UnBind();
		virtual GLuint getType();

		void setData(void *data, unsigned int len, GLenum type = GL_STATIC_DRAW);
		void setSubData(void *data, unsigned int len, unsigned int index = 0);
	};

	class VertexBufferObject : public BufferObject
	{
	public:
		virtual GLuint getType();
	};

	class ElementBufferObject : public BufferObject
	{
	public:
		virtual GLuint getType();
	};

	class UniformBufferObject : public BufferObject
	{
	public:
		virtual GLuint getType();
	};

	class FrameBufferObject : public OpenGLObject
	{
	public:
		FrameBufferObject();
		virtual ~FrameBufferObject();

		virtual void Bind();
		virtual void UnBind();
		virtual GLuint getType();
	};

}
