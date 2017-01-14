#pragma once
#include <memory>
#include "Atom.h"
#include "Dictionary.h"
#include "Filters.h"

#include "ExportDef.h"

namespace Pdf {

class pdfDocument;

class PDF_API pdfBlob : public pdfDictionary
{
public:
	pdfBlob(void);
	pdfBlob(pdfDocument& doc );
	pdfBlob(const pdfAtom& atm );
	pdfBlob(const pdfDictionary& src, const size_t iPosition );
	virtual ~pdfBlob(void);

	pdfBlob& operator=(const pdfAtom& atm );
	inline pdfBlob& operator=(const std::nullptr_t i)
	{
		SetValue( i );
		return *this;
	}


	bool NeedsLoading() const;
	void EnsureLoaded(bool bDecrypt);
	//inline bool HasData();
	//void GetBytes(CSharedBuff& pBuff, bool bLeaveEncoded, bool bDecrypt ); 
	void GetBytes(std::ostream& stmOut, bool bLeaveEncoded, bool bDecrypt );
	/*void SetData(CSharedBuff& bNewData, size_t nCount, Filter::pdfFilters filters);
	void SetData(CSharedBuff& bNewData, const char* sFilter );
	void SetData(CSharedBuff& bNewData );

	void AppendData( CSharedBuff& bNewData );
	void AppendData( CSharedBuff& bNewData, Filter::pdfFilters filters );
	void Decode();
protected:
	void AppendData( CSharedBuff& bNewData, Filter::pdfFilters filters, bool bReplaceFilter );*/

};

};// namespace Pdf
