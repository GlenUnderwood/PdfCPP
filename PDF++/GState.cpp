#include "stdafx.h"
#include "Array.h"
#include "GState.h"

namespace Pdf {

using namespace Content;
using namespace Geometry;

pdfGState::pdfGState( void ) : 
	m_RenderingIntent( "RelativeColorimetric" ),
	m_StrokeColor( 1.0F, 0 ), m_FillColor( 1.0F, 0 ),
	m_LineWidth( 1.0F ), m_MiterLimit( 10.0F ),
	m_Flags( GStateFlags::NoGState )
{
}
pdfGState::pdfGState( const pdfGState& rhs  )
{
	operator=( rhs );
}


pdfGState::~pdfGState(void)
{
}

pdfGState& pdfGState::operator=( const pdfGState& src )
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
	SetClip( src.m_pathClip );
	SetRenderingIntent( src.m_RenderingIntent.c_str() );
	SetDashPattern( src.m_Dash );
	SetExtGState( src.m_ExtGState );//new pdfExtGState(src.....

	m_Flags = src.m_Flags;

	return *this;
};

GStateFlags pdfGState::GetFlags() {
	return m_Flags;
}

void pdfGState::SetFlags( const GStateFlags& src)
{
	m_Flags = src;
}

pdfColor& pdfGState::GetStrokeColor()
{
	if (m_ExtGState != nullptr)
		m_StrokeColor.A = m_ExtGState.GetStrokeOpacity();
	else
		m_StrokeColor.A = 1;
	return m_StrokeColor;
}
void pdfGState::SetStrokeColor( const pdfColor& value )
{
	m_StrokeColor = pdfColor( value );
	if (m_ExtGState != nullptr)
		m_StrokeColor.A = m_ExtGState.GetStrokeOpacity();
	m_Flags |= GStateFlags::StrokeColor;
}

pdfColor& pdfGState::GetFillColor()
{
	if (m_ExtGState != nullptr)
		m_FillColor.A = m_ExtGState.GetFillOpacity();
	else
		m_FillColor.A = 1;
	return m_FillColor;
}
void pdfGState::SetFillColor( const pdfColor& value )
{
	m_FillColor = pdfColor( value );

	if (m_ExtGState != nullptr)
		m_FillColor.A = 1.0f - m_ExtGState.GetFillOpacity();

	m_Flags |= GStateFlags::FillColor;
}

float pdfGState::GetLineWidth()
{
	if (m_ExtGState != nullptr && m_ExtGState.Has( "LW" ))
		return m_ExtGState["LW"].RealF();
	return m_LineWidth;
}
void pdfGState::SetLineWidth( const float value )
{
	m_LineWidth = value;
	if (m_ExtGState != nullptr && m_ExtGState.Has( "LW" ))
		m_ExtGState.SetNumber( "LW", (int)value );
	m_Flags |= GStateFlags::LineWidth;
}

float pdfGState::GetMiterLimit()
{
	if (m_ExtGState != nullptr && m_ExtGState.Has( "ML" ))
		return m_ExtGState["ML"].RealF();
	return m_MiterLimit;
}
void pdfGState::SetMiterLimit( const float value )
{
	m_MiterLimit = value;
	if (m_ExtGState != nullptr && m_ExtGState.Has( "ML" ))
		m_ExtGState.SetNumber( "ML", (int)value );
	m_Flags |= GStateFlags::MiterLimit;
}


Geometry::pdfMatrix pdfGState::GetMatrix()
{
	return m_Mat;
}
void pdfGState::SetMatrix( const Geometry::pdfMatrix& value )
{
	m_Mat = value;
	m_Flags |= GStateFlags::Mat;
}


pdfGState::DashPattern::Ptr pdfGState::GetDashPattern()
{ 
	if (m_ExtGState != nullptr && m_ExtGState.Has("D"))
	{
		pdfAtom atm( m_ExtGState["D"] );
		pdfArray arrD( atm );
		if (arrD != nullptr && arrD.GetCount() >= 2) // must be two entries: a Dash array then a phase val
		{
			/*arrD.ForEach( [] ( pdfAtom& atm ) -> bool {
			} );*/

			pdfArray arrDash(arrD[0]);
			if (arrDash != nullptr)
			{
				m_Dash = std::make_shared<DashPattern>();
				//DashPattern extDash = new pdfGState.DashPattern();
				if (arrDash.GetCount() > 0)
				{
					std::vector<int>& vec = m_Dash->GetArray();
					vec.clear();
					vec.resize(arrDash.GetCount());
					for (int idx = 0; idx < arrDash.GetCount(); idx++)
					{
						vec[idx] = arrDash[idx].Int();
					}
				}
				//
				m_Dash->SetPhase(arrD[1].Int());
			}
		}
	}
	//if( m_Dash==null)
	//	m_Dash = new pdfGState.DashPattern();
	return m_Dash;
} 
void pdfGState::SetDashPattern( const pdfGState::DashPattern::Ptr& src )
{
	if (src.get() == m_Dash.get())
		return;
	m_Dash.reset();
	if (src != nullptr)
	{
		m_Dash = std::make_shared<DashPattern>( *src );
		/*m_Dash->SetArray( src->GetArray() );
		m_Dash->SetPhase( src->GetPhase() );*/
	}

	m_Flags |= GStateFlags::Dash;
} 

byte pdfGState::GetFlatnessTolerance()
{
	if (m_ExtGState != nullptr && m_ExtGState.Has("FL"))
		m_FlatnessTolerance = (byte)m_ExtGState["FL"].Int();
	return m_FlatnessTolerance;
} 
void pdfGState::SetFlatnessTolerance(const byte value)
{
	m_FlatnessTolerance = value;
	m_Flags |= GStateFlags::Flatness;
} 

