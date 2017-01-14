#pragma once
#include <memory>
#include "Atom.h"

#include "ExportDef.h"

namespace Pdf {

class pdfDocument;

class PDF_API pdfName : public pdfAtom
{
public:
	pdfName(void);
	pdfName(const pdfAtom& src);
	pdfName(const pdfDocument& doc);
	pdfName(const pdfDocument& doc, const char* i);
	virtual ~pdfName(void);

	pdfName& operator=(const pdfAtom& src);
	pdfName operator=(const char* src);
	inline pdfName& operator=(const std::nullptr_t i)
	{
		SetValue( i );
		return *this;
	}

	/*	pdfReal operator=(const pdfAtom& src);
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
		return m_data->m_Val.dVal;
*/
protected:
};

};// namespace Exception
