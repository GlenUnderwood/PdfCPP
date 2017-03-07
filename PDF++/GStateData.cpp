#include "stdafx.h"
#include "DocData.h"
#include "AtomData.h"
#include "GStateData.h"
#include "Document.h"

namespace Pdf{

/////////////////////////
CGStateData::CGStateData(void) : 
	m_RenderingIntent( "RelativeColorimetric" ),
	m_StrokeColor( 1.0F, 0 ), m_FillColor( 1.0F, 0 ),
	m_LineWidth( 1.0F ), m_MiterLimit( 10.0F ),
	m_Flags( GStateFlags::NoGState )

{
}

CGStateData::CGStateData(const CGStateData& src) : 
	m_RenderingIntent( "RelativeColorimetric" ),
	m_StrokeColor( 1.0F, 0 ), m_FillColor( 1.0F, 0 ),
	m_LineWidth( 1.0F ), m_MiterLimit( 10.0F ),
	m_Flags( GStateFlags::NoGState )
{
	operator=( src );
}

CGStateData::~CGStateData(void)
{
}

CGStateData& CGStateData::operator=(const CGStateData& src)
{

	//Mat = src.Mat != nullptr ? src.Mat.Clone() : new Matrix(1, 0, 0, 1, 0, 0);
	m_Mat = src.m_Mat;
	m_StrokeColor = src.m_StrokeColor;
	m_FillColor = src.m_FillColor;
	m_LineWidth = src.m_LineWidth;
	m_MiterLimit = src.m_MiterLimit;
	m_FlatnessTolerance = src.m_FlatnessTolerance;
	m_LineJoin = src.m_LineJoin;
	m_LineCap = src.m_LineCap;
	m_pathClip = src.m_pathClip;
	m_RenderingIntent = src.m_RenderingIntent;
	m_Dash = src.m_Dash;
	m_ExtGState = src.m_ExtGState ;

	m_Flags = src.m_Flags;

	return *this;
}

};//namespace Pdf
