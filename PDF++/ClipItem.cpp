#include "stdafx.h"
#include "ClipItem.h"


namespace Pdf {
namespace Content {


pdfClipItem::pdfClipItem()
{
}

pdfClipItem::pdfClipItem( const pdfDictionary & dictRes ) : m_Contents( nullptr, pdfResources( dictRes ) )
{
}

pdfClipItem::~pdfClipItem()
{
}

};
};