#pragma once
#include "Dictionary.h"

#include "ExportDef.h"

namespace Pdf {

class pdfDocument;

class PDF_API pdfExtGState : public pdfDictionary//pdfObject
{
protected:
	pdfExtGState(pdfDocument& doc);
public:
	pdfExtGState(void);
	pdfExtGState(const pdfAtom& src);
	virtual ~pdfExtGState(void);

	pdfExtGState& operator=(const pdfAtom& src);
	inline pdfExtGState& operator=(const std::nullptr_t i) { SetValue( i ); return *this;}

	inline float GetAlphaIsShape()
	{
		return _Get( "AIS", false );
	}
	inline void SetAlphaIsShape(const bool& value)
	{
		_Set( "AIS", value );
	}
	inline float GetStrokeOpacity()
	{
		return _Get( "CA", 1.0f );
	}
	inline void SetStrokeOpacity(const float& value)
	{
		_Set( "CA", value );
	}
	inline float GetFillOpacity()
	{
		return _Get( "ca", 1.0f );
	}
	inline void SetFillOpacity(const float& value)
	{
		_Set( "ca", value );
	}

	// TODO Need to find a good way to return strings.
	/*inline const char* GetBlendMode()
	{
		return _Get( "BM", "Normal" );
	}
	inline void SetBlendMode(const char* value)
	{
		_Set( "BM", value );
	}*/
};

};// namespace Exception
