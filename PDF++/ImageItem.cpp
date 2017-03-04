#include "stdafx.h"
#include "ImageItem.h"


namespace Pdf {
namespace Content {


pdfImageItem::pdfImageItem()
{
}

pdfImageItem::pdfImageItem(const pdfGState& src ) : pdfGraphicalContentItem(src)
{
}


pdfImageItem::~pdfImageItem()
{
}

};
};