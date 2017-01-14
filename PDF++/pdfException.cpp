#include "stdafx.h"
#include "pdfException.h"

namespace Pdf{
namespace Exception{

pdfException::pdfException(void)
{
	m_err=-1;
}


pdfException::~pdfException(void)
{
}

// CSerializable
void pdfException::Serialize( std::ostream& stm )
{
	if( m_explanation.size() > 0 )
		stm << what() << "(" << m_err << "): " << m_explanation.c_str();
	else
		stm << what() << "(" << m_err << ")";
}

};// namespace Exception
};// namespace Pdf

/*std::ostream& operator<<( std::ostream& stm, const Pdf::Exception::pdfException& ex )
{
	if( ex.explanation() != nullptr && ex.explanation()[0] !=0 )
		stm << ex.what() << "(" << ex.err() << "): " << ex.explanation();
	else
		stm << ex.what() << "(" << ex.err() << ")";
	return stm;
}
*/
