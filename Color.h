#pragma once
#include <cstdint>
#include <algorithm>
#include <iostream>

class Color
{
public:
	union
	{
		uint32_t value;
		struct
		{
			uint8_t a;
			uint8_t b;
			uint8_t g;
			uint8_t r;
		};
	};

	Color();
	explicit Color(uint32_t val);
	Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255);

	const static Color Red, Green, Blue, Black, White, Cyan, Magenta, Yellow;

	static Color clamp(int red, int green, int blue, int alpha = 255);
	static Color fromFloat(float red, float green, float blue, float alpha = 0);

	bool   operator== (const Color& rhs) const;
	bool   operator!= (const Color& rhs) const;
	Color  operator+  (const Color& rhs) const;
	Color& operator+= (const Color& rhs);
	Color  operator-  (const Color& rhs) const;
	Color& operator-= (const Color& rhs);
	Color  operator*  (const float rhs)  const;
	Color& operator*= (const float rhs);
	Color  operator/  (const float rhs)  const;
	Color& operator/= (const float rhs);
	Color invert() const;
	Color grayscale() const;
	Color lerp(const Color& color, float amount) const;
};

struct ColorHsv
{
	float h;
	float s;
	float v;
};

Color hsvToRgb(const ColorHsv& color);
ColorHsv rgbToHsv(const Color& color);