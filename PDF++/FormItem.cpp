#include "stdafx.h"
#include "FormItem.h"


namespace Pdf {
namespace Content {


pdfFormItem::pdfFormItem()
{
}

pdfFormItem::pdfFormItem(const pdfGState& src ) : pdfGraphicalContentItem(src)
{
}


pdfFormItem::~pdfFormItem()
{
}

};
};