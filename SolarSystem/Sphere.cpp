#include "stdafx.h"
#include "Sphere.h"

#include "Constants.h"

#include <vector>

#include <glm.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace OpenGL {

	Sphere::Sphere(float r, unsigned int lats, unsigned int longs)
		: m_lats(lats), m_longs(longs)
	{
		std::vector<GLfloat> buffer;
		static const int STRIDE_SIZE = 11; // 3 for vertex, 3 for normal, 2 for texture coordinates, 3 for tangent
		static const int STEP_SIZE = 2 * STRIDE_SIZE; // 2 vertices at once

		unsigned int length = (lats + 1) * (longs + 1) * STEP_SIZE;
		buffer.resize(length); 

		for (size_t i = 0; i < lats; ++i) {
			const double lat0 = M_PI * (-0.5 + static_cast<double>(i) / lats);
			const double z0 = r*sin(lat0);
			const double r0 = r*cos(lat0);

			const double lat1 = M_PI * (-0.5 + (i + 1.) / lats);
			const double z1 = r*sin(lat1);
			const double r1 = r*cos(lat1);

			for (size_t j = 0; j <= longs; ++j) {
				const double longitude = 2. * M_PI * static_cast<double>(j) / longs;

				const double c = cos(longitude);
				const double s = sin(longitude);

				double x0 = r0 * c;
				double y0 = r0 * s;

				double XTex = static_cast<double>(j) / longs;
				double YTex0 = static_cast<double>(i) / lats;
				double YTex1 = (static_cast<double>(i) + 1.) / lats;

				double x1 = r1 * c;
				double y1 = r1 * s;

				// first vertex

				// vertex
				const size_t base = i * (longs + 1ULL) * STEP_SIZE + j * STEP_SIZE;
				buffer[base] = static_cast<float>(x1);
				buffer[base + 1] = static_cast<float>(y1);
				buffer[base + 2] = static_cast<float>(z1);

				// normal
				buffer[base + 3] = static_cast<float>(x1 / r);
				buffer[base + 4] = static_cast<float>(y1 / r);
				buffer[base + 5] = static_cast<float>(z1 / r);

				// texture coordinate
				buffer[base + 6] = static_cast<float>(XTex);
				buffer[base + 7] = static_cast<float>(YTex1);

				glm::vec3 normal(buffer[base + 3], buffer[base + 4], buffer[base + 5]);
				
				const glm::vec3 approxBitangent(x1- x0, y1 - y0, z1 - z0);
				
				glm::vec3 bitangent = approxBitangent - glm::dot(approxBitangent, normal) * normal; // subtract the projection on the direction of the normal, now it's orthogonal
				bitangent = glm::normalize(bitangent); // now it's orthonormal

				// the other one is simply the cross product
				glm::vec3 tangent = glm::cross(bitangent, normal);
				tangent = glm::normalize(tangent); // this is not really necessary

				// tangent
				buffer[base + 8] = static_cast<float>(tangent.x);
				buffer[base + 9] = static_cast<float>(tangent.y);
				buffer[base + 10] = static_cast<float>(tangent.z);

				// second vertex

				// vertex 
				buffer[base + STRIDE_SIZE] = static_cast<float>(x0);
				buffer[base + STRIDE_SIZE + 1] = static_cast<float>(y0);
				buffer[base + STRIDE_SIZE + 2] = static_cast<float>(z0);

				// normal
				buffer[base + STRIDE_SIZE + 3] = static_cast<float>(x0 / r);
				buffer[base + STRIDE_SIZE + 4] = static_cast<float>(y0 / r);
				buffer[base + STRIDE_SIZE + 5] = static_cast<float>(z0 / r);

				// texture coordinate
				buffer[base + STRIDE_SIZE + 6] = static_cast<float>(XTex);
				buffer[base + STRIDE_SIZE + 7] = static_cast<float>(YTex0);

				normal = glm::vec3(buffer[base + STRIDE_SIZE + 3], buffer[base + STRIDE_SIZE + 4], buffer[base + STRIDE_SIZE + 5]);
				bitangent = approxBitangent - glm::dot(approxBitangent, normal) * normal; // subtract the projection on the direction of the normal, now it's orthogonal
				bitangent = glm::normalize(bitangent); // now it's orthonormal
				
				// the other one is simply the cross product
				tangent = glm::cross(bitangent, normal);
				tangent = glm::normalize(tangent); // this is not really necessary

				// tangent
				buffer[base + STRIDE_SIZE + 8] = static_cast<float>(tangent.x);
				buffer[base + STRIDE_SIZE + 9] = static_cast<float>(tangent.y);
				buffer[base + STRIDE_SIZE + 10] = static_cast<float>(tangent.z);
			}
		}

		Renderable::Bind();
		setData(buffer.data(), length * sizeof(GLfloat));

		// vertices
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, STRIDE_SIZE * sizeof(GLfloat), (GLvoid*)0);

		// normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, STRIDE_SIZE * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));

		// texture coordinates
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, STRIDE_SIZE * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));

		// tangent
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, STRIDE_SIZE * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
	}


	Sphere::~Sphere()
	{
	}

	void Sphere::Draw()
	{
		VertexBufferRenderable::Bind();

		for (unsigned int i = 0; i < m_lats; ++i)			
			glDrawArrays(GL_TRIANGLE_STRIP, static_cast<GLint>(i * (m_longs + 1) * 2 ) , static_cast<GLsizei>(2 * (m_longs + 1)));

		//VertexBufferRenderable::UnBind();
	}

	void Sphere::DrawInstanced(unsigned int count)
	{
		VertexBufferRenderable::Bind();

		for (unsigned int i = 0; i < m_lats; ++i)
			glDrawArraysInstanced(GL_TRIANGLE_STRIP, static_cast<GLint>(i * (m_longs + 1) * 2), static_cast<GLsizei>(2 * (m_longs + 1)), count);
	}
}