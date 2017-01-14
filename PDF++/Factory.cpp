#include "stdafx.h"
#include "Constants.h"
#include "Document.h"
#include "Factory.h"
#include "pdfException.h"

using namespace Pdf::Exception;

namespace Pdf{

pdfFactory::pdfFactory(void) : m_doc(nullptr)
{
	if( !m_doc )
		throw pdfDocUndefinedException();
}

pdfFactory::pdfFactory(pdfDocument* doc) : m_doc(doc)
{
	if( !m_doc )
		throw pdfDocUndefinedException();
}

pdfFactory::~pdfFactory(void)
{
}

pdfInt pdfFactory::CreateInteger(int i, bool bIndirect)
{
	pdfInt atmNew(m_doc, i);
	if( bIndirect )
		m_doc->CrossReference().Add(atmNew, MinValueInt64);
	return atmNew;
}

pdfInt64 pdfFactory::CreateInteger64(__int64 i, bool bIndirect)
{
	pdfInt64 atmNew(m_doc, i);
	if( bIndirect )
		m_doc->CrossReference().Add(atmNew, MinValueInt64);
	return atmNew;
}

pdfReal pdfFactory::CreateReal(float d, int prec, bool bIndirect)
{
	pdfReal atmNew(m_doc, d, prec);
	if( bIndirect )
		m_doc->CrossReference().Add(atmNew, MinValueInt64);
	return atmNew;
}

pdfReal pdfFactory::CreateReal(double d, int prec, bool bIndirect)
{
	pdfReal atmNew(m_doc, d, prec);
	if( bIndirect )
		m_doc->CrossReference().Add(atmNew, MinValueInt64);
	return atmNew;
}
pdfBool pdfFactory::CreateBoolean(bool Val, bool bIndirect)
{
	pdfBool atmNew(m_doc, Val);
	if (bIndirect)
		m_doc->CrossReference().Add(atmNew, MinValueInt64);
	return atmNew;
}

pdfString pdfFactory::CreateString(const char* Val, StringEncoding encoding, bool bIndirect)
{
	pdfString atmNew(m_doc, Val, encoding);
	if (bIndirect)
		m_doc->CrossReference().Add(atmNew, MinValueInt64);
	return atmNew;
}
pdfString pdfFactory::CreateHexString( const byte* Val, size_t iSize, bool bTakeOwnership, bool bIndirect)
{
	pdfString atmNew(m_doc, Val, iSize, bTakeOwnership);
	if (bIndirect)
		m_doc->CrossReference().Add(atmNew, MinValueInt64);
	return atmNew;
}
pdfName pdfFactory::CreateName(const char* Val, bool bIndirect)
{
	pdfName atmNew(m_doc, Val);
	if (bIndirect)
		m_doc->CrossReference().Add(atmNew, MinValueInt64);
	return atmNew;
}
pdfDictionary pdfFactory::CreateDict(bool bIndirect)
{
	pdfDictionary atmNew(m_doc);
	if( bIndirect )
		m_doc->CrossReference().Add(atmNew, MinValueInt64);
	return atmNew;
}
pdfBlob pdfFactory::CreateStream()
{
	pdfBlob atmNew(m_doc);
	m_doc->CrossReference().Add(atmNew, MinValueInt64);
	return atmNew;
}
pdfArray pdfFactory::CreateArray(const size_t iStartingCount, bool bIndirect)
{
	pdfArray atmNew(m_doc,iStartingCount);
	if( bIndirect )
		m_doc->CrossReference().Add(atmNew, MinValueInt64);
	return atmNew;
}

/*

pdfImage pdfFactory::CreateImage()
{
	pdfBlob stmImg = CreateStream();
	stmImg["Type"] = pdfName(Keys.XObject, doc);
	stmImg["Subtype"] = pdfName(Keys.Image, doc);
	pdfImage newImg(stmImg);
	return newImg;
}
*/


};// Pdf ns
