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

		void Bind() override; // it's actually a link
		GLuint getType() const override;

		void Use();
		void UnUse();

		void UnBind() override; // same as UnUse

		void Attach(const Shader& shader);

		bool getStatus() const override;
		const char* getStatusMessage() override;
	};

}