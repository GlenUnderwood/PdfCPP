#pragma once
#include <memory>
#include "Atom.h"

#include "ExportDef.h"

namespace Pdf {

class pdfDocument;

class PDF_API pdfString : public pdfAtom
{
public:
	pdfString(void);
	pdfString(const pdfAtom& src);
	pdfString(pdfDocument& doc);
	pdfString(pdfDocument& doc, const char* i, StringEncoding enc);
	pdfString(pdfDocument& doc, const wchar_t* i, StringEncoding enc);
	pdfString(pdfDocument& doc, const byte* pBytes, size_t nBytes, StringEncoding enc = StringEncoding::Byte );
	pdfString(pdfDocument& doc, const CSharedBuff& src, StringEncoding enc = StringEncoding::Byte );
	virtual ~pdfString(void);

	pdfString& operator=(const pdfAtom& src);
	inline pdfString& operator=(const std::nullptr_t i)
	{
		SetValue( i );
		return *this;
	}


	StringEncoding GetStringEncoding() const;
	void SetStringEncoding( StringEncoding enc);

	CSharedBuff GetStringBytes(bool bDecrypt) const;
	void SetStringBytes(const byte* pBytes, size_t nBytes, StringEncoding enc = StringEncoding::Byte);
	void SetStringBytes(const CSharedBuff& src, StringEncoding enc = StringEncoding::Byte);

	/*pdfReal operator=(const pdfAtom& src);
	pdfReal operator=(float src){
		if( m_data==nullptr || m_data->m_doc.expired() || !m_data->m_doc.lock()->IsOpen() )
			throw pdfDocUndefinedException(); 
		m_data->m_Val.dVal = src;
		return *this;
	}
	pdfReal operator=(double src){
		if( m_data==nullptr || m_data->m_doc.expired() || !m_data->m_doc.lock()->IsOpen() )
			throw pdfDocUndefinedException(); 
		m_data->m_Val.dVal = src;
		return *this;
	}
	operator float(){
		return m_data->m_Val.dVal;
	};
	operator double(){
		return m_data->m_Val.dVal;*/
protected:
};

};// namespace Exception
