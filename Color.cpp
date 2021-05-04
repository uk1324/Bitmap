#include "Color.h"

Color::Color()
	:value(0)
{}

Color::Color(uint32_t val)
	: value(val)
{}

Color::Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
	: value(red << 24 | green << 16 | blue << 8 | alpha)
{}

const Color Color::Red(255, 0, 0);
const Color Color::Green(0, 255, 0);
const Color Color::Blue(0, 0, 255);
const Color Color::Black(0, 0, 0);
const Color Color::White(255, 255, 255);
const Color Color::Cyan(0, 255, 255);
const Color Color::Magenta(255, 0, 255);
const Color Color::Yellow(255, 255, 0);

Color Color::clamp(int red, int green, int blue, int alpha)
{
	return Color(
		std::clamp(red,   0, 255),
		std::clamp(green, 0, 255),
		std::clamp(blue,  0, 255),
		std::clamp(alpha, 0, 255)
	);
}

Color Color::fromFloat(float red, float green, float blue, float alpha)
{
	return Color(red * 255, green * 255, blue * 255, alpha * 255);
}

bool Color::operator== (const Color& c) const
{
	return value == c.value;
}

bool Color::operator!= (const Color& c) const
{
	return value != c.value;
}

Color Color::operator+ (const Color& rhs) const
{
	return Color(
		static_cast<uint8_t>(std::min(static_cast<int>(r) + rhs.r, 255)),
		static_cast<uint8_t>(std::min(static_cast<int>(g) + rhs.g, 255)),
		static_cast<uint8_t>(std::min(static_cast<int>(b) + rhs.b, 255))
	);
}

Color& Color::operator+= (const Color& rhs)
{
	this->r = static_cast<uint8_t>(std::min(static_cast<int>(r) + rhs.r, 255));
	this->g = static_cast<uint8_t>(std::min(static_cast<int>(g) + rhs.g, 255));
	this->b = static_cast<uint8_t>(std::min(static_cast<int>(b) + rhs.b, 255));
	return *this;
}

Color Color::operator- (const Color& rhs) const
{
	return Color(
		static_cast<uint8_t>(std::max(static_cast<int>(r) - rhs.r, 0)),
		static_cast<uint8_t>(std::max(static_cast<int>(g) - rhs.g, 0)),
		static_cast<uint8_t>(std::max(static_cast<int>(b) - rhs.b, 0)),
		a
	);
}

Color& Color::operator-= (const Color& rhs)
{
	this->r = static_cast<uint8_t>(std::max(static_cast<int>(r) - rhs.r, 0));
	this->g = static_cast<uint8_t>(std::max(static_cast<int>(g) - rhs.g, 0));
	this->b = static_cast<uint8_t>(std::max(static_cast<int>(b) - rhs.b, 0));
	return *this;
}

Color Color::operator* (const float rhs) const
{
	return Color(
		static_cast<uint8_t>(std::clamp(static_cast<float>(r) * rhs, 0.f, 255.f)),
		static_cast<uint8_t>(std::clamp(static_cast<float>(g) * rhs, 0.f, 255.f)),
		static_cast<uint8_t>(std::clamp(static_cast<float>(b) * rhs, 0.f, 255.f)),
		a
	);
}

Color& Color::operator*= (const float rhs)
{
	this->r = static_cast<uint8_t>(std::clamp(static_cast<float>(r) * rhs, 0.f, 255.f));
	this->g = static_cast<uint8_t>(std::clamp(static_cast<float>(g) * rhs, 0.f, 255.f));
	this->b = static_cast<uint8_t>(std::clamp(static_cast<float>(b) * rhs, 0.f, 255.f));
	return *this;
}

Color Color::operator/ (const float rhs) const
{
	return Color(
		static_cast<uint8_t>(std::clamp(static_cast<float>(r) / rhs, 0.f, 255.f)),
		static_cast<uint8_t>(std::clamp(static_cast<float>(g) / rhs, 0.f, 255.f)),
		static_cast<uint8_t>(std::clamp(static_cast<float>(b) / rhs, 0.f, 255.f)),
		a
	);
}

