#pragma once
#include "ExportDef.h"
#include "Enum.h"
//#include "Resources.h"

namespace Pdf {
namespace Content {

class PDF_API pdfContentItemBase
{
public:
	pdfContentItemBase( void );
	virtual ~pdfContentItemBase(void);

	virtual ContentType GetType() const {
		return ContentType::Unknown;
	};
protected:

};

};// namespace Content
};// namespace Pdf
