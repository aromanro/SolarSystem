#pragma once

#include <vector>
#include <memory>
#include "ObjLoader.h"

#include "Material.h"
#include "Texture.h"
#include "SpaceshipProgram.h"

namespace OpenGL {



	class ComplexObject : public VertexBufferRenderable
	{
	public:
		ComplexObject();

		int Load(const ObjLoader& loader, int startIndex = 0);

		virtual void Draw() override;
		void DrawInstanced(unsigned int count);

		void SetValues(SpaceshipProgram& program);

	protected:
		Material material;
		int vertexCount = 0;

		std::shared_ptr<Texture> ambientTexture;
		std::shared_ptr<Texture> diffuseTexture;
	};

	class ComplexObjectCompositeMaterials
	{
	public:
		ComplexObjectCompositeMaterials(const ObjLoader& loader);

		void Draw(SpaceshipProgram& program);
		void DrawInstanced(unsigned int count);

		std::vector<std::shared_ptr<ComplexObject>> complexObjects;
	};
}

