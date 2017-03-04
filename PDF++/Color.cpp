#include "stdafx.h"
#include "Color.h"

namespace Pdf {

pdfColor::pdfColor( void)
{
	A = 1.0F;
	R = G = B = 0;
	C = M = Y = 0; K = 1;
	m_Type = ColorSpaceType::DeviceGray;
}

pdfColor::pdfColor( const pdfColor& src )
{
	operator=(src);
}

pdfColor::pdfColor( unsigned long c, bool bGray )
{
	// TODO byte order Might be different on a different processor
	byte	cA = (c & 0xFF000000) >> 24, 
			cB = (c & 0x00FF0000) >> 16,
			cG = (c & 0x0000FF00) >> 8,
			cR = (c & 0x000000FF);
	//return ToGrayScale( (float)(c & 0x0000FF00), (c & 0x00FF0000), (c & 0xFF000000) );

	//A = 1.0F;
	R = G = B = 0;
	C = M = Y = 0; K = 1;
	A = cA;
	if ( bGray )
	{
		m_Type = ColorSpaceType::DeviceGray;
		G = ToGrayScale( c ) / 255.0F;//((c.R * 0.299f) + (c.G * 0.587f) + (c.B * 0.144f));
		//R = G;
		//B = R;
	}
	else
	{
		m_Type = ColorSpaceType::DeviceRGB;
		R = cR / 255.0F;
		G = cG / 255.0F;
		B = cB / 255.0F;
	}
}
pdfColor::pdfColor( byte a, byte r, byte g, byte b )
{
	C = M = Y = 0; K = 1;
	A = (float)a / 255.0F;
	R = (float)r / 255.0F;
	G = (float)g / 255.0F;
	B = (float)b / 255.0F;
	m_Type = ColorSpaceType::DeviceRGB;
}
pdfColor::pdfColor( byte a, byte c, byte m, byte y, byte k )
{
	R = G = B = 0;
	A = (float)a / 255.0F;
	C = (float)c / 255.0F;
	M = (float)m / 255.0F;
	Y = (float)y / 255.0F;
	K = (float)k / 255.0F;
	m_Type = ColorSpaceType::DeviceCMYK;
}
pdfColor::pdfColor( byte a, byte g )
{
	A = (float)a / 255.0F;
	R = B = 0;
	C = M = Y = K = 0;
	G = (float)g / 255.0F;
	m_Type = ColorSpaceType::DeviceGray;
}
pdfColor::pdfColor( float a, float r, float g, float b )
{
	C = M = Y = K = 0;
	A = a;
	R = r;
	G = g;
	B = b;
	m_Type = ColorSpaceType::DeviceRGB;
}
pdfColor::pdfColor( float a, float c, float m, float y, float k )
{
	A = a;
	R = G = B = 0;
	C = c;
	M = m;
	Y = y;
	K = k;
	m_Type = ColorSpaceType::DeviceCMYK;
}
pdfColor::pdfColor( float a, float g )
{
	A = a;
	R = B = 0;
	C = M = Y = K = 0;
	G = g;
	m_Type = ColorSpaceType::DeviceGray;
}

pdfColor::~pdfColor(void)
{
}

pdfColor& pdfColor::operator=(const pdfColor& src)
{
	if( this != &src )
	{
		A = src.A;
		R = src.R;
		G = src.G;
		B = src.B;
		C = src.C;
		M = src.M;
		Y = src.Y;
		K = src.K;
		m_Type = src.m_Type;
	}
	return *this;
}

/*static*/ float pdfColor::ToGrayScale( unsigned long c )
{
	// TODO byte order Might be different on a different processor
	byte	cA = (c & 0xFF000000) >> 24, 
			cB = (c & 0x00FF0000) >> 16,
			cG = (c & 0x0000FF00) >> 8,
			cR = (c & 0x000000FF);
	return ToGrayScale( float(cR), float(cG), float(cB) );
}

/*static*/ float pdfColor::ToGrayScale( float r, float g, float b )
{
	return ( ( r * 0.299f ) + ( g * 0.587f ) + ( b * 0.144f ) );
}

void pdfColor::SetColorSpaceType(const ColorSpaceType& value )
{
	if ( value != m_Type )
	{
		switch ( value )
		{
			case ColorSpaceType::DeviceGray:
				if ( m_Type == ColorSpaceType::DeviceCMYK )
				{
					float k = 1.0F - K;
					R = ( k * ( 1.0F - C ) );
					G = ( k * ( 1.0F - M ) );
					B = ( k * ( 1.0F - Y ) );
					C = Y = M = 0;
					K = 1;


					//G = (R + G + B) / 3.0F;// THERE's a BETER FORMULA
					// 0.299f (red), 0.587f (green) and 0.144f (blue). 
					G = ToGrayScale( R, G, B );//((R * 0.299f) + (G * 0.587f) + (B * 0.144f));
					R = 0;
					B = 0;
				}
				else if ( m_Type == ColorSpaceType::DeviceRGB )
				{
					G = ToGrayScale( R, G, B );//((R * 0.299f) + (G * 0.587f) + (B * 0.144f));
					R = 0;
					B = 0;
				}
				m_Type = value;
				break;
			case ColorSpaceType::DeviceRGB:
				if ( m_Type == ColorSpaceType::DeviceCMYK )
				{
					float k = 1.0F - K;
					R = ( k * ( 1.0F - C ) );
					G = ( k * ( 1.0F - M ) );
					B = ( k * ( 1.0F - Y ) );
					C = Y = M = 0;
					K = 1;
				}
				else if ( m_Type == ColorSpaceType::DeviceGray )
				{
					R = G;
					B = G;
				}
				m_Type = value;
				break;

			case ColorSpaceType::DeviceCMYK:
				if ( m_Type == ColorSpaceType::DeviceRGB )
				{
					if ( R == 0 && B == 0 && G == 0 )
					{
						C = Y = M = 0;
						K = 1;
					}
					else
					{
						C = 1.0F - R;
						M = 1.0F - G;
						Y = 1.0F - B;

						float minCMY = Math::Min( C, Math::Min( M, Y ) );

						C = ( C - minCMY ) / ( 1 - minCMY );
						M = ( M - minCMY ) / ( 1 - minCMY );
						Y = ( Y - minCMY ) / ( 1 - minCMY );
						K = minCMY;
					}
				}
				else if ( m_Type == ColorSpaceType::DeviceGray )
				{
					C = M = Y = 0;
					K = 1.0F - G;
				}
				R = G = B = 0;
				m_Type = value;
				break;

			default:
				throw pdfUnhandledColorSpaceEx();
		}
	}
}

unsigned long pdfColor::GetRGB() const
{
	//ushort 2;
	// small local lambda func to pack into one unsigned long
	auto FromRgb = [&] ( int r, int g, int b ) { return ( (((unsigned long)(byte)b) << 16) ) | (((unsigned short)(byte)g) << 8) | (((byte)r)  ); };

	switch ( m_Type )
	{
		case ColorSpaceType::DeviceGray:
			return FromRgb( (int)( G * 255.0F ), (int)( G * 255.0F ), (int)( G * 255.0F ) );
		case ColorSpaceType::DeviceRGB:
			return FromRgb( (int)( R * 255.0F ), (int)( G * 255.0F ), (int)( B * 255.0F ) );
		case ColorSpaceType::DeviceCMYK:
			{
				//float k = 255.0f - (K * 255.0F);
				//int r = (int)((k * (255.0F - C)) / 255.0F);
				//int g = (int)((k * (255.0F - M)) / 255.0F);
				//int b = (int)((k * (255.0F - Y)) / 255.0F);

				float k = 1.0F - K;
				int r = (int)( ( k * ( 1.0F - C ) ) * 255.0F );
				int g = (int)( ( k * ( 1.0F - M ) ) * 255.0F );
				int b = (int)( ( k * ( 1.0F - Y ) ) * 255.0F );

				//int r = (int)((k * (255.0F - C)) / 255.0F);
				//int g = (int)((k * (255.0F - M)) / 255.0F);
				//int b = (int)((k * (255.0F - Y)) / 255.0F);
				//int r = (int)((1.0F - (K + C)) * 255.0F);
				//int g = (int)((1.0F - (K + M)) * 255.0F);
				//int b = (int)((1.0F - (K + Y)) * 255.0F);

				//Debug.Print("C={0:0.##} M={1:0.##} Y={2:0.##} K={3:0.##}", C, M, Y, K);

				//if (r < 0)
				//	r = 0;
#ifdef _MSC_VER
				_ASSERT( r >= 0 && r <= 255 && g >= 0 && g <= 255 && b >= 0 && b <= 255 );
#endif
				return FromRgb( r, g, b );
			}
	}
	throw pdfUnhandledColorSpaceEx();
}
		
unsigned long pdfColor::GetARGB() const
{
	// small local lambda func to pack into one unsigned long
	auto FromArgb = [&] ( int a, int r, int g, int b ) { return (unsigned long)( (((byte)a) << 24) | (((byte)b) << 16) ) | (((byte)g) << 8) | (((byte)r)  ); };
/*
	byte	cA = (c & 0xFF000000) >> 24, 
			cB = (c & 0x00FF0000) >> 16,
			cG = (c & 0x0000FF00) >> 8,
			cR = (c & 0x000000FF);
*/
	switch ( m_Type )
	{
		case ColorSpaceType::DeviceGray:
			return FromArgb( (int)( A * 255.0F ), (int)( G * 255.0F ), (int)( G * 255.0F ), (int)( G * 255.0F ) );
		case ColorSpaceType::DeviceRGB:
			return FromArgb( (int)( A * 255.0F ), (int)( R * 255.0F ), (int)( G * 255.0F ), (int)( B * 255.0F ) );
		case ColorSpaceType::DeviceCMYK:
			{
				//float k = 255.0f - (K * 255.0F);
				//int r = (int)((k * (255.0F - C)) / 255.0F);
				//int g = (int)((k * (255.0F - M)) / 255.0F);
				//int b = (int)((k * (255.0F - Y)) / 255.0F);

				float k = 1.0F - K;
				int r = (int)( ( k * ( 1.0F - C ) ) * 255.0F );
				int g = (int)( ( k * ( 1.0F - M ) ) * 255.0F );
				int b = (int)( ( k * ( 1.0F - Y ) ) * 255.0F );

				//int r = (int)((k * (255.0F - C)) / 255.0F);
				//int g = (int)((k * (255.0F - M)) / 255.0F);
				//int b = (int)((k * (255.0F - Y)) / 255.0F);
				//int r = (int)((1.0F - (K + C)) * 255.0F);
				//int g = (int)((1.0F - (K + M)) * 255.0F);
				//int b = (int)((1.0F - (K + Y)) * 255.0F);

				//Debug.Print("C={0:0.##} M={1:0.##} Y={2:0.##} K={3:0.##}", C, M, Y, K);

				//if (r < 0)
				//	r = 0;
#ifdef _MSC_VER
				_ASSERT( r >= 0 && r <= 255 && g >= 0 && g <= 255 && b >= 0 && b <= 255 );
#endif

				return FromArgb( (int)( A * 255.0F ), r, g, b );
			}
	}
	throw pdfUnhandledColorSpaceEx();
}

};
