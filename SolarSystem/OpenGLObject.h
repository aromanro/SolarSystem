#pragma once

#include <GL/glew.h>
#include <gl/GL.h>      // OpenGL Libraries
#include <gl/GLU.h>     // GLU OpenGL Libraries


namespace OpenGL {

	class OpenGLObject
	{
	public:
		OpenGLObject() = default;
		virtual ~OpenGLObject();

		virtual void Bind() = 0;
		virtual void UnBind() = 0;
		virtual GLuint getType() const = 0;

		GLuint getID() const { return ID; }
		
	private: 
		OpenGLObject(const OpenGLObject&) = delete;
		OpenGLObject& operator=(const OpenGLObject&) = delete;

	protected:
		GLuint ID = 0;
	};


	class VertexArrayObject : public OpenGLObject
	{
	public:
		VertexArrayObject();
	    ~VertexArrayObject() override;

		void Bind() override;
		void UnBind() override;
		GLuint getType() const override;
	};


	class BufferObject : public OpenGLObject {
	public:
		BufferObject();
		~BufferObject() override;

		void Bind() override;
		void UnBind() override;
		GLuint getType() const override;

		void setData(const void *data, unsigned int len, GLenum type = GL_STATIC_DRAW);
		void setSubData(const void *data, unsigned int len, unsigned int index = 0);
	};

	class VertexBufferObject : public BufferObject
	{
	public:
		GLuint getType() const override;
	};

	class ElementBufferObject : public BufferObject
	{
	public:
		GLuint getType() const override;
	};

	class UniformBufferObject : public BufferObject
	{
	public:
		GLuint getType() const override;
	};

	class FrameBufferObject : public OpenGLObject
	{
	public:
		FrameBufferObject();
		~FrameBufferObject() override;

		void Bind() override;
		void UnBind() override;
		GLuint getType() const override;
	};

}
