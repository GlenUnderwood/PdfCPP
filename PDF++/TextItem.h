#pragma once
#include "ContentItem.h"
#include "GraphicalContentItem.h"

namespace Pdf {
namespace Content {

class PDF_API pdfTextItem : public pdfGraphicalContentItem
{
public:
	pdfTextItem();
	pdfTextItem(const pdfGState& gs);
	virtual ~pdfTextItem();

	ContentType GetType() const override {
		return ContentType::Text;
	};
};

};// namespace Content
};// namespace Pdf
