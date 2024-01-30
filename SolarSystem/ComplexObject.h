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

		void Draw() override;
		void DrawInstanced(unsigned int count);

		void SetValues(SpaceshipProgram& program);

	protected:
		void LoadAndGenerateMipmaps(const ObjLoader& loader, int startIndex);
		int SetVerticesData(const ObjLoader& loader, int startIndex, int triangleCount);

		ObjMaterial material;
		int vertexCount = 0;

		std::shared_ptr<Texture> ambientTexture;
		std::shared_ptr<Texture> diffuseTexture;
		std::shared_ptr<Texture> specularTexture;
		
		std::shared_ptr<Texture> exponentTexture;
		std::shared_ptr<Texture> bumpTexture;
	};

	class ComplexObjectCompositeMaterials
	{
	public:
		explicit ComplexObjectCompositeMaterials(const ObjLoader& loader);

		void Draw(SpaceshipProgram& program);
		void DrawInstanced(unsigned int count);

		std::vector<std::shared_ptr<ComplexObject>> complexObjects;
	};
}

