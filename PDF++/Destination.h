#pragma once
#include "Dictionary.h"

#include "ExportDef.h"

namespace Pdf {

class pdfDocument;

class PDF_API pdfDestination : public pdfDictionary//pdfObject
{
protected:
	pdfDestination(pdfDocument& doc);
public:
	pdfDestination(void);
	pdfDestination(const pdfAtom& src);
	virtual ~pdfDestination(void);

	pdfDestination& operator=(const pdfAtom& src);
	inline pdfDestination& operator=(const std::nullptr_t i) { SetValue( i ); return *this;}

};

};// namespace Exception
