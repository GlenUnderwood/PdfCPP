#pragma once
#include "Filter.h"
#include "Dictionary.h"
#include <list>

#include "ExportDef.h"

namespace Pdf {
namespace Filter {

class PDF_API pdfFilters : public pdfFilter
{
public:
	pdfFilters(void);
	pdfFilters(const pdfDictionary& dictSrc);
	virtual ~pdfFilters(void);

	/*static */pdfFilter* FilterFromName( const char* sFilter );
	//static pdfFilters GetFilters(const pdfBlob& dictSrc);// can be an image or any stream obejct

	// pdfFilter
	void Transform( const CSharedBuff& buffSrc, CSharedBuff& buffDest, bool bDecode );
	void Transform( const CSharedBuff& buffSrc, std::ostream& buffDest, bool bDecode );
/*	void Encode( const CSharedBuff& buffSrc, CSharedBuff& buffDest );
	void Encode( const CSharedBuff& buffSrc, std::ostream& stmDest );
	void Decode( const CSharedBuff& buffSrc, CSharedBuff& buffDest );
	void Decode( const CSharedBuff& buffSrc, std::ostream& stmDest );*/
	FilterType GetType() const { return FilterType::NoFilter; };
	const char* ToString() { return ""; };

	//pdfAtom CreateObj( pdfDocument* doc );

	inline void Add(const char* sFilter);
	inline void Add(const pdfFilter& pFilter);
	inline size_t GetCount() const;
	//inline pdfFilter& GetFilter(int idx);

protected:
	std::shared_ptr<std::list<pdfFilter*>> m_lst;
};

}; //namespace Filter 
}; //namespace Pdf 
