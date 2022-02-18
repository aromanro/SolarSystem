#include "stdafx.h"
#include "ComplexObject.h"
#include "BodyProperties.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


namespace OpenGL {

	ComplexObject::ComplexObject()
		: VertexBufferRenderable()
	{
	}


	int ComplexObject::Load(const ObjLoader& loader, int startIndex)
	{
		if (loader.triangles.size() <= startIndex) return startIndex;

		std::string curMaterial = loader.triangles[startIndex]->material.name;

		int triangleCount = 1;
		for (int i = startIndex + 1; i < loader.triangles.size(); ++i)
		{
			if (curMaterial == loader.triangles[i]->material.name)
				++triangleCount;
			else break;
		}

		int endIndex = SetVerticesData(loader, startIndex, triangleCount);

		LoadAndGenerateMipmaps(loader, startIndex);

		return endIndex;
	}

	int ComplexObject::SetVerticesData(const ObjLoader& loader, int startIndex, int triangleCount)
	{
		const long long int STRIDE_SIZE = 8; // 3 for vertex, 3 for normal, maybe later 3 for tangent - the one with texture will add 2 for texture coordinate


		vertexCount = static_cast<int>(3 * triangleCount); // 3 vertexes / triangle


		GLfloat* vertices = new GLfloat[STRIDE_SIZE * vertexCount];

		int baseIndex = 0;

		const int endIndex = startIndex + triangleCount;
		for (int i = startIndex; i < endIndex; ++i)
		{
			const auto& triangle = loader.triangles[i];
			vertices[baseIndex] = static_cast<GLfloat>(triangle->A.X);
			vertices[baseIndex + 1] = static_cast<GLfloat>(triangle->A.Y);
			vertices[baseIndex + 2] = static_cast<GLfloat>(triangle->A.Z);
			vertices[baseIndex + 3] = static_cast<GLfloat>(triangle->normal1.X);
			vertices[baseIndex + 4] = static_cast<GLfloat>(triangle->normal1.Y);
			vertices[baseIndex + 5] = static_cast<GLfloat>(triangle->normal1.Z);
			vertices[baseIndex + 6] = static_cast<GLfloat>(triangle->U1);
			vertices[baseIndex + 7] = static_cast<GLfloat>(triangle->V1);

			vertices[baseIndex + 8] = static_cast<GLfloat>(triangle->B.X);
			vertices[baseIndex + 9] = static_cast<GLfloat>(triangle->B.Y);
			vertices[baseIndex + 10] = static_cast<GLfloat>(triangle->B.Z);
			vertices[baseIndex + 11] = static_cast<GLfloat>(triangle->normal2.X);
			vertices[baseIndex + 12] = static_cast<GLfloat>(triangle->normal2.Y);
			vertices[baseIndex + 13] = static_cast<GLfloat>(triangle->normal2.Z);
			vertices[baseIndex + 14] = static_cast<GLfloat>(triangle->U2);
			vertices[baseIndex + 15] = static_cast<GLfloat>(triangle->V2);


			vertices[baseIndex + 16] = static_cast<GLfloat>(triangle->C.X);
			vertices[baseIndex + 17] = static_cast<GLfloat>(triangle->C.Y);
			vertices[baseIndex + 18] = static_cast<GLfloat>(triangle->C.Z);
			vertices[baseIndex + 19] = static_cast<GLfloat>(triangle->normal3.X);
			vertices[baseIndex + 20] = static_cast<GLfloat>(triangle->normal3.Y);
			vertices[baseIndex + 21] = static_cast<GLfloat>(triangle->normal3.Z);
			vertices[baseIndex + 22] = static_cast<GLfloat>(triangle->U3);
			vertices[baseIndex + 23] = static_cast<GLfloat>(triangle->V3);

			baseIndex += 3 * STRIDE_SIZE;
		}

		Renderable::Bind();

		setData(vertices, vertexCount * sizeof(GLfloat) * STRIDE_SIZE);
		// Link vertex attributes
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, STRIDE_SIZE * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, STRIDE_SIZE * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		delete[] vertices;

		return endIndex;
	}


