#include "stdafx.h"
#include "Page.h"
#include "Enum.h"
#include "Document.h"
#include "Array.h"

using namespace Pdf::Geometry;

namespace Pdf {

pdfPage::pdfPage(void)// : pdfObject()
{
}

pdfPage::pdfPage(const pdfAtom& src) : pdfDictionary(src)
{
	if( !IsNull() )
	{
		if (GetType() != AtomType::Dictionary )
			throw pdfTypeMismatchException();

		bool bIsPage = Get("Type").Str() == "Page";
		if( !bIsPage )
			throw pdfObjectInvalidEx();
	}
}

pdfPage::pdfPage(pdfDocument& doc)  : pdfDictionary(doc)
{

}

pdfPage::~pdfPage(void)
{
}

pdfPage& pdfPage::operator=(const pdfAtom& src)
{
	pdfDictionary::operator=(src);
	if( !IsNull() )
	{
		if (GetType() != AtomType::Dictionary )
			throw pdfTypeMismatchException();

		bool bIsPage = Get("Type").Str() == "Page";
		if( !bIsPage )
			throw pdfObjectInvalidEx();
	}
	return *this;
}

pdfRect pdfPage::GetBox(const char* szName)
{
	CheckIsValid();

	pdfRect rc;
	pdfArray box( operator[](szName) );

	if( !box.IsNull() )
	if( box )
	{
		if( box.GetCount() < 4 )
			throw pdfException("Invalid page rectangle format.");

		rc.Left = box[0].Real();
		rc.Top = box[1].Real();
		rc.Right = box[2].Real();
		rc.Bottom = box[3].Real();
	}

	return rc;
}

void pdfPage::SetBox(const char* szName, const pdfRect& rc)
{
	CheckIsValid();

	pdfArray box( operator[](szName) );

	//if( !box.IsNull() )
	if( box )
	{
		if( box.GetCount() < 4 )
			throw pdfException("Invalid page rectangle format.");

		/*rc.Left = box[0].Real();
		rc.Top = box[1].Real();
		rc.Right = box[2].Real();
		rc.Bottom = box[3].Real();*/
	}
	else if( !box )
	{
		pdfDocument doc( m_data->m_doc.lock() );
		box = doc.CreateArray( 4 );

		Set( szName, box );
	}
	box[0].SetValue( rc.Left );
	box[1].SetValue( rc.Top );
	box[2].SetValue( rc.Right );
	box[3].SetValue( rc.Bottom );
}

pdfRect pdfPage::GetMediaBox()
{
	return GetBox("MediaBox");
}

void pdfPage::SetMediaBox( const pdfRect& rect)
{
	SetBox( "Media", rect );
}

void pdfPage::SetCropBox( const pdfRect& rect)
{
	SetBox( "Crop", rect );
}

	/*public RectangleF GetBox(string sName)
	{
		//RectangleF rc = new RectangleF(0,0,0,0);
		pdfAtom atmCrop = this[sName];
		if (atmCrop != null && atmCrop is pdfArray)
		{
			pdfArray arr = atmCrop as pdfArray;
			if (arr.Count >= 4)
			{
				return new RectangleF( arr.m_lst[ 0 ].RealF, arr.m_lst[ 1 ].RealF,
										arr.m_lst[ 2 ].RealF - arr.m_lst[ 0 ].RealF, arr.m_lst[ 3 ].RealF - arr.m_lst[ 1 ].RealF );
				//rc.Location = new PointF(arr.m_lst[0].RealF, arr.m_lst[1].RealF);// = RectangleF.FromLTRB(arr.m_lst[0].RealF, arr.m_lst[3].RealF, arr.m_lst[2].RealF, arr.m_lst[1].RealF);
				//rc.Size = new SizeF(arr.m_lst[2].RealF, arr.m_lst[3].RealF);// = RectangleF.FromLTRB(arr.m_lst[0].RealF, arr.m_lst[3].RealF, arr.m_lst[2].RealF, arr.m_lst[1].RealF);
			}
		}
		return new RectangleF(0,0,0,0);
	}
		*/
pdfRect pdfPage::GetCropBox()
{
	if( Has("CropBox") )
		return GetBox("CropBox");
	return GetBox("MediaBox");
}

pdfAtom pdfPage::Annotations()
{
	//pdfDictionary me( GetDict() );
	if ( Has("Annotations") )
		return Get( "Annotations" );
	return pdfAtom();
}

pdfContents pdfPage::Contents()
{
	pdfContents res;
	//pdfDictionary me( GetDict() );
	if ( Has("Contents") )
		return Get( "Contents" );
	return res;
}

pdfResources pdfPage::Resources()
{
	pdfResources res;
	//pdfDictionary me( GetDict() );
	if ( Has("Resources") )
		return Get( "Resources" );
	return res;
}

};// namespace Pdf
