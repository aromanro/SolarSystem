#pragma once

#include <vector>
#include <memory>
#include "ObjLoader.h"


namespace OpenGL {



	class ComplexObject : public VertexBufferRenderable
	{
	public:
		ComplexObject();

		int Load(const ObjLoader& loader, int startIndex = 0);

		virtual void Draw() override;
		void DrawInstanced(unsigned int count);

	protected:
		int vertexCount = 0;
	};

	class ComplexObjectCompositeMaterials
	{
	public:
		ComplexObjectCompositeMaterials(const ObjLoader& loader);

		void Draw();
		void DrawInstanced(unsigned int count);

		std::vector<std::shared_ptr<ComplexObject>> complexObjects;
	};
}

