#pragma once

#include <vector>
#include <memory>
#include "ObjLoader.h"


namespace OpenGL {

	// TODO: implement it!
	class ComplexObject : public VertexBufferRenderable
	{
	public:
		ComplexObject(const ObjLoader& loader); //pass it with the loaded file!

		virtual void Draw() override;
		void DrawInstanced(unsigned int count);

	protected:
		int vertexCount = 0;
	};

}

