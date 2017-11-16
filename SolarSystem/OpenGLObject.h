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
		virtual GLuint getType() const = 0;

		GLuint getID() const { return ID; }
		operator GLuint() const { return getID(); }

	private: 
		OpenGLObject(const OpenGLObject&);
		OpenGLObject& operator=(const OpenGLObject&);
	};


	class VertexArrayObject : public OpenGLObject
	{
	public:
		VertexArrayObject();
		virtual ~VertexArrayObject();

		virtual void Bind();
		virtual void UnBind();
		virtual GLuint getType() const;
	};


	class BufferObject : public OpenGLObject {
	public:
		BufferObject();
		virtual ~BufferObject();

		virtual void Bind();
		virtual void UnBind();
		virtual GLuint getType() const;

		void setData(const void *data, unsigned int len, GLenum type = GL_STATIC_DRAW);
		void setSubData(const void *data, unsigned int len, unsigned int index = 0);
	};

	class VertexBufferObject : public BufferObject
	{
	public:
		virtual GLuint getType() const;
	};

	class ElementBufferObject : public BufferObject
	{
	public:
		virtual GLuint getType() const;
	};

	class UniformBufferObject : public BufferObject
	{
	public:
		virtual GLuint getType() const;
	};

	class FrameBufferObject : public OpenGLObject
	{
	public:
		FrameBufferObject();
		virtual ~FrameBufferObject();

		virtual void Bind();
		virtual void UnBind();
		virtual GLuint getType() const;
	};

}
