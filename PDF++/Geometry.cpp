#include "stdafx.h"
#include "Geometry.h"


namespace Pdf {
namespace Geometry {

/////////////////////////////////////////
const double pdfPoint::PI = 3.14159L;

pdfPoint::pdfPoint()
{
	X = 0;
	Y = 0;
}
pdfPoint::pdfPoint( float x, float y )
{
	X = x;
	Y = y;
}
pdfPoint::pdfPoint( const pdfPoint& src )
{
	X = src.X;
	Y = src.Y;
}

pdfPoint& pdfPoint::operator+=( const pdfPoint& rhs )
{ X += rhs.X; Y += rhs.Y; return *this; }
pdfPoint& pdfPoint::operator-=( const pdfPoint& rhs )
{ X -= rhs.X; Y -= rhs.Y; return *this; }


void pdfPoint::Offset( float x, float y )
{
	X += x;
	Y += y;
}
void pdfPoint::Rotate( const pdfPoint& ptOrigin, float fDegrees )
{
	double dRadians = fDegrees * ( PI / 180.0 );
	// rotation matrix  (z-axis, specifically)  [cos, sin, -sin, cos, 0, 0]
	double dSin = sin( dRadians ), dCos = cos( dRadians );

	X -= ptOrigin.X;
	Y -= ptOrigin.Y;

	double x = Y;
	double y = Y;
	X = (float)( x * dCos + y * dSin );
	Y = (float)( x * -dSin + y * dCos );

	X += ptOrigin.X;
	Y += ptOrigin.Y;
}
void pdfPoint::Rotate( float fDegrees )
{
	Rotate( pdfPoint( 0, 0 ), fDegrees );
}
//void pdfPoint::Transform( const pdfMatrix& m )
//{
//	m.TransformPoint( *this );
//}

double pdfPoint::DotProduct( const pdfPoint& b )
{ return X * b.X + Y * b.Y /*+ z*z*/; }

double pdfPoint::VectorLength( const pdfPoint& b )
{
	double dx = X - b.X;
	double dy = Y - b.Y;
	//double dz = z - b.z;

	return sqrt( ( dx * dx ) + ( dy * dy ) /*+ (dz*dz)*/ );
}
double pdfPoint::VectorLength( )
{
	double dx =  -X;
	double dy =  -Y;

	return sqrt( ( dx * dx ) + ( dy * dy ));
}

double pdfPoint::AngleTo( const pdfPoint& ptB )
{
	pdfPoint ptOrigin( X, Y ),// ( pptOrigin ? *pptOrigin : *this ),
		// I use this so that I can basically create a triangle with the points - it is always just to the right of the origin
			ptA( 1.234567F, 0 );

	//cos-1( Dot(AB) / (Len(A)*Len(B) )			= angle up to 180
	//cos-1( (Ax*Bx + Ay*By) / Len(A)*Len(B) )	= angle up to 180
	double len = ptOrigin.VectorLength( ptA ) * ptOrigin.VectorLength( ptB ),
			dDotProd = ptA.DotProduct( ptB );

	// gets us a measurement up to 180degs
	if ( len == 0 )
		len = acos( dDotProd ); // just reusing len, now it represents the angle in radians
	else
		len = acos( dDotProd / len );

	//  if the ptB.y val is negative (below A) then the angle is > 180
	if ( ptB.Y < ptA.Y )
		len = ( PI * 2.0 ) - len;	// gives 0-359 degs

	// convert to degrees
	double dAngle = ( len / ( PI / 180.0 ) );
	while ( dAngle >= 360 )
		dAngle -= 360;
	return dAngle;
}

/////////////////////////////////////////////////
pdfRect::pdfRect( )
{
	Left = 	Top = Right = 	Bottom = 0;
}
pdfRect::pdfRect( float x, float y, float r, float b )
{
	Left = x;
	Top = y;
	Right = r;
	Bottom = b;
}
pdfRect::pdfRect( const pdfRect& src )
{
	Left = src.Left;
	Top = src.Top;
	Right = src.Right;
	Bottom = src.Bottom;
}

void pdfRect::Offset( float x, float y )
{
	Left += x;
	Top += y;
	Right += x;
	Bottom += y;
}

	
/////////////////////////////////////////////////
pdfMatrix::pdfMatrix()
{
	SetToIdentity();
}
pdfMatrix::pdfMatrix( const pdfMatrix& src )
{
	memcpy( m_mat, src.m_mat, sizeof(m_mat) );
	ScaleX = src.ScaleX;
	ScaleY = src.ScaleY;
}
pdfMatrix::pdfMatrix( float a, float b, float c, float d, float e, float f )
{
	Set( a, b, c, d, e, f );
}

bool pdfMatrix::IsIdentity() const
{
	return ( m_mat[ 0 ] == 1 && m_mat[ 1 ] == 0 && m_mat[ 2 ] == 0 && m_mat[ 3 ] == 1 && m_mat[ 4 ] == 0 && m_mat[ 5 ] == 0 );
}

float pdfMatrix::GetRotation()
{
	pdfPoint p0( 0, 0 );
	pdfPoint p1( m_mat[ 0 ], m_mat[ 1 ] );
	return (float)Round(p0.AngleTo(p1));
	//return (float)Round(Utils.AngleTo( new System.Drawing.PointF( 0, 0 ), new System.Drawing.PointF( m_mat[ 0 ], m_mat[ 1 ] ) ), 4);
}
void pdfMatrix::SetRotation(float value)
{
	pdfPoint p0( 0, 0 );
	pdfPoint p1( m_mat[ 0 ], m_mat[ 1 ] );
	float d = value - (float)Round( p1.AngleTo( p1 ) );
	Rotate( d );
}

float pdfMatrix::operator[]( const int& idx )
{
	return m_mat[ idx ];
}
void pdfMatrix::Set( const int& idx, float a )
{
	//float dRot = Utils.AngleTo( new System.Drawing.PointF( 0, 0 ), new System.Drawing.PointF( m_mat[ 0 ], m_mat[ 1 ] ) );
	m_mat[ idx ] = a;
	if ( idx == 0 || idx == 1 )
		ScaleX = CalcScaleX();//(float)Round( sqrt( ( m_mat[ 0 ] * m_mat[ 0 ] ) + ( m_mat[ 1 ] * m_mat[ 1 ] ) ), 4 );
	else if ( idx == 2 || idx == 3 )
		ScaleY = CalcScaleY();//(float)Round(sqrt( ( m_mat[ 2 ] * m_mat[ 2 ] ) + ( m_mat[ 3 ] * m_mat[ 3 ] ) ), 4 );
}

void pdfMatrix::Set( float a, float b, float c, float d, float e, float f )
{
	m_mat[ 0 ] = a;
	m_mat[ 1 ] = b;
	m_mat[ 2 ] = c;
	m_mat[ 3 ] = d;
	m_mat[ 4 ] = e;
	m_mat[ 5 ] = f;
	ScaleX = CalcScaleX();//(float)Round(sqrt( ( a * a ) + ( b * b ) ), 4);
	ScaleY = CalcScaleY();//(float)Round(sqrt( ( c * c ) + ( d * d ) ), 4);
}
void pdfMatrix::SetToIdentity()
{
	ScaleX = ScaleY = 1;
	m_mat[ 0 ] = 1; m_mat[ 1 ] = 0;
	m_mat[ 2 ] = 0; m_mat[ 3 ] = 1;
	m_mat[ 4 ] = 0; m_mat[ 5 ] = 0;
}

void pdfMatrix::Rotate( float fDegrees )
{
	//System.Drawing.Drawing2D.Matrix m = new System.Drawing.Drawing2D.Matrix( m_mat[ 0 ], m_mat[ 1 ], m_mat[ 2 ], m_mat[ 3 ], m_mat[ 4 ], m_mat[ 5 ] );
	//m.Rotate( fDegrees );
	//Set( m.Elements[ 0 ], m.Elements[ 1 ], m.Elements[ 2 ], m.Elements[ 3 ], m.Elements[ 4 ], m.Elements[ 5 ] );

	double dRadians = fDegrees * ( pdfPoint::PI / 180.0 );
	// rotation matrix  (z-axis, specifically)  [cos, sin, -sin, cos, 0, 0]
	float dSin = (float)sin( dRadians ), dCos = (float)cos( dRadians );
	Multiply( dCos, dSin, -dSin, dCos, 0, 0/*, System.Drawing.Drawing2D.MatrixOrder.Append*/ );
}
void pdfMatrix::Scale( float a )
{
	//Multiply( a, 0, 0, a, 0, 0, System.Drawing.Drawing2D.MatrixOrder.Append );
}
void pdfMatrix::Scale( float a, float b )
{
	//Multiply( a, 0, 0, b, 0, 0, System.Drawing.Drawing2D.MatrixOrder.Append );
}
void pdfMatrix::Translate( float a, float b )
{
	m_mat[ 4 ] += a;
	m_mat[ 5 ] += b;
}

void pdfMatrix::TransformPoint( pdfPoint& pt )
{
	pt.X = ( pt.X * m_mat[ 0 ] + pt.Y * m_mat[ 1 ] ) + m_mat[ 4 ];
	pt.Y = ( pt.X * m_mat[ 2 ] + pt.Y * m_mat[ 3 ] ) + m_mat[ 5 ];
}
/*void pdfMatrix::TransformPoint( ref System.Drawing.PointF pt )
{
	pt.X = ( pt.X * m_mat[ 0 ] + pt.Y * m_mat[ 1 ] ) + m_mat[ 4 ];
	pt.Y = ( pt.X * m_mat[ 2 ] + pt.Y * m_mat[ 3 ] ) + m_mat[ 5 ];
}
void pdfMatrix::TransformPoints( System.Drawing.PointF[] pts )
{
	for ( int i = 0; i < pts.Length; i++ )
	{
		TransformPoint( ref pts[ i ] );
	}
}*/
void pdfMatrix::Multiply( float a, float b, float c, float d, float e, float f/*, System.Drawing.Drawing2D.MatrixOrder order*/ )
{
	/*System.Drawing.Drawing2D.Matrix m = new System.Drawing.Drawing2D.Matrix( m_mat[ 0 ], m_mat[ 1 ], m_mat[ 2 ], m_mat[ 3 ], m_mat[ 4 ], m_mat[ 5 ] );
	m.Multiply( new System.Drawing.Drawing2D.Matrix( a, b, c, d, e, f), order );
	this[ 0 ] = m.Elements[ 0 ];
	this[ 1 ] = m.Elements[ 1 ];
	this[ 2 ] = m.Elements[ 2 ];
	this[ 3 ] = m.Elements[ 3 ];
	this[ 4 ] = m.Elements[ 4 ];
	this[ 5 ] = m.Elements[ 5 ];*/
}
void pdfMatrix::Multiply( const pdfMatrix& rhs/*, System.Drawing.Drawing2D.MatrixOrder order*/ )
{
	/*System.Drawing.Drawing2D.Matrix m = new System.Drawing.Drawing2D.Matrix( m_mat[ 0 ], m_mat[ 1 ], m_mat[ 2 ], m_mat[ 3 ], m_mat[ 4 ], m_mat[ 5 ] );
	m.Multiply( rhs, order );
	this[ 0 ] = m.Elements[ 0 ];
	this[ 1 ] = m.Elements[ 1 ];
	this[ 2 ] = m.Elements[ 2 ];
	this[ 3 ] = m.Elements[ 3 ];
	this[ 4 ] = m.Elements[ 4 ];
	this[ 5 ] = m.Elements[ 5 ];*/
}


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

double pdfMatrix::Round( double d, int iDecimals )
{
	if( iDecimals > 0 )
		d *= (10*iDecimals);
	int r = (d > 0.0) ? (d + 0.5) : (d - 0.5);// d = float( d + 0.5 );
	d = (double)r;
	if( iDecimals > 0 )
		d /= (10*iDecimals);
	return d;
}
float pdfMatrix::CalcScaleX()
{
	return (float)Round(sqrt( ( m_mat[ 0 ] * m_mat[ 0 ] ) + ( m_mat[ 1 ] * m_mat[ 1 ] ) ), 4 );
}
float pdfMatrix::CalcScaleY()
{
	return (float)Round( sqrt( ( m_mat[ 2 ] * m_mat[ 2 ] ) + ( m_mat[ 3 ] * m_mat[ 3 ] ) ), 4 );
}
/*override string ToString()
{
	return Utils.MatrixToString( m_mat, 4 );
}*/

};// namespace Geometry
};// namespace Pdf


