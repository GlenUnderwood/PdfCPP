#pragma once
#include "ExportDef.h"
#include "Enum.h"
#include "Geometry.h"
#include "ExtGState.h"
#include "ClipItem.h"
#include "Color.h"

#include <vector>

namespace Pdf {


class PDF_API pdfGState
{
public:
	pdfGState( void );
	pdfGState(  const pdfGState& rhs );
	virtual ~pdfGState(void);

	pdfGState& operator=( const pdfGState& rhs );

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

	std::vector<int> GetDashArray();
	void SetDashArray( const std::vector<int>& value );
	int GetDashPhase();
	void SetDashPhase( const int value );

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

	Content::pdfClipItem& GetClip();
	void SetClip( const Content::pdfClipItem& value );

	GStateFlags Differences( const pdfGState& rhs );

protected:
	void* data;
};

};

