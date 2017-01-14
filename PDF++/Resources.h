#pragma once
#include "ExportDef.h"
#include "Object.h"
#include "GState.h"
#include "TState.h"

namespace Pdf {

class pdfDocument;

class PDF_API pdfResources : public pdfDictionary//pdfObject
{
protected:
	pdfResources(pdfDocument& doc);
public:
	pdfResources(void);
	pdfResources(const pdfAtom& src);
	virtual ~pdfResources(void);

	pdfResources& operator=(const pdfAtom& src);
	inline pdfResources& operator=(const std::nullptr_t i) { SetValue( i ); return *this;}

	string Add( pdfObject pObj );
	string Add( pdfAtom atmObj );

protected:
};

};// namespace Exception
