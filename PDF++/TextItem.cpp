#include "stdafx.h"
#include "TextItem.h"


namespace Pdf {
namespace Content {


pdfTextItem::pdfTextItem()
{
}

pdfTextItem::pdfTextItem(const pdfGState& src ) : pdfGraphicalContentItem(src)
{
}


pdfTextItem::~pdfTextItem()
{
}

};
};