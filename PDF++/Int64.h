#pragma once
#include <memory>
#include "Atom.h"

#include "ExportDef.h"

namespace Pdf {

class pdfDocument;

class PDF_API pdfInt64 : public pdfAtom
{
public:
	pdfInt64(void);
	pdfInt64(const pdfAtom& src);
	pdfInt64(pdfDocument& doc, int64 bVal = 0);
	virtual ~pdfInt64(void);

	pdfInt64& operator=(const pdfAtom& src);
	inline pdfInt64& operator=(const std::nullptr_t i)
	{
		SetValue( i );
		return *this;
	}

	pdfInt64 operator=(int64 src);
	//operator int64()
protected:
};

};// namespace Exception
