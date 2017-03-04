#pragma once
#include "ContentItem.h"
#include "GraphicalContentItem.h"

namespace Pdf {
namespace Content {

class PDF_API pdfContainerItem : public pdfGraphicalContentItem
{
public:
	pdfContainerItem();
	pdfContainerItem( const pdfGState& gs );
	virtual ~pdfContainerItem();

	ContentType GetType() const override {
		return ContentType::Container;
	};

};

};// namespace Content
};// namespace Pdf
