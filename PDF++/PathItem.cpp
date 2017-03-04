#include "stdafx.h"
#include "PathItem.h"


namespace Pdf {
namespace Content {


pdfPathItem::pdfPathItem()
{
}

pdfPathItem::pdfPathItem(const pdfGState& src ) : pdfGraphicalContentItem(src)
{
}


pdfPathItem::~pdfPathItem()
{
}

};
};