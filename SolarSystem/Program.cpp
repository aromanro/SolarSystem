#include "stdafx.h"
#include "Program.h"

#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace OpenGL {

	Program::Program()
	{
		getID() = glCreateProgram();
	}


	Program::~Program()
	{
		UnUse();
		glDeleteProgram(getID());
	}

	void Program::UnUse()
	{
		glUseProgram(0);
	}

	void Program::UnBind()
	{
		UnUse();
	}

	void Program::Bind()
	{
		glLinkProgram(getID());
		DetachShaders();
	}
	
	GLuint Program::getType() const
	{
		return GL_PROGRAM;
	}

	void Program::Attach(const Shader& shader)
	{
		glAttachShader(getID(), shader.getID());
		shaders.emplace_back(shader.getID());
	}


	bool Program::getStatus() const
	{
		GLint Result = GL_FALSE;

		glGetProgramiv(getID(), GL_LINK_STATUS, &Result);

		return Result == GL_TRUE;
	}

	const char* Program::getStatusMessage()
	{
		int InfoLength = 0;
		glGetProgramiv(getID(), GL_INFO_LOG_LENGTH, &InfoLength);

		if (InfoLength > 0)
		{
			getErrorMsg().resize(static_cast<unsigned int>(InfoLength + 1ULL));
			getErrorMsg()[0] = 0;
			glGetProgramInfoLog(getID(), InfoLength, nullptr, &getErrorMsg()[0]);

			return &getErrorMsg()[0];
		}

		return "";
	}

	void Program::DetachShaders()
	{
		GLuint id = getID();

		std::for_each(shaders.begin(), shaders.end(), [id](GLuint shader) { 
			glDetachShader(id, shader); 
		});

		shaders.clear();
	}

	void Program::Use()
	{
		glUseProgram(getID());
	}
}
