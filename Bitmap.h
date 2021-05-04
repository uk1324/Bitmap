#pragma once
#include <cstdint>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>

#include "Color.h"
#include "BmpHeaders.h"
#include "Matrix.h"

class Bitmap
{
public:
	Bitmap() = delete;
	Bitmap(const Bitmap& bitmap);
	Bitmap(Bitmap&& bitmap) noexcept;
	Bitmap(int width, int height);
	Bitmap(const char* filename);
	~Bitmap();

	Bitmap& operator= (const Bitmap& bitmap);
	Bitmap& operator= (Bitmap&& bitmap) noexcept;

	Color* pixelData();
	int32_t width() const;
	int32_t height() const;
	const Color& getPixel(int x, int y) const;
	Color& getPixel(int x, int y);
	void setPixel(int x, int y, const Color color);
	Color* begin();
	Color* end();

	void drawLine(int x1, int y1, int x2, int y2, Color color);
	void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, Color color);
	void fillTriangle(float x1, float y1, float x2, float y2, float x3, float y3, Color color);
	void fillTriangleInterpolate(float x1, float y1, float x2, float y2, float x3, float y3, Color color1, Color color2, Color color3);
	void drawRect(int x, int y, int width, int height, const Color color);
	void fillRect(int x, int y, int width, int height, const Color color);
	void drawBitmap(int x, int y, Bitmap& bitmap);

	void applyConvolutionFilter(Matrix matrix);
	void applyMedianFilter(size_t radius);
	void clear();

	void saveToPPM(const char* filename) const;
	void saveToBMP(const char* filename, uint16_t bitCount = 32) const;

private:
	Color* m_pixelData;
	int32_t m_width;
	int32_t m_height;
};

inline Color* Bitmap::pixelData()
{
	return m_pixelData;
}

inline int32_t Bitmap::width() const
{
	return m_width;
}

inline int32_t Bitmap::height() const {
	return m_height;
}

inline const Color& Bitmap::getPixel(int x, int y) const
{
	return m_pixelData[y * m_width + x];
}

inline Color& Bitmap::getPixel(int x, int y) {
	return m_pixelData[y * m_width + x];
}

inline void Bitmap::setPixel(int x, int y, const Color color)
{
	m_pixelData[y * m_width + x].value = color.value;
}

inline Color* Bitmap::begin()
{
	return m_pixelData;
}

inline Color* Bitmap::end()
{
	return m_pixelData + m_width * m_height;
}