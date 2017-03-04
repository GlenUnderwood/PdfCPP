#pragma once
#include <memory>
#include <vector>
#ifdef __GNUC__
#include <math.h>
#include <cmath>
#endif
#include "ExportDef.h"

namespace Pdf {
namespace Geometry {

	// TODO turn these into structs. ?
class PDF_API pdfPoint
{
public:
	static const double PI;// = 3.14159L;

	pdfPoint();
	pdfPoint( float x, float y );
	pdfPoint( const pdfPoint& src );

	inline pdfPoint& operator+=( const pdfPoint& rhs );
	inline pdfPoint& operator-=( const pdfPoint& rhs );

	void Offset( float x, float y );
	void Rotate( const pdfPoint& ptOrigin, float fDegrees );
	inline void Rotate( float fDegrees );
//	inline void Transform( const pdfMatrix& m );

	inline double DotProduct( const pdfPoint& b );

	double VectorLength( const pdfPoint& b );
	double VectorLength( );

	double AngleTo( const pdfPoint& ptB );

	float X;
	float Y;

};

typedef std::vector<pdfPoint> PDF_API pdfLine;
/*
class pdfLine : std::vector
{
	protected pdfPoint[] m_pts;
}
*/

typedef std::vector<pdfLine> PDF_API pdfPolygon;
/*class pdfPolygon
{
	protected pdfPoint[] m_pts;
}
*/

class PDF_API pdfRect
{
public:
	pdfRect();
	pdfRect( float x, float y, float r, float b );
	pdfRect( const pdfRect& src );

	inline float Width() { return fabs( Right - Left ); };
	inline float Height() { return fabs( Top - Bottom );  };
	inline pdfPoint TopLeft() { return pdfPoint( Left, Top ); } ;
	inline pdfPoint TopRight() { return pdfPoint( Right, Top ); } ;
	inline pdfPoint BottomLeft() { return pdfPoint( Left, Bottom ); } ;
	inline pdfPoint BottomRight() { return pdfPoint( Right, Bottom ); } ;
	inline pdfPoint Center() { return pdfPoint( Left + Width()/2.0F, Top<Bottom ? (Top+Height()/2.0F) : (Top-Height()/2.0F) ); } ;

	void Offset( float x, float y );

	float Left;// { get; set; }
	float Top;// { get; set; }
	float Right;// { get; set; }
	float Bottom;// { get; set; }

};

class PDF_API pdfMatrix
{
public:
	pdfMatrix();
	pdfMatrix( const pdfMatrix& src );
	pdfMatrix( float a, float b, float c, float d, float e, float f );

/*	static implicit operator System.Drawing.Drawing2D.Matrix( pdfMatrix m )
	{
		return new System.Drawing.Drawing2D.Matrix( m.m_mat[ 0 ], m.m_mat[ 1 ], m.m_mat[ 2 ], m.m_mat[ 3 ], m.m_mat[ 4 ], m.m_mat[ 5 ] );
	}
	static implicit operator pdfMatrix( System.Drawing.Drawing2D.Matrix m )
	{
		float[] f = m.Elements;
		return new pdfMatrix( f[ 0 ], f[ 1 ], f[ 2 ], f[ 3 ], f[ 4 ], f[ 5 ] );
	}*/

	inline bool IsIdentity() const;
	float GetRotation();
	void SetRotation(float value);

	inline bool operator==( const pdfMatrix& rhs ) {
		return memcmp( &m_mat, &rhs.m_mat, sizeof( m_mat ) ) == 0;
	};
	inline bool operator!=( const pdfMatrix& rhs ) {
		return !operator==(rhs);
	};
	inline float operator[]( const int& idx );

	void Set( const int& idx, float a );

	void Set( float a, float b, float c, float d, float e, float f );

	void SetToIdentity();

	void Rotate( float fDegrees );
	void Scale( float a );
	void Scale( float a, float b );
	void Translate( float a, float b );

