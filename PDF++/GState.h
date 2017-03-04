#pragma once
#include "ExportDef.h"
#include "Enum.h"
#include "Geometry.h"
#include "ExtGState.h"
#include "Color.h"

#include <vector>

namespace Pdf {

namespace Content {
	class pdfClipItem;
}
class PDF_API pdfGState
{
public:
	pdfGState( void );
	pdfGState(  const pdfGState& rhs );
	virtual ~pdfGState(void);

	pdfGState& operator=( const pdfGState& rhs );

	class DashPattern
	{
	public:
		DashPattern() :m_Phase( 0 ) { }
		DashPattern( const DashPattern& rhs );

		DashPattern& operator=( const DashPattern& rhs );

		std::vector<int>& GetArray();
		void SetArray(const std::vector<int>& src);

		int GetPhase() const;
		void SetPhase( const int& src );

		bool operator==( const DashPattern& rhs );
		bool operator!=( const DashPattern& rhs ) {
			return !operator==( rhs );
		};
	private:
		std::vector<int> m_Array;
		int m_Phase;
	};

	//////
	GStateFlags GetFlags();
	void SetFlags( const GStateFlags& value );

	pdfColor& GetStrokeColor();
	void SetStrokeColor( const pdfColor& src );

	pdfColor& GetFillColor();
	void SetFillColor( const pdfColor& value );

	float GetLineWidth();
	void SetLineWidth( const float value );

	float GetMiterLimit();
	void SetMiterLimit( const float value );

	Geometry::pdfMatrix GetMatrix();
	void SetMatrix( const Geometry::pdfMatrix& value );

	DashPattern* GetDash();
	void SetDash( const DashPattern* value );

	byte GetFlatnessTolerance();
	void SetFlatnessTolerance( const byte value );

	byte GetLineCap();
	void SetLineCap( const byte value );

	byte GetLineJoin();
	void SetLineJoin( const byte value );

	const char* GetRenderingIntent();
	void SetRenderingIntent( const char* val);

	pdfExtGState GetExtGState();
	void SetExtGState( const pdfExtGState& value );

	//Content::pdfClipItem& GetClip();
	//void SetClip( const Content::pdfClipItem& value );

	GStateFlags Differences( const pdfGState& rhs );

protected:
	void* data;

	GStateFlags m_Flags;
	pdfColor m_StrokeColor;
	pdfColor m_FillColor;
	float m_LineWidth;
	float m_MiterLimit;
	byte m_FlatnessTolerance;
	byte m_LineCap;

	byte m_LineJoin;
	std::string m_RenderingIntent ;

	std::unique_ptr<DashPattern> m_Dash;
	//Content::pdfClipItem m_pathClip;
	pdfExtGState m_ExtGState;

	Geometry::pdfMatrix m_Mat;
};

};

