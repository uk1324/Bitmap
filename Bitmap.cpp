#include "Bitmap.h"

Bitmap::Bitmap(const Bitmap& bitmap)
	:m_width(bitmap.m_width)
	, m_height(bitmap.m_height)
	, m_pixelData(new Color[bitmap.m_width * bitmap.m_height])
{
	memcpy(m_pixelData, bitmap.m_pixelData, m_width * m_height);
}

Bitmap::Bitmap(Bitmap&& bitmap) noexcept
	:m_pixelData(bitmap.m_pixelData)
	, m_width(bitmap.m_width)
	, m_height(bitmap.m_height)
{
	bitmap.m_pixelData = nullptr;
}

Bitmap::Bitmap(int width, int height)
	:m_pixelData(new Color[width * height])
	, m_width(width)
	, m_height(height)
{}

Bitmap::~Bitmap()
{
	delete[] m_pixelData;
}

Bitmap::Bitmap(const char* filename) {
	std::ifstream file(filename, std::ios::in | std::ios::binary);

	if (file.fail())
		throw std::runtime_error("Couldn't open file.");

	const std::streampos start = file.tellg();
	file.seekg(0, std::ios::end);
	const std::streampos end = file.tellg();
	uint32_t fileSize = uint32_t(end) - uint32_t(start);

	file.seekg(start);

	bmpFileHeader fileHeader;
	file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));

	if (fileHeader.type != 0x4D42)
		throw std::runtime_error("File signature doesn't match the BMP file signature.");

	uint32_t infoHeaderSize;
	bmpInfoHeader infoHeader;

	std::streampos infoHeaderStart = file.tellg();
	file.read(reinterpret_cast<char*>(&infoHeaderSize), 4);
	file.seekg(infoHeaderStart);

	if (infoHeaderSize + sizeof(fileHeader) >= fileSize)
		throw std::runtime_error("Headers too big.");

	file.read(reinterpret_cast<char*>(&infoHeader), std::min(infoHeaderSize, static_cast<uint32_t>(sizeof(infoHeader))));

	//std::cout << (infoHeader.height * (infoHeader.width * (infoHeader.bitCount / 8) + ((4 -((infoHeader.width * (infoHeader.bitCount / 8)) % 4)) % 4))) << "\n";
	//std::cout << (fileSize - infoHeaderSize - sizeof(fileHeader)) << "\n";

	m_width = infoHeader.width;
	m_height = infoHeader.height;

	m_pixelData = new Color[m_width * m_height];

	file.seekg(fileHeader.offset, std::ios::beg);

	if (infoHeader.bitCount == 24 && infoHeader.compression == bmpCompression::BI_RGB)
	{
		// Every row in a BMP image has to align to 32 bits.
		int paddingSize = (4 - ((m_width * 3) % 4)) % 4;
		char* padding = nullptr;
		if (paddingSize) padding = new char[paddingSize];

		for (int y = m_height - 1; y >= 0; y--)
		{
			for (int x = 0; x < m_width; x++)
			{
				file.read(reinterpret_cast<char*>(&getPixel(x, y)), 3);
				setPixel(x, y, Color(getPixel(x, y).value << 8 | 0xFF));
			}
			if (paddingSize)
				file.read(padding, paddingSize);
		}
		if (paddingSize)
			delete[] padding;
	}
	else if (infoHeader.bitCount == 32 && infoHeader.compression == bmpCompression::BI_BITFIELDS)
	{
		// Bitmasks match how the colors are stored inside this class - faster loading.
		if (infoHeader.redBitmask == 0xFF000000 && infoHeader.greenBitmask == 0x00FF0000 && infoHeader.blueBitmask == 0x0000FF00)
		{
			// Iterating backwards because BMP pixel data starts from the bottom left corner of an image.
			for (int y = m_height - 1; y >= 0; y--)
			{
				file.read(reinterpret_cast<char*>(&getPixel(0, y)), 4 * static_cast<std::streamsize>(infoHeader.width));
			}
		}
		else
		{
			uint32_t redBitOffset   = __builtin_ctz(infoHeader.redBitmask);
			uint32_t greenBitOffset = __builtin_ctz(infoHeader.greenBitmask);
			uint32_t blueBitOffset  = __builtin_ctz(infoHeader.blueBitmask);
			uint32_t alphaBitOffset = __builtin_ctz(infoHeader.alphaBitmask);

			// Iterating backwards because BMP pixel data starts from the bottom left corner of an image.
			for (int y = m_height - 1; y >= 0; y--)
			{
				for (int x = 0; x < m_width; x++)
				{
					uint32_t value;
					file.read(reinterpret_cast<char*>(&value), 4);
					setPixel(
						x, y, 
						Color(
							static_cast<uint8_t>((value & infoHeader.redBitmask)   >> redBitOffset),
							static_cast<uint8_t>((value & infoHeader.greenBitmask) >> greenBitOffset),
							static_cast<uint8_t>((value & infoHeader.blueBitmask)  >> blueBitOffset),
							static_cast<uint8_t>((value & infoHeader.alphaBitmask) >> alphaBitOffset)
						)
					);
				}
			}
		}
		
	}
	else
	{
		if (infoHeader.compression != bmpCompression::BI_BITFIELDS && infoHeader.compression != bmpCompression::BI_RGB)
			throw std::runtime_error("Unsupported compression type.");
		throw std::runtime_error("Unsupported color bitcount.");
	}

	file.close();
}

