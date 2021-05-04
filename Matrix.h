#pragma once
#include <cstdint>
#include <stdexcept>
#include <initializer_list>
#include <vector>
#include <iostream>

class Matrix
{
public:
	Matrix(size_t width, size_t height);
	Matrix(std::initializer_list<std::initializer_list<float>> data);

	Matrix& operator= (const Matrix& matrix) = default;
	Matrix operator* (float scalar) const;
	Matrix& operator*= (float scalar);
	Matrix operator+ (Matrix& matrix) const;
	Matrix& operator+= (Matrix& matrix);
	Matrix operator- (Matrix& matrix) const;
	Matrix& operator-= (Matrix& matrix);
	Matrix operator- () const;

	size_t width() const;
	size_t height() const;
	float* data();
	float get(int32_t x, int32_t y) const;
	float set(int32_t x, int32_t y, float value);

	friend std::ostream& operator<< (std::ostream& os, Matrix& matrix);

private:
	std::vector<float> m_data;
	size_t m_width;
	size_t m_height;
};

inline size_t Matrix::width() const
{
	return m_width;
}

inline size_t Matrix::height() const
{
	return m_height;
}

inline float* Matrix::data()
{
	return m_data.data();
}

inline float Matrix::get(int32_t x, int32_t y) const
{
	return m_data[y * m_width + x];
}

inline float Matrix::set(int32_t x, int32_t y, float value)
{
	return m_data[y * m_width + x] = value;
}
