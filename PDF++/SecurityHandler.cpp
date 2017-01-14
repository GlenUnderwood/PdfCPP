#include "stdafx.h"
#include "SecurityHandler.h"
#include "Model.h"
#include "Document.h"

namespace Pdf{
namespace Security{

pdfSecurityHandler::pdfSecurityHandler(void) //: m_doc(nullptr)
{
}

//pdfSecurityHandler::pdfSecurityHandler (pdfDocument& doc) : m_doc(doc)
//{
//}

pdfSecurityHandler::~pdfSecurityHandler(void)
{
}

/*static*/ bool pdfSecurityHandler::IsStandard(pdfDocument& doc)
{
	if ( doc.IsOpen() && doc.IsEncrypted() )
	{
		pdfDictionary enc( doc.Trailer()[ "Encrypt" ] );
		if ( !enc.IsNull() ) // otherwise throw an except!
		{
			std::string sFilter( enc[ "Filter" ].Str() );
			int V = enc.Has( "V" ) ? enc[ "V" ].Int() : 0;
			if ( sFilter != "Standard" || !( V >= 1 && V <= 3 ) )
				return false;
		}
	}
	return true;
};

} ; //namespace Security
} ; //namespace Pdf
