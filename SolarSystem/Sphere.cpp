#include "stdafx.h"
#include "Sphere.h"

#include "Constants.h"

#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace OpenGL {

	Sphere::Sphere(float r, unsigned int lats, unsigned int longs)
		: m_lats(lats), m_longs(longs)
	{
		std::vector<GLfloat> buffer;
		const int STRIDE_SIZE = 8; // 3 for vertex, 3 for normal, 2 for texture coordinates
		const int STEP_SIZE = 2 * STRIDE_SIZE; // 2 vertices at once

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

				double x = r0 * c;
				double y = r0 * s;

				double XTex = static_cast<double>(j) / longs;
				double YTex0 = static_cast<double>(i) / lats;
				double YTex1 = (static_cast<double>(i) + 1.) / lats;

				double lng = sqrt(x*x + y*y + z0*z0);

				double x1 = r1 * c;
				double y1 = r1 * s;
				double lng1 = sqrt(x1*x1 + y1*y1 + z1*z1);

				// vertex
				const size_t base = i * (longs + 1ULL) * STEP_SIZE + j * STEP_SIZE;
				buffer[base] = static_cast<float>(x1);
				buffer[base + 1] = static_cast<float>(y1);
				buffer[base + 2] = static_cast<float>(z1);

				// normal
				buffer[base + 3] = static_cast<float>(x1 / lng1);
				buffer[base + 4] = static_cast<float>(y1 / lng1);
				buffer[base + 5] = static_cast<float>(z1 / lng1);

				// texture coordinate
				buffer[base + 6] = static_cast<float>(XTex);
				buffer[base + 7] = static_cast<float>(YTex1);


				// vertex 
				buffer[base + 8] = static_cast<float>(x);
				buffer[base + 9] = static_cast<float>(y);
				buffer[base + 10] = static_cast<float>(z0);

				// normal
				buffer[base + 11] = static_cast<float>(x / lng);
				buffer[base + 12] = static_cast<float>(y / lng);
				buffer[base + 13] = static_cast<float>(z0 / lng);

				// texture coordinate
				buffer[base + 14] = static_cast<float>(XTex);
				buffer[base + 15] = static_cast<float>(YTex0);
			}
		}

		Renderable::Bind();
		setData(buffer.data(), length * sizeof(GLfloat));

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, STRIDE_SIZE * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, STRIDE_SIZE * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, STRIDE_SIZE * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
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

}