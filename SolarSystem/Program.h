#pragma once

#include <list>

#include "OpenGLObject.h"
#include "Shader.h"

namespace OpenGL {

	class Program : public Executable
	{
	protected:
		std::list<GLuint> shaders;

	public:
		Program();
		virtual ~Program();

		void DetachShaders();

		virtual void Bind() override; // it's actually a link
		virtual GLuint getType() const override;

		void Use();
		void UnUse();

		virtual void UnBind() override; // same as UnUse

		void Attach(const Shader& shader);

		virtual bool getStatus() const override;
		virtual const char* getStatusMessage() override;
	};

}