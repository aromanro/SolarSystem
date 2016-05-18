#pragma once

#include <glm.hpp>

namespace OpenGL {

	// Resource acquisition is initialization pattern

	class MatrixPush
	{
	protected:
		glm::mat4 saveMat;
		glm::mat4 &topMat;
	public:
		MatrixPush(glm::mat4& matrix);
		MatrixPush(MatrixPush& matrix);

		~MatrixPush();

		MatrixPush& operator=(glm::mat4& matrix);
		MatrixPush& operator=(MatrixPush& matrix);

		glm::mat4& Top() { return topMat; };
	};

}
