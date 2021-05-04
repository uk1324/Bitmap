#include "BmpHeaders.h"
#include <iostream>

void logBitmapHeaders(bmpFileHeader& f, bmpInfoHeader& i)
{
	std::cout << std::hex
			  << "type:    " << '\t' << f.type << std::dec
			  << std::endl << "size:    " << '\t' << f.size
			  << std::endl << "reserved: " << '\t' << f.reserved
			  << std::endl << "offset: " << '\t' << f.offset
			  << std::endl
			  << std::endl << "size:    " << '\t' << i.size
			  << std::endl << "width:   " << '\t' << i.width
			  << std::endl << "height: " << '\t' << i.height
			  << std::endl << "planes: " << '\t' << i.planes
			  << std::endl << "bitCount: " << '\t' << i.bitCount
			  << std::endl << "compression: " << '\t' << static_cast<int>(i.compression)
			  << std::endl << "sizeImage: " << '\t' << i.sizeImage
			  << std::endl << "pxPerMeterH: " << '\t' << i.pxPerMeterH
			  << std::endl << "pxPerMeterV: " << '\t' << i.pxPerMeterV
			  << std::endl << "palette: " << '\t' << i.colPalette
			  << std::endl << "colImportant: " << '\t' << i.colImportant << std::hex
			  << std::endl << "redBitmask: " << '\t' << i.redBitmask
			  << std::endl << "greenBitmask: " << '\t' << i.greenBitmask
			  << std::endl << "blueBitmask: " << '\t' << i.blueBitmask
			  << std::endl << "alphaBitmask: " << '\t' << i.alphaBitmask
			  << std::endl << "colorSpace: " << '\t' << i.colorSpace << std::endl << std::dec;
}