Color& Color::operator/= (const float rhs)
{
	this->r = static_cast<uint8_t>(std::clamp(static_cast<float>(r) / rhs, 0.f, 255.f));
	this->g = static_cast<uint8_t>(std::clamp(static_cast<float>(g) / rhs, 0.f, 255.f));
	this->b = static_cast<uint8_t>(std::clamp(static_cast<float>(b) / rhs, 0.f, 255.f));
	return *this;
}

Color Color::invert() const
{
	// This is equal to Color(255u - r, 255u - g, 255u - b, a)
	// ^ = XOR
	return Color(value ^ 0xFFFFFF00u);
}

Color Color::grayscale() const
{
	int v = uint8_t(static_cast<float>(r) * 0.3f + static_cast<float>(g) * 0.59f + static_cast<float>(b) * 0.11f);
	return Color(v, v, v, a);
}

Color Color::lerp(const Color& color, float amount) const
{
	return Color(
		static_cast<uint8_t>(static_cast<float>(r) + value * (static_cast<float>(color.r) - static_cast<float>(r)), 0.f, 255.f),
		static_cast<uint8_t>(static_cast<float>(g) + value * (static_cast<float>(color.g) - static_cast<float>(b)), 0.f, 255.f),
		static_cast<uint8_t>(static_cast<float>(b) + value * (static_cast<float>(color.b) - static_cast<float>(g)), 0.f, 255.f),
		a
	);
}

std::ostream& operator<< (std::ostream& os, const Color& color)
{
	os << "r: " << static_cast<int>(color.r) << '\t'
	   << "g: " << static_cast<int>(color.g) << '\t'
	   << "b: " << static_cast<int>(color.b) << '\t'
	   << "a: " << static_cast<int>(color.a);
	return os;
}

Color hsvToRgb(const ColorHsv& color)
{
	float hue = color.h * 360.f;
	/*if (hue > 360 || hue < 0 || color.s > 100 || color.s < 0 || color.v > 100 || color.v < 0) {
		return Color();
	}*/
	float C = color.s * color.v;
	float X = C * (1 - abs(fmod(hue / 60.0, 2) - 1));
	float m = color.v - C;
	float r, g, b;
	if (hue >= 0 && hue < 60)
		r = C, g = X, b = 0;
	else if (hue >= 60 && hue < 120)
		r = X, g = C, b = 0;
	else if (hue >= 120 && hue < 180) 
		r = 0, g = C, b = X;
	else if (hue >= 180 && hue < 240) 
		r = 0, g = X, b = C;
	else if (hue >= 240 && hue < 300) 
		r = X, g = 0, b = C;
	else
		r = C, g = 0, b = X;
	int R = (r + m) * 255;
	int G = (g + m) * 255;
	int B = (b + m) * 255;
	return Color(255 * (r + m), 255 * (g + m), 255 * (b + m));
}

ColorHsv rgbToHsv(const Color& color)
{
	float cR = color.r / 255.0f;
	float cG = color.g / 255.0f;
	float cB = color.b / 255.0f;

	float h, s, v; // h:0-360.0, s:0.0-1.0, v:0.0-1.0

	float min = std::fminf(cR, fminf(cG, cB));
	float max = std::fmaxf(cR, fmaxf(cG, cB));

	v = max;

	if (max == 0.0f)
	{
		s = 0;
		h = 0;
	}
	else if (max - min == 0.0f)
	{
		s = 0;
		h = 0;
	}
	else {
		s = (max - min) / max;

		if (max == cR)
			h = 60 * ((cG - cB) / (max - min)) + 0;
		else if (max == color.g)
			h = 60 * ((cB - cR) / (max - min)) + 120;
		else
			h = 60 * ((cR - cG) / (max - min)) + 240;
	}

	if (h < 0) h += 360.0f;

	return ColorHsv{ h / 360.0f, s, v };
}
