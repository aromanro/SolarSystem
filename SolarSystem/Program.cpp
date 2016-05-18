#include "stdafx.h"
#include "Program.h"

#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace OpenGL {

	Program::Program()
	{
		ID = glCreateProgram();
	}


	Program::~Program()
	{
		UnUse();
		glDeleteProgram(ID);
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
		glLinkProgram(ID);
		DetachShaders();
	}
	
	GLuint Program::getType()
	{
		return GL_PROGRAM;
	}

	void Program::Attach(Shader& shader)
	{
		glAttachShader(ID, shader.getID());
		shaders.push_back(shader.getID());
	}


	bool Program::getStatus()
	{
		GLint Result = GL_FALSE;

		glGetProgramiv(ID, GL_LINK_STATUS, &Result);

		return Result == GL_TRUE ? true : false;
	}

	const char* Program::getStatusMessage()
	{
		int InfoLength = 0;
		glGetProgramiv(ID, GL_INFO_LOG_LENGTH, &InfoLength);

		if (InfoLength > 0)
		{
			ErrorMsg.resize((unsigned int)InfoLength + 1);
			ErrorMsg[0] = 0;
			glGetProgramInfoLog(ID, InfoLength, NULL, &ErrorMsg[0]);

			return &ErrorMsg[0];
		}

		return "";
	}

	void Program::DetachShaders()
	{
		GLuint id = ID;

		std::for_each(shaders.begin(), shaders.end(), [id](GLuint shader) { 
			glDetachShader(id, shader); 
		});

		shaders.clear();
	}

	void Program::Use()
	{
		glUseProgram(ID);
	}
}