	void ComplexObject::LoadAndGenerateMipmaps(const ObjLoader& loader, int startIndex)
	{
		material = loader.triangles[startIndex]->material;
		if (!material.ambientTexture.empty())
		{
			const std::string path = loader.dir + material.ambientTexture;
			CString fileName(path.c_str());
			ambientTexture = BodyProperties::LoadTexture(fileName);
			if (ambientTexture)
				ambientTexture->GenerateMipmaps();
		}

		if (!material.diffuseTexture.empty())
		{
			const std::string path = loader.dir + material.diffuseTexture;
			CString fileName(path.c_str());
			diffuseTexture = BodyProperties::LoadTexture(fileName, 1);
			if (diffuseTexture)
				diffuseTexture->GenerateMipmaps();
		}

		if (!material.specularTexture.empty())
		{
			const std::string path = loader.dir + material.specularTexture;
			CString fileName(path.c_str());
			specularTexture = BodyProperties::LoadTexture(fileName, 2);
			if (specularTexture)
				specularTexture->GenerateMipmaps();
		}

		if (!material.exponentTexture.empty())
		{
			const std::string path = loader.dir + material.exponentTexture;
			CString fileName(path.c_str());
			// I have to find an obj file with an exponent texture to check what is their format: 8bpp or 3 bytes
			exponentTexture = BodyProperties::LoadTexture(fileName, 3/*, 8*/);
			if (exponentTexture)
				exponentTexture->GenerateMipmaps();
		}

		if (!material.bumpTexture.empty())
		{
			const std::string path = loader.dir + material.bumpTexture;
			CString fileName(path.c_str());
			bumpTexture = BodyProperties::LoadNormalTexture(fileName, 2, 4);
			if (bumpTexture)
				bumpTexture->GenerateMipmaps();
		}
	}

	void ComplexObject::SetValues(SpaceshipProgram& program)
	{
		// TODO: Implement it completely

		if (ambientTexture)
		{
			ambientTexture->Bind();
			glUniform1i(program.useAmbientTextureLocation, 1);
		}
		else
		{
			glUniform1i(program.useAmbientTextureLocation, 0);
		}
		glUniform3f(program.ambientColorLocation, static_cast<float>(material.ambientColor.r), static_cast<float>(material.ambientColor.g), static_cast<float>(material.ambientColor.b));

		if (diffuseTexture)
		{
			diffuseTexture->Bind(1);
			glUniform1i(program.useDiffuseTextureLocation, 1);
		}
		else
		{
			glUniform1i(program.useDiffuseTextureLocation, 0);
		}
		glUniform3f(program.diffuseColorLocation, static_cast<float>(material.diffuseColor.r), static_cast<float>(material.diffuseColor.g), static_cast<float>(material.diffuseColor.b));


		if (specularTexture)
		{
			specularTexture->Bind(2);
			glUniform1i(program.useSpecularTextureLocation, 1);
		}
		else
		{
			glUniform1i(program.useSpecularTextureLocation, 0);
		}
		glUniform3f(program.specularColorLocation, static_cast<float>(material.specularColor.r), static_cast<float>(material.specularColor.g), static_cast<float>(material.specularColor.b));


		if (exponentTexture)
		{
			exponentTexture->Bind(3);
			glUniform1i(program.useExponentTextureLocation, 1);
		}
		else
		{
			glUniform1i(program.useExponentTextureLocation, 0);
		}
		glUniform1f(program.exponentLocation, static_cast<float>(material.exponent));
		
		// there is no bump mapping for now
		/*
		if (bumpTexture)
		{
			bumpTexture->Bind(4);
			glUniform1i(program.useBumpTextureLocation, 1);
		}
		else
		{
			glUniform1i(program.useBumpTextureLocation, 0);
		}
		*/

		// now things from 'material':
		glUniform1i(program.illuminationLocation, static_cast<GLint>(material.illumination));
		// no 'emission' for now
		//glUniform3f(program.emissionColorLocation, material.emissionColor.r, material.emissionColor.g, material.emissionColor.b);
	}


	void ComplexObject::Draw()
	{		
		VertexBufferRenderable::Bind();

		glDrawArrays(GL_TRIANGLES, 0, vertexCount);

		//VertexBufferRenderable::UnBind();
	}

	void ComplexObject::DrawInstanced(unsigned int count)
	{
		VertexBufferRenderable::Bind();

		glDrawArraysInstanced(GL_TRIANGLES, 0, vertexCount, count);
	}



	ComplexObjectCompositeMaterials::ComplexObjectCompositeMaterials(const ObjLoader& loader)
	{
		if (loader.triangles.size() == 0) return;

		std::string curMaterial;

		// count the number of different materials objects encountered
		int materialsCount = 0;
		for (int i = 0; i < loader.triangles.size(); ++i)
		{
			if (curMaterial != loader.triangles[i]->material.name)
			{
				++materialsCount;
				curMaterial = loader.triangles[i]->material.name;
			}
		}

		complexObjects.resize(materialsCount);

		int startIndex = 0;
		int i = 0;
		while (startIndex < loader.triangles.size())
		{
			complexObjects[i] = std::make_shared<ComplexObject>();
			startIndex = complexObjects[i]->Load(loader, startIndex);
			++i;
		}

		BodyProperties::ClearTexturesCache();
	}

	void ComplexObjectCompositeMaterials::Draw(SpaceshipProgram& program)
	{
		for (auto& obj : complexObjects)
		{
			obj->SetValues(program);
			obj->Draw();
		}
	}

	void ComplexObjectCompositeMaterials::DrawInstanced(unsigned int count)
	{
		for (auto& obj : complexObjects)
			obj->DrawInstanced(count);
	}

}