Bitmap& Bitmap::operator= (const Bitmap& bitmap)
{
	if (&bitmap == this)
		return *this;

	delete[] m_pixelData;

	m_pixelData = new Color[bitmap.m_width * bitmap.m_height];
	memcpy(m_pixelData, bitmap.m_pixelData, bitmap.m_width * bitmap.m_height);
	m_width = bitmap.m_width;
	m_height = bitmap.m_height;

	return *this;
}

Bitmap& Bitmap::operator= (Bitmap&& bitmap) noexcept
{
	delete[] m_pixelData;

	m_width = bitmap.m_width;
	m_height = bitmap.m_height;
	m_pixelData = bitmap.m_pixelData;

	bitmap.m_pixelData = nullptr;

	return *this;
}

void Bitmap::drawLine(int x1, int y1, int x2, int y2, const Color color)
{
	int x, y;

	int deltaX = abs(x1 - x2);
	int deltaY = abs(y1 - y2);

	int stepX = x1 < x2 ? 1 : -1;
	int stepY = y1 < y2 ? 1 : -1;

	int distance;

	if (deltaX > deltaY)
	{
		distance = 2 * deltaY - deltaX;
		y = y1;
		x2 += stepX;

		for (x = x1; x != x2; x += stepX)
		{
			if (x >= 0 && x < m_width && y >= 0 && y < m_height)
				setPixel(x, y, color);

			if (distance > 0)
			{
				y += stepY;
				distance += 2 * (deltaY - deltaX);
			}
			else
				distance += 2 * deltaY;
		}
	}
	else
	{
		distance = 2 * deltaX - deltaY;
		x = x1;
		y2 += stepY;

		for (y = y1; y != y2; y += stepY)
		{
			if (x >= 0 && x < m_width && y >= 0 && y < m_height)
				setPixel(x, y, color);

			if (distance > 0)
			{
				x += stepX;
				distance += 2 * (deltaX - deltaY);
			}
			else
				distance += 2 * deltaX;
		}
	}
}

void Bitmap::drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, Color color)
{
	drawLine(x1, y1, x2, y2, color);
	drawLine(x2, y2, x3, y3, color);
	drawLine(x1, y1, x3, y3, color);
}

void Bitmap::fillTriangle(float x1, float y1, float x2, float y2, float x3, float y3, Color color)
{
	// round here maybe idk
	int minX = std::clamp(std::min(x1, std::min(x2, x3)), 0.f, static_cast<float>(m_width));
	int maxX = std::clamp(std::max(x1, std::max(x2, x3)), 0.f, static_cast<float>(m_width));

	int minY = std::clamp(std::min(y1, std::min(y2, y3)), 0.f, static_cast<float>(m_height));
	int maxY = std::clamp(std::max(y1, std::max(y2, y3)), 0.f, static_cast<float>(m_height));

	float area = (x1 - x3) * (y2 - y3) - (x2 - x3) * (y1 - y3);
	float weight1, weight2, weight3;
	bool hasNeg, hasPos;

	for (float x = minX; x <= maxX; x++)
	{
		for (float y = minY; y <= maxY; y++)
		{
			// Calculate barycentric weights using 2D cross product
			weight1 = (x - x2) * (y1 - y2) - (x1 - x2) * (y - y2);
			weight2 = (x - x3) * (y2 - y3) - (x2 - x3) * (y - y3);
			weight3 = (x - x1) * (y3 - y1) - (x3 - x1) * (y - y1);

			hasNeg = (weight1 < 0) || (weight2 < 0) || (weight3 < 0);
			hasPos = (weight1 > 0) || (weight2 > 0) || (weight3 > 0);

			if (!(hasNeg && hasPos))
				setPixel(x, y, color);
		}
	}
}