//////////////////////////////////////////////////////////////////////////////////////////////////////////////

Pdf::Geometry::pdfPoint operator+( Pdf::Geometry::pdfPoint lhs, Pdf::Geometry::pdfPoint rhs )
{ return Pdf::Geometry::pdfPoint( lhs.X + rhs.X, lhs.Y + rhs.Y ); }

Pdf::Geometry::pdfPoint operator-( Pdf::Geometry::pdfPoint lhs, Pdf::Geometry::pdfPoint rhs )
{ return Pdf::Geometry::pdfPoint( lhs.X - rhs.X, lhs.Y - rhs.Y ); }

// Persistence
std::ostream& operator<<( std::ostream& stm, const Pdf::Geometry::pdfPoint& id )
{
	stm << '(' << id.X << ',' << id.Y << ')';
	return stm;
}
// global operators for pdfMatrix.
// Persistence
std::ostream& operator<<( std::ostream& stm, const Pdf::Geometry::pdfMatrix& id )
{
	stm << '{' << id.m_mat[0] << ','  << id.m_mat[1] << ','  << id.m_mat[2] << ',' << id.m_mat[3] << ','  << id.m_mat[4] << ','  << id.m_mat[5] << ','  << '}';
	return stm;
}
// chaining one or more ops together.
Pdf::Geometry::pdfMatrix operator*( Pdf::Geometry::pdfMatrix& lhs, Pdf::Geometry::pdfMatrix& rhs )
{
	//System.Drawing.Drawing2D.Matrix m = new System.Drawing.Drawing2D.Matrix( lhs[ 0 ], lhs[ 1 ], lhs[ 2 ], lhs[ 3 ], lhs[ 4 ], lhs[ 5 ] );
	//m.Multiply( rhs, System.Drawing.Drawing2D.MatrixOrder.Append );
	return Pdf::Geometry::pdfMatrix();//m;
}
