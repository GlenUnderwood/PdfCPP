#include "stdafx.h"
#include "DocData.h"
#include "AtomData.h"
#include "GStateData.h"
#include "Document.h"

namespace Pdf{

CDashPattern::CDashPattern( const CDashPattern& rhs ) : m_Phase( 0 )
{
	operator=( rhs );
}

CDashPattern& CDashPattern::operator=( const CDashPattern& rhs )
{
	m_Array.insert( m_Array.end(), rhs.m_Array.begin(), rhs.m_Array.end() );
	m_Phase = rhs.m_Phase;
	return *this;
}
bool CDashPattern::operator==( const CDashPattern& rhs )
{
	if ((!m_Array.empty() && rhs.m_Array.empty()) || (m_Array.empty() && !rhs.m_Array.empty()))
		return false;
	if (m_Array != rhs.m_Array)
		return false;
	return m_Phase == rhs.m_Phase;
	//pdfGState rhs = obj as pdfGState ;
	//if (m_LineWidth != rhs.m_LineWidth
	//    || m_MiterLimit != rhs.m_MiterLimit
	//    || m_FlatnessTolerance != rhs.m_FlatnessTolerance
	//    || m_LineCap != rhs.m_LineCap
	//    || m_LineJoin != rhs.m_LineJoin
	//    || m_RenderingIntent != rhs.m_RenderingIntent
	//    || m_StrokeColor != rhs.m_StrokeColor
	//    || m_FillColor != rhs.m_FillColor
	//    || ExtGState != rhs.ExtGState
	//    || Mat != rhs.Mat
	//    || Dash != rhs.Dash
	//    || Clip != rhs.Clip
	//    )
	//    return false;
	//return true;
}

std::vector<int>& CDashPattern::GetArray()
{
	return m_Array;
}
void CDashPattern::SetArray( const std::vector<int>& src )
{
	m_Array = src;
}

int CDashPattern::GetPhase() const
{
	return m_Phase;
}
void CDashPattern::SetPhase( const int& src )
{
	m_Phase = src;
}

/////////////////////////
CGStateData::CGStateData(void)
{
}

//CGStateData::CGStateData(const CGStateData& src)
//{
//	operator=( src );
//}

CGStateData::~CGStateData(void)
{
}

//CGStateData& CGStateData::operator=(const CGStateData& _src)
//{
//	return *this;
//
//}

};//namespace Pdf