byte pdfGState::GetLineCap()
{
	if (m_ExtGState != nullptr && m_ExtGState.Has("LC"))
		return (byte)m_ExtGState["LC"].Int();
	return m_LineCap;
} 
void pdfGState::SetLineCap(const byte value)
{
	m_LineCap = value;
	if (m_ExtGState != nullptr && m_ExtGState.Has("LC"))
		m_ExtGState.SetNumber( "LC", (int)value );
	m_Flags |= GStateFlags::LineCap;
}

byte pdfGState::GetLineJoin()
{
	if (m_ExtGState != nullptr && m_ExtGState.Has("LJ"))
		return (byte)m_ExtGState["LJ"].Int();
	return m_LineJoin;
} 
void pdfGState::SetLineJoin( const byte value )
{
	m_LineJoin = value;
	if (m_ExtGState != nullptr && m_ExtGState.Has("LJ"))
		m_ExtGState.SetNumber( "LJ", (int)value );
	m_Flags |= GStateFlags::LineJoin;
} 

const char* pdfGState::GetRenderingIntent()
{
	if (m_ExtGState != nullptr && m_ExtGState.Has("RI"))
		return m_ExtGState["RI"].Str().c_str();
	return m_RenderingIntent.c_str();
} 

void pdfGState::SetRenderingIntent( const char* value)
{
	m_RenderingIntent = value;

	if (m_RenderingIntent.empty())
		m_RenderingIntent = "RelativeColorimetric";

	if ( m_ExtGState != nullptr && m_ExtGState.Has( "RI" ) )
		m_ExtGState.SetName( "RI", m_RenderingIntent.c_str() );
	m_Flags |= GStateFlags::RI;
} 

pdfExtGState pdfGState::GetExtGState()
{
	return m_ExtGState;
}

void pdfGState::SetExtGState(const pdfExtGState& value)
{
	// replace any vals from the ExtGState
	m_ExtGState = nullptr;
	if (value != nullptr)
		m_ExtGState = value;// std::make_unique<pdfExtGState>( *value );
	m_Flags |= GStateFlags::ExtGState_;
}

Content::pdfClipItem pdfGState::GetClip()
{
	return m_pathClip;
}

void pdfGState::SetClip(const pdfClipItem& value )
{
	m_pathClip = value;
	m_Flags |= GStateFlags::Clip;
}

GStateFlags pdfGState::Differences(const pdfGState& rhs)
{
	GStateFlags flags = GStateFlags::NoGState;
	if (m_LineWidth != rhs.m_LineWidth )
		flags |= GStateFlags::LineWidth;
	if (m_MiterLimit != rhs.m_MiterLimit)
		flags |= GStateFlags::MiterLimit;
	if (m_FlatnessTolerance != rhs.m_FlatnessTolerance)
		flags |= GStateFlags::Flatness;
	if (m_LineCap != rhs.m_LineCap)
		flags |= GStateFlags::LineCap;
	if (m_LineJoin != rhs.m_LineJoin)
		flags |= GStateFlags::LineJoin;
	if (m_RenderingIntent != rhs.m_RenderingIntent)
		flags |= GStateFlags::RI;
	if (m_StrokeColor != rhs.m_StrokeColor)
		flags |= GStateFlags::StrokeColor;
	if (m_FillColor != m_FillColor )
		flags |= GStateFlags::FillColor;
	if (m_ExtGState != rhs.m_ExtGState)
		flags |= GStateFlags::ExtGState_;


	pdfMatrix m1 = GetMatrix();
	pdfMatrix m2 = const_cast<pdfGState&>(rhs).GetMatrix();
	if ( m1 != m2 )
		flags |= GStateFlags::Mat;

	DashPattern::Ptr dash1 = GetDashPattern();
	DashPattern::Ptr dash2 = const_cast<pdfGState&>(rhs).GetDashPattern();
	if ( ((dash1 == nullptr && dash2!=nullptr) || (dash1 != nullptr && dash2==nullptr)) || ( dash1 != nullptr && (*dash1)!=(*dash2) ) )
		flags |= GStateFlags::Dash;

	if ( m_pathClip != rhs.m_pathClip )
		flags |= GStateFlags::Clip;
	/*if (((m_pathClip == nullptr && rhs.m_pathClip != nullptr) || 
		(m_pathClip != nullptr && rhs.m_pathClip == nullptr)) || 
		(m_pathClip != nullptr && (*m_pathClip) != (*rhs.m_pathClip)))
		flags |= GStateFlags::Clip;*/

	return flags;
}


////////////////////

pdfGState::DashPattern::DashPattern( const pdfGState::DashPattern& rhs ) : m_Phase( 0 )
{
	operator=( rhs );
}

pdfGState::DashPattern& pdfGState::DashPattern::operator=( const pdfGState::DashPattern& rhs )
{
	m_Array.insert( m_Array.end(), rhs.m_Array.begin(), rhs.m_Array.end() );
	m_Phase = rhs.m_Phase;
	return *this;
}
bool pdfGState::DashPattern::operator==( const pdfGState::DashPattern& rhs )
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

std::vector<int>& pdfGState::DashPattern::GetArray()
{
	return m_Array;
}
void pdfGState::DashPattern::SetArray( const std::vector<int>& src )
{
	m_Array = src;
}

int pdfGState::DashPattern::GetPhase() const
{
	return m_Phase;
}
void pdfGState::DashPattern::SetPhase( const int& src )
{
	m_Phase = src;
}

};