#pragma once
#include "ContentItem.h"

namespace Pdf {
namespace Content {

class PDF_API pdfClipItem : public pdfContentItemBase
{
public:
	pdfClipItem();
	virtual ~pdfClipItem();

	ContentType GetType() const override {
		return ContentType::Clipping;
	};
};


};// namespace Content
};// namespace Pdf
