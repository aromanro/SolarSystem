#include "stdafx.h"
#include "MatrixPush.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace OpenGL {

	MatrixPush::MatrixPush(glm::mat4& matrix)
		: topMat(matrix), saveMat(matrix)
	{
	}

	MatrixPush::MatrixPush(MatrixPush& matrix)
		: topMat(matrix.topMat), saveMat(matrix.saveMat)
	{
	}

	MatrixPush::~MatrixPush()
	{
		topMat = saveMat;
	}

	MatrixPush& MatrixPush::operator=(MatrixPush& matrix)
	{
		saveMat = matrix.saveMat;
		topMat = matrix.topMat;

		return *this;
	}

	MatrixPush& MatrixPush::operator=(glm::mat4& matrix)
	{
		saveMat = matrix;
		topMat = matrix;

		return *this;
	}

}
