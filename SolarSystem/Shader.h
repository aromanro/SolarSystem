#pragma once

#include <vector>
#include "OpenGLObject.h"

namespace OpenGL {

	class Executable : public OpenGLObject {
	protected:
		std::vector<char> ErrorMsg;
	public:
		virtual bool getStatus() const = 0;
		virtual const char* getStatusMessage() = 0;
	};

	class Shader : public Executable
	{
	public:
		Shader(GLuint type = GL_VERTEX_SHADER);
		virtual ~Shader();

		bool getStatus() const override;
		const char* getStatusMessage() override;

		void Bind() override; // it's actually a compile, no need to be called, it's called automatically by setSource
		void UnBind() override; // does nothing for a shader

		void setSource(const char* src);
	};

	class TessellationControlShader : public Shader {
	public:
		TessellationControlShader();
		GLuint getType() const override;
	};

	class TessellationEvaluationShader : public Shader {
	public:
		TessellationEvaluationShader();
		GLuint getType() const override;
	};

	class GeometryShader : public Shader {
	public:
		GeometryShader();
		GLuint getType() const override;
	};

	class ComputeShader : public Shader {
	public:
		ComputeShader();
		GLuint getType() const override;
	};

	class VertexShader : public Shader {
	public:
		VertexShader();
		GLuint getType() const override;
	};


	class FragmentShader : public Shader {
	public:
		FragmentShader();
		GLuint getType() const override;
	};
}