	void TransformPoint( pdfPoint& pt );
	/*void TransformPoint( ref System.Drawing.PointF pt )
	{
		pt.X = ( pt.X * m_mat[ 0 ] + pt.Y * m_mat[ 1 ] ) + m_mat[ 4 ];
		pt.Y = ( pt.X * m_mat[ 2 ] + pt.Y * m_mat[ 3 ] ) + m_mat[ 5 ];
	}
	void TransformPoints( System.Drawing.PointF[] pts )
	{
		for ( int i = 0; i < pts.Length; i++ )
		{
			TransformPoint( ref pts[ i ] );
		}
	}*/
	void Multiply( float a, float b, float c, float d, float e, float f/*, System.Drawing.Drawing2D.MatrixOrder order*/ );
	void Multiply( const pdfMatrix& rhs/*, System.Drawing.Drawing2D.MatrixOrder order*/ );


	float ScaleX;
	float ScaleY;
	float m_mat[6];


	//FT_Matrix_Multiply( const FT_Matrix*  a,
//                      FT_Matrix        *b )
//  {
//    FT_Fixed  xx, xy, yx, yy;


//    if ( !a || !b )
//      return;

//    xx = FT_MulFix( a->xx, b->xx ) + FT_MulFix( a->xy, b->yx );
//    xy = FT_MulFix( a->xx, b->xy ) + FT_MulFix( a->xy, b->yy );
//    yx = FT_MulFix( a->yx, b->xx ) + FT_MulFix( a->yy, b->yx );
//    yy = FT_MulFix( a->yx, b->xy ) + FT_MulFix( a->yy, b->yy );

//    b->xx = xx;  b->xy = xy;
//    b->yx = yx;  b->yy = yy;
//  }


//  /* documentation is in ftglyph.h */

//  FT_EXPORT_DEF( FT_Error )
//  FT_Matrix_Invert( FT_Matrix*  matrix )
//  {
//    FT_Pos  delta, xx, yy;


//    if ( !matrix )
//      return FT_Err_Invalid_Argument;

//    /* compute discriminant */
//    delta = FT_MulFix( matrix->xx, matrix->yy ) -
//            FT_MulFix( matrix->xy, matrix->yx );

//    if ( !delta )
//      return FT_Err_Invalid_Argument;  /* matrix can't be inverted */

//    matrix->xy = - FT_DivFix( matrix->xy, delta );
//    matrix->yx = - FT_DivFix( matrix->yx, delta );

//    xx = matrix->xx;
//    yy = matrix->yy;

//    matrix->xx = FT_DivFix( yy, delta );
//    matrix->yy = FT_DivFix( xx, delta );

//    return FT_Err_Ok;
//  }
private:
	double Round( double d, int iDecimals = 8 );
	inline float CalcScaleX();
	inline float CalcScaleY();
	/*override string ToString()
	{
		return Utils.MatrixToString( m_mat, 4 );
	}*/
};


};// namespace Geometry
};// namespace Pdf

// for chaining one or more ops together.
PDF_API Pdf::Geometry::pdfPoint operator+( Pdf::Geometry::pdfPoint lhs, Pdf::Geometry::pdfPoint rhs );
PDF_API Pdf::Geometry::pdfPoint operator-( Pdf::Geometry::pdfPoint lhs, Pdf::Geometry::pdfPoint rhs );
PDF_API std::ostream& operator<<( std::ostream& stm, const Pdf::Geometry::pdfPoint& id );

// global operators for pdfMatrix.
// Persistence
PDF_API std::ostream& operator<<( std::ostream& stm, const Pdf::Geometry::pdfMatrix& id );
// for chaining one or more ops together.
PDF_API Pdf::Geometry::pdfMatrix operator*( Pdf::Geometry::pdfMatrix& lhs, Pdf::Geometry::pdfMatrix& rhs );

