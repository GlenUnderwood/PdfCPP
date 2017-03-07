#pragma once
#include "ContentItem.h"
#include "Contents.h"

namespace Pdf {
namespace Content {

class PDF_API pdfClipItem : public pdfContentItemBase
{
	pdfClipItem();
public:
	pdfClipItem(const pdfDictionary& res);
	virtual ~pdfClipItem();

	ContentType GetType() const override {
		return ContentType::Clipping;
	};
	inline pdfContents& GetContents()
	{
		return m_Contents;
	};

protected:
	pdfContents m_Contents;
};


};// namespace Content
};// namespace Pdf
