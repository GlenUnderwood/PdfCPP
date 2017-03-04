#pragma once
#include "ContentItem.h"
#include "GraphicalContentItem.h"

namespace Pdf {
namespace Content {

class PDF_API pdfPathItem : public pdfGraphicalContentItem
{
public:
	pdfPathItem();
	pdfPathItem(const pdfGState& gs);
	virtual ~pdfPathItem();

	ContentType GetType() const override {
		return ContentType::Path;
	};
};

};// namespace Content
};// namespace Pdf
