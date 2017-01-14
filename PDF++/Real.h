#pragma once
#include <memory>
#include "Atom.h"

#include "ExportDef.h"

namespace Pdf {

class pdfDocument;

class PDF_API pdfReal : public pdfAtom
{
public:
	pdfReal(void);
	pdfReal(const pdfAtom& src);
	pdfReal(const pdfDocument& doc, float bVal = false, int iPrecision = 8);
	pdfReal(const pdfDocument& doc, double bVal = false, int iPrecision = 8);
	virtual ~pdfReal(void);

	pdfReal& operator=(const pdfAtom& src);
	inline pdfReal& operator=(const std::nullptr_t i)
	{
		SetValue( i );
		return *this;
	}


	pdfReal& operator=(float src);
	pdfReal& operator=(double src);
	/*operator float();
	operator double();*/

	int GetPrecision() const;
	void SetPrecision(const int prec);
protected:
};



};// namespace Exception
