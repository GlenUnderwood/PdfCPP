#pragma once
#include "ContentItem.h"
#include "GraphicalContentItem.h"

namespace Pdf {
namespace Content {

class PDF_API pdfImageItem : public pdfGraphicalContentItem
{
public:
	pdfImageItem();
	pdfImageItem(const pdfGState& gs);
	virtual ~pdfImageItem();

	ContentType GetType() const override {
		return ContentType::Image;
	};
};

};// namespace Content
};// namespace Pdf
