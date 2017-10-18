#include "stdafx.h"
#include "Shader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace OpenGL {

	Shader::Shader(GLuint type)
	{
		ID = glCreateShader(type);
	}

	Shader::~Shader()
	{
		glDeleteShader(ID);
	}

	void Shader::Bind() 
	{
		glCompileShader(ID);
	}

	void Shader::UnBind()
	{
		// nothing to be done
	}


	bool Shader::getStatus() const
	{
		GLint Result = GL_FALSE;

		glGetShaderiv(ID, GL_COMPILE_STATUS, &Result);

		return Result == GL_TRUE;
	}

	const char* Shader::getStatusMessage()
	{
		GLint InfoLength = 0;
		glGetShaderiv(ID, GL_INFO_LOG_LENGTH, &InfoLength);

		if (InfoLength > 0)
		{
			ErrorMsg.resize((unsigned int)InfoLength + 1);
			ErrorMsg[0] = 0;
			glGetShaderInfoLog(ID, (GLsizei)InfoLength, NULL, &ErrorMsg[0]);

			return &ErrorMsg[0];
		}

		return "";
	}

	void Shader::setSource(const char* src)
	{
		glShaderSource(ID, 1, &src, NULL);
		Bind();
	}

	VertexShader::VertexShader()
		: Shader(GL_VERTEX_SHADER)
	{
	}

	GLuint VertexShader::getType() const 
	{ 
		return GL_VERTEX_SHADER; 
	}

	FragmentShader::FragmentShader()
		: Shader(GL_FRAGMENT_SHADER)
	{
	}

	GLuint FragmentShader::getType() const 
	{ 
		return GL_FRAGMENT_SHADER; 
	}

	GeometryShader::GeometryShader()
		: Shader(GL_GEOMETRY_SHADER)
	{
	}

	GLuint GeometryShader::getType() const
	{
		return GL_GEOMETRY_SHADER;
	}

	ComputeShader::ComputeShader()
		: Shader(GL_COMPUTE_SHADER)
	{
	}

	GLuint ComputeShader::getType() const
	{
		return GL_COMPUTE_SHADER;
	}
}