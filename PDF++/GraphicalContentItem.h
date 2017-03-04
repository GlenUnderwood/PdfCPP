#pragma once
#include "ExportDef.h"
#include "GState.h"
#include "ContentItem.h"

namespace Pdf {
namespace Content {

class PDF_API pdfGraphicalContentItem : public pdfContentItemBase
{
public:
	pdfGraphicalContentItem() = default;
	pdfGraphicalContentItem( const pdfGState& gs )
	{
		SetGState( gs );
	};


	pdfGState& GetGState() {
		return m_GState;
	};
	void SetGState( const pdfGState& gs )
	{
		m_GState = gs;
	};

protected:
	pdfGState m_GState;
};


};// namespace Content
};// namespace Pdf
