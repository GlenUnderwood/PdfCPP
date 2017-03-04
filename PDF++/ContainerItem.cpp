#include "stdafx.h"
#include "ContainerItem.h"


namespace Pdf {
namespace Content {


pdfContainerItem::pdfContainerItem()
{
}

pdfContainerItem::pdfContainerItem( const pdfGState& src ) : pdfGraphicalContentItem( src )
{
}

pdfContainerItem::~pdfContainerItem()
{
}

};
};