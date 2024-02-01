#include "stdafx.h"
#include "Shader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace OpenGL {

	Shader::Shader(GLuint type)
	{
		getID() = glCreateShader(type);
	}

	Shader::~Shader()
	{
		glDeleteShader(getID());
	}

	void Shader::Bind() 
	{
		glCompileShader(getID());
	}

	void Shader::UnBind()
	{
		// nothing to be done
	}


	bool Shader::getStatus() const
	{
		GLint Result = GL_FALSE;

		glGetShaderiv(getID(), GL_COMPILE_STATUS, &Result);

		return Result == GL_TRUE;
	}

	const char* Shader::getStatusMessage()
	{
		GLint InfoLength = 0;
		glGetShaderiv(getID(), GL_INFO_LOG_LENGTH, &InfoLength);

		if (InfoLength > 0)
		{
			getErrorMsg().resize(static_cast<unsigned int>(InfoLength + 1ULL));
			getErrorMsg()[0] = 0;
			glGetShaderInfoLog(getID(), static_cast<GLsizei>(InfoLength), nullptr, &getErrorMsg()[0]);

			return &getErrorMsg()[0];
		}

		return "";
	}

	void Shader::setSource(const char* src)
	{
		glShaderSource(getID(), 1, &src, nullptr);
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

	TessellationControlShader::TessellationControlShader()
		: Shader(GL_TESS_CONTROL_SHADER)
	{
	}

	GLuint TessellationControlShader::getType() const
	{
		return GL_TESS_CONTROL_SHADER;
	}

	TessellationEvaluationShader::TessellationEvaluationShader()
		: Shader(GL_TESS_EVALUATION_SHADER)
	{
	}

	GLuint TessellationEvaluationShader::getType() const
	{
		return GL_TESS_EVALUATION_SHADER;
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

	FragmentShader::FragmentShader()
		: Shader(GL_FRAGMENT_SHADER)
	{
	}

	GLuint FragmentShader::getType() const 
	{ 
		return GL_FRAGMENT_SHADER; 
	}

}