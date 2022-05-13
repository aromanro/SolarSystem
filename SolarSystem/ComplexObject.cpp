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
		const long long int STRIDE_SIZE = 11; // 3 for vertex, 3 for normal, 2 for texture coordintates, 3 for tangent 


		vertexCount = static_cast<int>(3 * triangleCount); // 3 vertexes / triangle


		GLfloat* vertices = new GLfloat[STRIDE_SIZE * vertexCount];

		int baseIndex = 0;

		const int endIndex = startIndex + triangleCount;
		for (int i = startIndex; i < endIndex; ++i)
		{
			const auto& triangle = loader.triangles[i];

			const Vector3D<double> edge1 = triangle->B - triangle->A;
			const Vector3D<double> edge2 = triangle->C - triangle->A;

			const double dU1 = triangle->U2 - triangle->U1;
			const double dV1 = triangle->V2 - triangle->V1;
			const double dU2 = triangle->U3 - triangle->U1;
			const double dV2 = triangle->V3 - triangle->V1;

			const double f = 1. / (dU1 * dV2 - dU2 * dV1);

			Vector3D<double> tangent(f * (dV2 * edge1.X - dV1 * edge2.X), f * (dV2 * edge1.Y - dV1 * edge2.Y), f * (dV2 * edge1.Z - dV1 * edge2.Z));
			// this is tangent to the triangle, but not necessarily orthogonal to the vertex normal (because it might not be orthogonal to the triangle plane), so orthogonalize it:
			tangent = tangent.Normalize();
			// just subtract out the component along the normal, for each vertex, then normalize it again

			vertices[baseIndex] = static_cast<GLfloat>(triangle->A.X);
			vertices[baseIndex + 1] = static_cast<GLfloat>(triangle->A.Y);
			vertices[baseIndex + 2] = static_cast<GLfloat>(triangle->A.Z);			
			vertices[baseIndex + 3] = static_cast<GLfloat>(triangle->normal1.X);
			vertices[baseIndex + 4] = static_cast<GLfloat>(triangle->normal1.Y);
			vertices[baseIndex + 5] = static_cast<GLfloat>(triangle->normal1.Z);			
			vertices[baseIndex + 6] = static_cast<GLfloat>(triangle->U1);
			vertices[baseIndex + 7] = static_cast<GLfloat>(triangle->V1);

			Vector3D<double> vtangent = tangent - (tangent * triangle->normal1) * triangle->normal1;
			vtangent = vtangent.Normalize();
			vertices[baseIndex + 8] = static_cast<GLfloat>(vtangent.X);
			vertices[baseIndex + 9] = static_cast<GLfloat>(vtangent.Y);
			vertices[baseIndex + 10] = static_cast<GLfloat>(vtangent.Z);

			vertices[baseIndex + 11] = static_cast<GLfloat>(triangle->B.X);
			vertices[baseIndex + 12] = static_cast<GLfloat>(triangle->B.Y);
			vertices[baseIndex + 13] = static_cast<GLfloat>(triangle->B.Z);
			vertices[baseIndex + 14] = static_cast<GLfloat>(triangle->normal2.X);
			vertices[baseIndex + 15] = static_cast<GLfloat>(triangle->normal2.Y);
			vertices[baseIndex + 16] = static_cast<GLfloat>(triangle->normal2.Z);
			vertices[baseIndex + 17] = static_cast<GLfloat>(triangle->U2);
			vertices[baseIndex + 18] = static_cast<GLfloat>(triangle->V2);

			vtangent = tangent - (tangent * triangle->normal2) * triangle->normal2;
			vtangent = vtangent.Normalize();
			vertices[baseIndex + 19] = static_cast<GLfloat>(vtangent.X);
			vertices[baseIndex + 20] = static_cast<GLfloat>(vtangent.Y);
			vertices[baseIndex + 21] = static_cast<GLfloat>(vtangent.Z);

			vertices[baseIndex + 22] = static_cast<GLfloat>(triangle->C.X);
			vertices[baseIndex + 23] = static_cast<GLfloat>(triangle->C.Y);
			vertices[baseIndex + 24] = static_cast<GLfloat>(triangle->C.Z);
			vertices[baseIndex + 25] = static_cast<GLfloat>(triangle->normal3.X);
			vertices[baseIndex + 26] = static_cast<GLfloat>(triangle->normal3.Y);
			vertices[baseIndex + 27] = static_cast<GLfloat>(triangle->normal3.Z);
			vertices[baseIndex + 28] = static_cast<GLfloat>(triangle->U3);
			vertices[baseIndex + 29] = static_cast<GLfloat>(triangle->V3);

			vtangent = tangent - (tangent * triangle->normal3) * triangle->normal3;
			vtangent = vtangent.Normalize();
			vertices[baseIndex + 30] = static_cast<GLfloat>(vtangent.X);
			vertices[baseIndex + 31] = static_cast<GLfloat>(vtangent.Y);
			vertices[baseIndex + 32] = static_cast<GLfloat>(vtangent.Z);

			baseIndex += 3 * STRIDE_SIZE;
		}

		Renderable::Bind();

		setData(vertices, vertexCount * sizeof(GLfloat) * STRIDE_SIZE);
		// Link vertex attributes

		// vertices
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, STRIDE_SIZE * sizeof(GLfloat), (GLvoid*)0);
		// normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, STRIDE_SIZE * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		
		// texture coordinates
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, STRIDE_SIZE * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));

		// tangent
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, STRIDE_SIZE * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));

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
			/*
			else {
				CString str = L"Ambient texture problem: ";
				str += path.c_str();
				AfxMessageBox(str);
			}
			*/

			if (material.ambientColor.r == material.ambientColor.g && material.ambientColor.r == material.ambientColor.b)
			{
				// some models look too dark because ambient color is multiplied with the ambient texture value and ambientColor is set way too low
				// I suspect they expect you to use either the color or the texture, but not both
				material.ambientColor.r = material.ambientColor.g = material.ambientColor.b = 1;
			}
		}

		if (!material.diffuseTexture.empty())
		{
			const std::string path = loader.dir + material.diffuseTexture;
			CString fileName(path.c_str());
			diffuseTexture = BodyProperties::LoadTexture(fileName, 1);
			if (diffuseTexture)
				diffuseTexture->GenerateMipmaps();
			/*
			else {
				CString str = L"Diffuse texture problem: ";
				str += path.c_str();
				AfxMessageBox(str);
			}
			*/

			if (material.diffuseColor.r == material.diffuseColor.g && material.diffuseColor.r == material.diffuseColor.b)
			{
				// some models look too dark because diffuse color is multiplied with the diffuse texture value and diffuseColor is set way too low
				// I suspect they expect you to use either the color or the texture, but not both
				material.diffuseColor.r = material.diffuseColor.g = material.diffuseColor.b = 1;
			}
		}

		if (!material.specularTexture.empty())
		{
			const std::string path = loader.dir + material.specularTexture;
			CString fileName(path.c_str());
			specularTexture = BodyProperties::LoadTexture(fileName, 2);
			if (specularTexture)
				specularTexture->GenerateMipmaps();
			/*
			else {
				CString str = L"Specular texture problem: ";
				str += path.c_str();
				AfxMessageBox(str);
			}
			*/
		}

		if (!material.exponentTexture.empty())
		{
			const std::string path = loader.dir + material.exponentTexture;
			CString fileName(path.c_str());
			// I have to find an obj file with an exponent texture to check what is their format: 8bpp or 3 bytes
			exponentTexture = BodyProperties::LoadTexture(fileName, 3/*, 8*/);
			if (exponentTexture)
				exponentTexture->GenerateMipmaps();
			/*
			else {
				CString str = L"Exponent texture problem: ";
				str += path.c_str();
				AfxMessageBox(str);
			}
			*/
		}

		
		if (!material.bumpTexture.empty())
		{
			const std::string path = loader.dir + material.bumpTexture;
			CString fileName(path.c_str());
			bumpTexture = BodyProperties::LoadNormalTexture(fileName, 2, 4);
			if (bumpTexture)
				bumpTexture->GenerateMipmaps();
			/*
			else {
				CString str = L"Bump texture problem: ";
				str += path.c_str();
				AfxMessageBox(str);
			}
			*/
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
		
		if (bumpTexture)
		{
			bumpTexture->Bind(4);
			glUniform1i(program.useBumpTextureLocation, 1);
		}
		else
		{
			glUniform1i(program.useBumpTextureLocation, 0);
		}

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

