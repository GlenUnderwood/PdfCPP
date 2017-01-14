#pragma once
#include "Atom.h"
#include "Int.h"
#include "Int64.h"
#include "Real.h"
#include "Bool.h"
#include "String.h"
#include "Name.h"
#include "Dictionary.h"
#include "Blob.h"
#include "Array.h"

#ifdef PDF_EXPORTS
#define PDF_API __declspec(dllexport)
#else
#define PDF_API __declspec(dllimport)
#endif

namespace Pdf{

class PDF_API pdfDocument;

class PDF_API pdfFactory
{
public:
	pdfFactory(void);
	pdfFactory(pdfDocument* doc);
	virtual ~pdfFactory(void);

	pdfInt		CreateInteger(int i, bool bIndirect);
	pdfInt64	CreateInteger64(__int64 i, bool bIndirect);
	pdfReal		CreateReal(float d, int iPrecision=8, bool bIndirect = false);
	pdfReal		CreateReal(double d, int iPrecision=8, bool bIndirect = false);
	pdfBool		CreateBoolean(bool Val, bool bIndirect);
	pdfString	CreateString(const char* Val, StringEncoding encoding, bool bIndirect);
	pdfString	CreateHexString( const byte* Val, size_t iSize, bool bTakeOwnership, bool bIndirect);
	pdfName		CreateName(const char* Val, bool bIndirect);
	pdfDictionary CreateDict(bool bIndirect);
	pdfBlob		CreateStream();
	pdfArray	CreateArray(const size_t iInitialCount, bool bIndirect);
protected:
	pdfDocument* m_doc;
};

};//