#pragma once
#include <map>
#include <list>
#include <algorithm>
#include "Page.h"

#include "ExportDef.h"

namespace Pdf {

class pdfDocument;
class CDocData;

class PDF_API pdfPages// TODO Change to a pdfDict
{
	friend class CDocData;
	friend class pdfDocument;
public:
	pdfPages(void);
protected:
	pdfPages(const std::shared_ptr<CDocData>& src);
public:
	pdfPages(const pdfPages& src);
	virtual ~pdfPages(void);

protected:
	pdfPages& operator=(const std::shared_ptr<CDocData>& src);
public:
	pdfPages& operator=(const pdfPages& src);

	virtual pdfPage operator[]( const size_t i );
	virtual pdfPage operator[]( const pdfObjId& i );

	pdfPage Add(double Width = 612.0, double Height = 792.0);
	pdfPage Get( const size_t i );
	//void Set( const size_t idx, const pdfAtom& atm );
	size_t GetCount();
	void Remove( const size_t idx );
	void Clear();

protected:
	void GatherPages(pdfArray& arrKids);
	void Init();

	// 
	std::shared_ptr<CDocData> m_doc;
};

};// namespace Exception
