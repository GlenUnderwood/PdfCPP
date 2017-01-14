#pragma once
#include <memory>
#include "Atom.h"

#include "ExportDef.h"

namespace Pdf {

class pdfDocument;

class PDF_API pdfBool : public pdfAtom
{
public:
	pdfBool(void);
	pdfBool(const pdfAtom& src);
	pdfBool(pdfDocument& doc, bool bVal = false);
	virtual ~pdfBool(void);

	pdfBool& operator=(const pdfAtom& src);
	inline pdfBool& operator=(const std::nullptr_t i)
	{
		SetValue( i );
		return *this;
	}


	pdfBool operator=(bool src);
	//operator bool();

protected:
};

};// namespace Exception
