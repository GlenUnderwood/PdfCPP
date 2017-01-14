#pragma once
#include <memory>
#include "Atom.h"

#include "ExportDef.h"

namespace Pdf {

class pdfDocument;

class PDF_API pdfInt : public pdfAtom
{
public:
	pdfInt(void);
	pdfInt(const pdfAtom& src);
	pdfInt(pdfDocument& doc, int bVal = 0);
	virtual ~pdfInt(void);

	pdfInt& operator=(const pdfAtom& src);
	inline pdfInt& operator=(const std::nullptr_t i)
	{
		SetValue( i );
		return *this;
	}


	pdfInt operator=(int src);
	//operator int();
protected:
};

};// namespace Exception
