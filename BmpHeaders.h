#pragma once
#include <cstdint>

enum class bmpCompression : uint32_t {
	BI_RGB = 0,
	BI_BITFIELDS = 3
};

#pragma pack(push, 2)
struct bmpFileHeader
{
	uint16_t type;			// File signature
	uint32_t size;			// Size of file
	uint32_t reserved;		// Application specific
	uint32_t offset;		// Offset in bytes where the bitmap data is
};
#pragma pack(pop)

struct bmpInfoHeader {
	uint32_t size;			// Number of bytes in the header from this point
	uint32_t width;			// Width of the image in pixels
	uint32_t height;		// Height of the image in pixels
	uint16_t planes;		// Number of color planes
	uint16_t bitCount;		// Number of bits per pixel
	bmpCompression compression;	// Type of compression
	uint32_t sizeImage;		// Size of the raw bitmap data in bytes including padding
	uint32_t pxPerMeterH;	// Pixels per meter horizontal
	uint32_t pxPerMeterV;	// Pixels per meter vertical
	uint32_t colPalette;	// Number of colors in the palette 
	uint32_t colImportant;  // Important colors
	uint32_t redBitmask;	// Red channel bitmask
	uint32_t greenBitmask;	// Green channel bitmask 
	uint32_t blueBitmask;	// Blue channel bitmask
	uint32_t alphaBitmask;	// Alpha channel bitmask
	uint32_t colorSpace;	// Color space
	uint32_t unused[12];	// If used the first 36 bytes represent color space endpoints the rest allow for gamma correction or rgb values.
};