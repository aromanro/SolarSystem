#include "stdafx.h"
#include "MemoryBitmap.h"

#include "Options.h"

#include <cassert>

MemoryBitmap::MemoryBitmap(int width, int height)
	: m_width(width), m_height(height), data(nullptr)
{	
	if (m_width > 0 && m_height > 0)
		SetSize(m_width, m_height);
}


MemoryBitmap::MemoryBitmap(const MemoryBitmap& other) // copy constructor
{
	if (other.data)
	{
		m_width = other.m_width;
		m_height = other.m_height;
		int size = GetStrideLength() * m_height;
		data = new unsigned char[size];
		memcpy(data, other.data, size);
	}
	else {
		data = nullptr;
		m_width = m_height = 0;
	}
}

MemoryBitmap::MemoryBitmap(MemoryBitmap&& other) noexcept // move constructor
	: data(other.data), m_width(other.m_width), m_height(other.m_height)
{
	other.data = nullptr;
	other.m_height = other.m_width = 0;
}

MemoryBitmap& MemoryBitmap::operator=(const MemoryBitmap& other) //copy assignment operator
{
	MemoryBitmap temp(other);
	
	*this = std::move(temp);

	return *this;
}

MemoryBitmap& MemoryBitmap::operator=(MemoryBitmap&& other) noexcept // move assignment operator
{
	delete[] data;
	
	m_width = other.m_width;
	m_height = other.m_height;

	data = other.data;

	other.m_height = other.m_width = 0;
	other.data = nullptr;

	return *this;
}

MemoryBitmap::~MemoryBitmap() noexcept
{
	delete[] data;
}


void MemoryBitmap::SetSize(int width, int height)
{
	assert(width != 0 && height != 0);

	if (m_width != width || m_height != height)
	{
		delete[] data;

		m_width = width;
		m_height = height;

		data = new unsigned char[static_cast<size_t>(GetStrideLength()) * height];
	}
}


void MemoryBitmap::Draw(CDC* pDC)
{
	BITMAPINFO bmi;
	ZeroMemory(&bmi, sizeof(BITMAPINFOHEADER));
	
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = m_width;
	bmi.bmiHeader.biHeight = m_height;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 24;
	bmi.bmiHeader.biCompression = BI_RGB;

	CBitmap bitmap;
	
	bitmap.CreateCompatibleBitmap(pDC, m_width, m_height);
	::SetDIBits(pDC->GetSafeHdc(), bitmap, 0, m_height, data, &bmi, DIB_RGB_COLORS);
	CDC dcMemory;
	dcMemory.CreateCompatibleDC(pDC);
	CBitmap * pOldBitmap = dcMemory.SelectObject(&bitmap);
	pDC->BitBlt(0, 0, m_width, m_height, &dcMemory, 0, 0, SRCCOPY);
	dcMemory.SelectObject(pOldBitmap);
}

void MemoryBitmap::Draw(CDC* pDC, CRect& rect, int origWidth, int origHeight)
{
	BITMAPINFO bmi;
	ZeroMemory(&bmi, sizeof(BITMAPINFOHEADER));

	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = m_width;
	bmi.bmiHeader.biHeight = m_height;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 24;
	bmi.bmiHeader.biCompression = BI_RGB;

	CBitmap bitmap;

	bitmap.CreateCompatibleBitmap(pDC, m_width, m_height);
	::SetDIBits(pDC->GetSafeHdc(), bitmap, 0, m_height, data, &bmi, DIB_RGB_COLORS);
	CDC dcMemory;
	dcMemory.CreateCompatibleDC(pDC);
	CBitmap * pOldBitmap = dcMemory.SelectObject(&bitmap);
	pDC->StretchBlt(rect.left, rect.top, rect.Width(), rect.Height(), &dcMemory, origWidth ? (m_width - origWidth)/2 : 0, origHeight ? (m_height - origHeight)/2 : 0, origWidth ? origWidth : m_width, origHeight ? origHeight : m_height, SRCCOPY);
	dcMemory.SelectObject(pOldBitmap);
}


void MemoryBitmap::WriteText(const char* text, CFont& font, DWORD color, DWORD bkcolor)
{
	BITMAPINFO bmi;
	ZeroMemory(&bmi, sizeof(BITMAPINFOHEADER));

	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = m_width;
	bmi.bmiHeader.biHeight = m_height;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 24;
	bmi.bmiHeader.biCompression = BI_RGB;

	CBitmap bitmap;
	CDC dcMemory;

	CDC* pDC = CDC::FromHandle(GetDC(NULL));
	dcMemory.CreateCompatibleDC(pDC);
	bitmap.CreateCompatibleBitmap(pDC, m_width, m_height);
	pDC->DeleteDC();

	CBitmap* pOldBitmap = dcMemory.SelectObject(&bitmap);

	CFont* oldFont = dcMemory.SelectObject(&font);

	RECT rect;
	rect.left = rect.top = 0;
	rect.right = m_width;
	rect.bottom = m_height;

	int nOldBkMode = dcMemory.SetBkMode(TRANSPARENT);
	COLORREF oldBkColor = dcMemory.SetBkColor(bkcolor);
	COLORREF oldForeColor = dcMemory.SetTextColor(color);

	dcMemory.FillSolidRect(&rect, bkcolor);

	dcMemory.DrawText(CString(text), &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	::GetDIBits(dcMemory.GetSafeHdc(), bitmap, 0, m_height, data, &bmi, DIB_RGB_COLORS);

	dcMemory.SetBkMode(nOldBkMode);
	dcMemory.SetBkColor(oldBkColor);
	dcMemory.SetTextColor(oldForeColor);

	dcMemory.SelectObject(oldFont);
	dcMemory.SelectObject(pOldBitmap);	
}

void MemoryBitmap::SetIntoTexture(OpenGL::Texture& texture)
{
	if (!data) return;

	const int stride = GetStrideLength();

	const size_t s = 3ULL * m_height * m_width;
	texdata.resize(s);

	for (int line = 0; line < m_height; ++line)
	{
		const int startLine = (m_height - line - 1) * stride;

		for (int horz = 0; horz < 3 * m_width; horz += 3)
		{
			const int pos = startLine + horz;
			const int posout = line * m_width * 3 + horz;

			texdata[posout] = data[pos];
			texdata[posout + 1ULL] = data[pos + 1ULL];
			texdata[posout + 2ULL] = data[pos + 2ULL];
		}
	}

	texture.setData(texdata.data(), m_width, m_height);
}