void Bitmap::fillTriangleInterpolate(float x1, float y1, float x2, float y2, float x3, float y3, Color color1, Color color2, Color color3)
{
	int minX = std::clamp(std::min(x1, std::min(x2, x3)), 0.f, static_cast<float>(m_width));
	int maxX = std::clamp(std::max(x1, std::max(x2, x3)), 0.f, static_cast<float>(m_width));

	int minY = std::clamp(std::min(y1, std::min(y2, y3)), 0.f, static_cast<float>(m_height));
	int maxY = std::clamp(std::max(y1, std::max(y2, y3)), 0.f, static_cast<float>(m_height));

	float area = (x1 - x3) * (y2 - y3) - (x2 - x3) * (y1 - y3);
	float weight1, weight2, weight3;
	bool hasNeg, hasPos;
	float r, g, b, a;

	for (float x = minX; x <= maxX; x++)
	{
		for (float y = minY; y <= maxY; y++)
		{
			// Calculate barycentric weights using 2D cross product
			weight1 = (x - x2) * (y1 - y2) - (x1 - x2) * (y - y2);
			weight2 = (x - x3) * (y2 - y3) - (x2 - x3) * (y - y3);
			weight3 = (x - x1) * (y3 - y1) - (x3 - x1) * (y - y1);

			hasNeg = (weight1 < 0) || (weight2 < 0) || (weight3 < 0);
			hasPos = (weight1 > 0) || (weight2 > 0) || (weight3 > 0);

			if (!(hasNeg && hasPos))
			{
				weight1 /= area;
				weight2 /= area;
				weight3 /= area;

				r = weight1 * color1.r + weight2 * color2.r + weight3 * color3.r;
				g = weight1 * color1.g + weight2 * color2.g + weight3 * color3.g;
				b = weight1 * color1.b + weight2 * color2.b + weight3 * color3.b;
				a = weight1 * color1.a + weight2 * color2.a + weight3 * color3.a;
				setPixel(x, y, Color(r, g, b, a));
			}
		}
	}
}

void Bitmap::drawRect(int x, int y, int width, int height, const Color color)
{
	for (int i = x; i < x + width; i++)
	{
		setPixel(i, y, color);
		setPixel(i, y + height - 1, color);
	}

	for (int j = y; j < y + height; j++)
	{
		setPixel(x, j, color);
		setPixel(x + width - 1, j, color);
	}
}

void Bitmap::fillRect(int x, int y, int width, int height, const Color color)
{
	for (int i = x; i < x + width; i++)
	{
		for (int j = y; j < y + height; j++)
		{
			setPixel(i, j, color);
		}
	}
}

void Bitmap::drawBitmap(int x, int y, Bitmap& bitmap)
{
	int minX = std::clamp(x, 0, m_width);
	int minY = std::clamp(y, 0, m_height);
	int maxX = std::clamp(x + bitmap.m_width, 0, m_width);
	int maxY = std::clamp(y + bitmap.m_height, 0, m_height);

	for (int i = minX; i < maxX; i++)
	{
		for (int j = minY; j < maxY; j++)
		{
			Color pixel = bitmap.getPixel(i - x, j - y);
			if (pixel.a > 128)
				setPixel(i, j, pixel);
		}
	}
}

void Bitmap::applyConvolutionFilter(Matrix matrix)
{
	// might change it to static_assert
	if (!(matrix.width() & 1 && matrix.height() & 1))
		throw std::runtime_error("Matrix width and height must be an odd number.");

	int32_t halfMatWidth = matrix.width() / 2;
	int32_t halfMatHeight = matrix.height() / 2;

	Bitmap output(m_width, m_height);

	int32_t x, y, mX, mY;
	float mVal;
	Color currentColor;
	for (y = 0; y < m_height; y++)
	{
		for (x = 0; x < m_width; x++)
		{
			float color[3]{ 0 };
			for (mY = 0;  mY < matrix.height(); mY++)
			{
				for (mX = 0; mX < matrix.width(); mX++)
				{
					mVal = matrix.get(mX, mY);
					currentColor = getPixel(std::clamp(x + mX - halfMatWidth, 0, m_width - 1), std::clamp(y + mY - halfMatHeight, 0, m_height - 1));
					color[0] += currentColor.r * mVal;
					color[1] += currentColor.g * mVal;
					color[2] += currentColor.b * mVal;
				}
			}
			output.setPixel(x, y, Color(std::clamp(color[0], 0.f, 255.f), std::clamp(color[1], 0.f, 255.f), std::clamp(color[2], 0.f, 255.f)));
		}
	}
	*this = std::move(output);
}

