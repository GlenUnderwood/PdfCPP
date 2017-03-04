#pragma once
#include "ContentItem.h"
#include "GraphicalContentItem.h"

namespace Pdf {
namespace Content {

class PDF_API pdfFormItem : public pdfGraphicalContentItem
{
public:
	pdfFormItem();
	pdfFormItem(const pdfGState& gs);
	virtual ~pdfFormItem();

	ContentType GetType() const override {
		return ContentType::FormXObject;
	};
};

};// namespace Content
};// namespace Pdf
