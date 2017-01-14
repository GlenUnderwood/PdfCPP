#include "stdafx.h"
#include "Document.h"
#include "Pages.h"
#include "Array.h"

namespace Pdf {

using namespace Exception;

pdfPages::pdfPages(void)
{
	// we should always have a m_data, like pdfAtom does
	m_doc.reset( new CDocData() );
}

pdfPages::pdfPages(const std::shared_ptr<CDocData>& doc)
{
	operator=(doc);
}

pdfPages::pdfPages(const pdfPages& src)
{
	operator=(src);
}

pdfPages::~pdfPages(void)
{
}

pdfPages& pdfPages::operator=(const std::shared_ptr<CDocData>& src)
{
	if( m_doc != src )
		m_doc = src;
	if( m_doc == nullptr )
		m_doc.reset( new CDocData() );
	return *this;
}

pdfPages& pdfPages::operator=(const pdfPages& src)
{
	if( m_doc != src.m_doc )
		m_doc = src.m_doc;
	if( m_doc == nullptr )
		m_doc.reset( new CDocData() );
	return *this;
}

pdfPage pdfPages::operator[]( const size_t idx )
{
	Init();

	if( idx >= m_doc->m_pgs.size() ) 
		throw pdfBadIndexException();

	size_t i = 0;
	for( auto it = m_doc->m_pgs.begin(); it != m_doc->m_pgs.end(); ++it )
		if( i++ == idx )
			return *it;
	return pdfPage();
}

pdfPage pdfPages::operator[]( const pdfObjId& id )
{
	Init();
	for( auto it = m_doc->m_pgs.begin(); it != m_doc->m_pgs.end(); ++it )
		if( it->operator==(id) )
			return *it;
	return pdfPage();
}

pdfPage pdfPages::Add(double Width, double Height)
{
	Init();

	pdfDocument doc( m_doc );

	// TODO clearly this needs more work.
	pdfPage pgNew;;
	pdfDictionary dictCatalog( m_doc->m_Trailer[ "Root" ] );
	pdfDictionary dictPages( dictCatalog["Pages"] );


	// add to xref table first, this will generate a reference which we add to our kids array.
	pdfDictionary dictPage = doc.CreateDict(true);
	dictPage.SetName( "Type", "Page" );

	//
	pgNew = dictPage;

	pdfArray arrKids = dictPages["Kids"];
	arrKids.Add(dictPage);

	// add a pointer to our little easy-access list
	m_doc->m_pgs.push_back(pgNew);

	// alter what's there? or replace?
	if( dictPages.Has("Count") )
		dictPages["Count"].SetValue( (int)m_doc->m_pgs.size() );
	else
		dictPages.SetNumber( "Count", (int)m_doc->m_pgs.size() );


	// Set some basic info.
	//pgNew.CropBox = new RectangleF(0, 0, (float)Width, (float)Height);
	pgNew.SetMediaBox( Geometry::pdfRect(0, 0, (float)Width, (float)Height) );
	pgNew.Set( "Parent", dictPages );// will automatically use the reference if there is one.
	return pgNew;
}

pdfPage pdfPages::Get( const size_t i )
{
	return operator[](i);
}
//	void Set( const size_t idx, const pdfAtom& atm );
size_t pdfPages::GetCount()
{
	Init();
	return m_doc->m_pgs.size();
}

void pdfPages::Remove( const size_t idx )
{
	/*if( idx >= m_items.size() ) 
		throw pdfBadIndexException();
*/
}

void pdfPages::Clear()
{
	if( m_doc==nullptr )
		throw pdfDocUndefinedException();
//	pdfDocument* doc = m_doc;
	m_doc->m_pgs.clear();

//	m_doc = doc;
}

void pdfPages::Init()
{
	if( m_doc==nullptr )
		throw pdfDocUndefinedException();

	if( m_doc!=nullptr && m_doc->m_pgs.size() == 0 )
	{
		pdfDocument doc( m_doc );

		pdfDictionary dictCatalog( m_doc->m_Trailer[ "Root" ] );
		pdfDictionary dictPages;//( dictCatalog["Pages"] );
		pdfArray arrKids;

		if( !dictCatalog.Has( "Pages" ) )
		{
			dictPages = dictCatalog.Set( "Pages", doc.CreateDict( true ) );

			dictPages.SetNumber( "Count", (int)0 );
			dictPages.SetName( "Type", "Pages" );//  TODO Maybe turn pdfPages into a pdfObject-derived class which has a pdfObType enum (for the catalog,bookmarks,etc obj, too).  Just to have high-level objs around
		}
		else
			dictPages = dictCatalog["Pages"];

		if( !dictPages.Has( "Kids" ) )
			arrKids = dictPages.Set( "Kids", doc.CreateArray( 0/*, true*/ ) );
		else
			arrKids = dictPages["Kids"];

		GatherPages( arrKids );
	}
}

void pdfPages::GatherPages(pdfArray& arrKids)
{
	if ( !arrKids.IsNull() )
	{
		size_t i =0, c = arrKids.GetCount();
		for( ;i < c; i++ )
		{
			pdfAtom atm( arrKids[i] );
			if ( atm.IsArray() )
			{
				pdfArray arr( atm );
				GatherPages( arr );
			}
			else if ( atm.IsDictionary() )
			{
				pdfDictionary atmDict(atm);
				if (atmDict.Has("Type"))
				{
					string sType = atmDict["Type"].Str();
					if (sType == "Page")
					{
						pdfPage pg( atm );

						m_doc->m_pgs.push_back(pg);
					}
					else if (sType == "Pages" && atmDict.Has("Kids"))
					{
						pdfArray arr( atmDict["Kids"] );
						GatherPages( arr );
					}
				}
			}
			//else
			//	_ASSERT(false);
		}
	}
}

};// namespace Pdf