void Bitmap::applyMedianFilter(size_t radius)
{
	Bitmap output(m_width, m_height);

	int32_t x, y, mX, mY;
	float mVal;
	Color currentColor;
	for (y = 0; y < m_height; y++)
	{
		for (x = 0; x < m_width; x++)
		{
			std::vector<std::pair<uint8_t, Color>> values;
			for (mY = 0; mY < radius * 2 + 1; mY++)
			{
				for (mX = 0; mX < radius * 2 + 1; mX++)
				{
					currentColor = getPixel(std::clamp(x + mX - static_cast<int32_t>(radius), 0, m_width - 1), std::clamp(y + mY - static_cast<int32_t>(radius), 0, m_height - 1));
					values.push_back(std::pair<uint8_t, Color>(currentColor.grayscale().r, currentColor));
				}
			}
			struct {
				bool operator()(std::pair<uint8_t, Color> a, std::pair<uint8_t, Color> b) const { return a.first < b.first; }
			} customLess;
			std::sort(values.begin(), values.end(), customLess);
			int mid = ((radius * 2 + 1) * (radius * 2 + 1)) / 2;
			output.setPixel(x, y, values[mid].second);
		}
	}
	*this = std::move(output);
}

void Bitmap::clear()
{
	memset(m_pixelData, 0, m_width * m_height * sizeof(Color));
}

void Bitmap::saveToPpm(const char* filename) const
{
	std::fstream file;
	file.open(filename);

	file << "P3\n" << m_width << ' ' << m_height << "\n255\n";

	for (int y = 0; y < m_height; y++)
	{
		for (int x = 0; x < m_width; x++)
		{
			Color p = getPixel(x, y);
			file << static_cast<int>(p.r) << '\t' << static_cast<int>(p.g) << '\t' << static_cast<int>(p.b) << '\n';
		}
	}

	file.close();
}

void Bitmap::saveToBmp(const char* filename, uint16_t bitCount) const
{
	std::fstream file(filename, std::ios::out | std::ios::binary);

	if (bitCount == 32)
	{
		bmpFileHeader fileHeader{ 0x4D42, static_cast<uint32_t>(sizeof(bmpFileHeader)) + static_cast<uint32_t>(sizeof(bmpInfoHeader)) + 4 * static_cast<uint32_t>(m_width * m_height), 0, sizeof(bmpFileHeader) + sizeof(bmpInfoHeader) };
		bmpInfoHeader infoHeader{ sizeof(bmpInfoHeader), static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height), 1, bitCount, bmpCompression::BI_BITFIELDS, 4 * static_cast<uint32_t>(m_height * m_width), 2835, 2835, 0, 0, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF, 0x57696E20 };

		file.write(reinterpret_cast<char*>(&fileHeader), sizeof(bmpFileHeader));
		file.write(reinterpret_cast<char*>(&infoHeader), sizeof(bmpInfoHeader));

		// Iterating backwards because BMP pixel data starts from the bottom left corner of an image.
		for (int y = m_height - 1; y >= 0; y--)
		{
			file.write(
				reinterpret_cast<char*>(m_pixelData) + sizeof(Color) * m_width * y,
				static_cast<std::streamsize>(sizeof(Color)) * static_cast<std::streamsize>(m_width)
			);
		}
	}
	else if (bitCount == 24)
	{
		int paddingSize = (4 - ((m_width * 3) % 4)) % 4;
		uint32_t imageSize = m_width * (m_height * 3 + paddingSize);

		bmpFileHeader fileHeader{ 0x4D42, static_cast<uint32_t>(sizeof(bmpFileHeader)) + static_cast<uint32_t>(sizeof(bmpInfoHeader)) + imageSize, 0, sizeof(bmpFileHeader) + sizeof(bmpInfoHeader) };
		bmpInfoHeader infoHeader{ sizeof(bmpInfoHeader), static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height), 1, bitCount, bmpCompression::BI_RGB, imageSize, 2835, 2835, 0, 0, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF, 0x57696E20 };

		file.write(reinterpret_cast<char*>(&fileHeader), sizeof(bmpFileHeader));
		file.write(reinterpret_cast<char*>(&infoHeader), sizeof(bmpInfoHeader));

		char* padding = nullptr;
		if (paddingSize) padding = new char[paddingSize];

		for (int y = m_height - 1; y >= 0; y--)
		{
			for (int x = 0; x < m_width; x++)
			{
				uint32_t value = getPixel(x, y).value >> 8;
				file.write(reinterpret_cast<const char*>(&value), 3);
			}
			if (paddingSize)
				file.write(padding, paddingSize);
		}
		if (paddingSize)
			delete[] padding;
	}
	else
	{
		file.close();
		throw std::runtime_error("Unsupported color bitcount.");
	}

	file.close();
}