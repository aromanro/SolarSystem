#pragma once

#include <list>

#include "OpenGLObject.h"
#include "Shader.h"

namespace OpenGL {

	class Program : public Executable
	{
	protected:
		std::list<GLuint> shaders;

		void DetachShaders();
	public:
		Program();
		virtual ~Program();

		virtual void Bind(); // it's actually a link
		virtual GLuint getType() const;

		void Use();
		void UnUse();

		virtual void UnBind(); // same as UnUse

		void Attach(Shader& shader);

		virtual bool getStatus() const;
		virtual const char* getStatusMessage();
	};

}