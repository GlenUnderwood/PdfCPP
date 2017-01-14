#pragma once
#include <memory>
#include "Object.h"
#include "Geometry.h"
#include "Contents.h"

#include "ExportDef.h"

namespace Pdf {

class pdfDocument;

class PDF_API pdfPage : public pdfObject
{
protected:
	pdfPage(pdfDocument& doc);
public:
	pdfPage(void);
	pdfPage(const pdfAtom& src);
	virtual ~pdfPage(void);

	pdfPage& operator=(const pdfAtom& src);
	inline pdfPage& operator=(const std::nullptr_t i) { m_atm.SetValue( i ); return *this;}

	Geometry::pdfRect GetMediaBox();
	void SetMediaBox( const Geometry::pdfRect& rect);
	Geometry::pdfRect GetCropBox();
	void SetCropBox( const Geometry::pdfRect& rect);

	inline pdfAtom Annotations();
	inline pdfContents Contents();
	inline pdfResources Resources();

protected:
	Geometry::pdfRect GetBox(const char* szName);
	void SetBox(const char* szName, const Geometry::pdfRect& rect );
};

};// namespace Exception
