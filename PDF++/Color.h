#pragma once

#include "ExportDef.h"

namespace Pdf {

typedef enum _ColorSpaceType
{
	UnknownColorSpace = -1,

	DeviceGray = 0,
	DeviceRGB,
	DeviceCMYK,

	Indexed,
	DeviceN,
	Pattern,
	Separation
} PDF_API ColorSpaceType;

class PDF_API pdfColor
{
public:
	pdfColor(void);
	pdfColor( const pdfColor& src );
	pdfColor( unsigned long c, bool bGray );
	pdfColor( byte a, byte r, byte g, byte b );
	pdfColor( byte a, byte c, byte m, byte y, byte k );
	pdfColor( byte a, byte g );
	pdfColor( float a, float r, float g, float b );
	pdfColor( float a, float c, float m, float y, float k );
	pdfColor( float a, float g );
	virtual ~pdfColor(void);

	pdfColor& operator=(const pdfColor& src);

	static float ToGrayScale( unsigned long c );
	static float ToGrayScale( float r, float g, float b );

	inline ColorSpaceType GetColorSpaceType() const throw()
		{ return m_Type; };
	void SetColorSpaceType(const ColorSpaceType& value );

	unsigned long GetRGB();

	unsigned long GetARGB();

	float A;
	float R, G, B;

	float C, M, Y, K;

private:
	ColorSpaceType m_Type;
};

};