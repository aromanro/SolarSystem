#pragma once

#include <algorithm>
#include <vector>

#include "Texture.h"

// to use direct transfer (that is, not copying data into 'texdata'
// the stride must be of proper length, so not all resolutions will work

#define USE_DIRECT_TRANSFER 1

class MemoryBitmap
{
public:
	MemoryBitmap(int width = 0, int height = 0);

	MemoryBitmap(const MemoryBitmap& other); // copy constructor
	MemoryBitmap(MemoryBitmap&& other) noexcept; // move constructor
	MemoryBitmap& operator=(const MemoryBitmap& other); //copy assignment operator
	MemoryBitmap& operator=(MemoryBitmap&& other) noexcept; // move assignment operator

	~MemoryBitmap() noexcept;
protected:
	int m_width;
	int m_height;

	unsigned char* data;

#if USE_DIRECT_TRANSFER == 0
	std::vector<unsigned char> texdata;
#endif

public:
	void SetSize(int width, int height);

	inline void FillSquare(int Xpos, int Ypos, int size, COLORREF color)
	{
		if (!data) return;

		const int stride = GetStrideLength();

		const int limX = 3 * std::min<int>(Xpos + size, m_width);
		const int limY = std::min<int>(Ypos + size, m_height);

		for (int line = Ypos; line < limY; ++line)
		{
			const int startLine = (m_height - line - 1) * stride;

			for (int horz = 3 * Xpos; horz < limX; horz += 3)
			{
				const int pos = startLine + horz;

				data[pos] = GetBValue(color);
				data[pos + 1] = GetGValue(color);
				data[pos + 2] = GetRValue(color);
			}
		}
	}

	inline void SetPixel(int Xpos, int Ypos, COLORREF color)
	{
		if (!data) return;

		const int stride = GetStrideLength();

		const int startLine = (m_height - Ypos - 1) * stride;
		const int pos = startLine + static_cast<int>(3. * Xpos);

		data[pos] = GetBValue(color);
	    data[pos + 1] = GetGValue(color);
		data[pos + 2] = GetRValue(color);
	}

	inline int GetStrideLength() const {
		return 4 * ((m_width * 3 + 3) / 4);
	}

	const unsigned char* GetData() const { return data; }

	void Draw(CDC* pDC);
	void Draw(CDC* pDC, CRect& rect, int origWidth = 0, int origHeight = 0);

	void WriteText(const char* text, CFont& font, DWORD color = RGB(0, 0, 255), DWORD bkcolor = RGB(0, 0, 0));

	void SetIntoTexture(OpenGL::Texture& texture, int nr = 0);

	void Save(const CString& name);
};

