#include "Matrix.h"

Matrix::Matrix(size_t width, size_t height)
	:m_data(std::vector<float>(width * height))
	,m_width(width)
	,m_height(height)
{}

Matrix::Matrix(std::initializer_list<std::initializer_list<float>> data)
	:m_data(std::vector<float>(data.size() * data.begin()->size()))
	,m_width(data.begin()->size())
	,m_height(data.size())
{
	auto position = data.begin();
	size_t colIndex = 0;
	for (auto& col : data)
	{
		if (col.size() != m_width)
			throw std::runtime_error("Inconsistent row count.");
		std::copy(col.begin(), col.end(), m_data.data() + colIndex * m_width);
		colIndex++;
	}
}

Matrix Matrix::operator* (float scalar) const
{
	Matrix output(m_width, m_height);
	for (size_t i = 0; i < m_data.size(); i++)
		output.m_data[i] = m_data[i] * scalar;
	return output;
}

Matrix& Matrix::operator*= (float scalar)
{
	for (float& value : m_data)
		value *= scalar;
	return *this;
}

Matrix Matrix::operator+ (Matrix& matrix) const
{
	Matrix output(m_width, m_height);
	for (size_t i = 0; i < m_data.size(); i++)
		output.m_data[i] = m_data[i] + matrix.m_data[i];
	return output;
}

Matrix& Matrix::operator+= (Matrix& matrix)
{
	for (size_t i = 0; i < m_data.size(); i++)
		m_data[i] = m_data[i] + matrix.m_data[i];
	return *this;
}

Matrix Matrix::operator- (Matrix& matrix) const
{
	Matrix output(m_width, m_height);
	for (size_t i = 0; i < m_data.size(); i++)
		output.m_data[i] = m_data[i] - matrix.m_data[i];
	return output;
}

Matrix& Matrix::operator-= (Matrix& matrix)
{
	for (size_t i = 0; i < m_data.size(); i++)
		m_data[i] = m_data[i] - matrix.m_data[i];
	return *this;
}

Matrix Matrix::operator- () const
{
	Matrix output(m_width, m_height);
	for (size_t i = 0; i < m_data.size(); i++)
		output.m_data[i] = -m_data[i];
	return output;
}

std::ostream& operator<< (std::ostream& os, Matrix& matrix)
{
	for (size_t i = 1; i <= matrix.m_data.size(); i++)
	{
		os << matrix.m_data[i - 1] << ' ';
		if (i % matrix.m_width == 0)
			os << '\n';
	}
	return os;
}