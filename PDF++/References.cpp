#include "stdafx.h"
#include "DocData.h"
#include "References.h"
#include "Document.h"

namespace Pdf {

using namespace Exception;

pdfReferences::pdfReferences(void)
{
	m_doc.reset( new CDocData() );
}

//pdfReferences::pdfReferences(pdfDocument* doc) : m_doc(doc)
//{
//	if( !m_doc )
//		throw pdfDocUndefinedException();
//}

pdfReferences::pdfReferences(const pdfReferences& src)
{
	operator=(src);
}

pdfReferences::pdfReferences(const CDocData::Ptr& src)
{
	operator=(src);
}

pdfReferences::~pdfReferences(void)
{
}

pdfReferences& pdfReferences::operator=(const pdfReferences& src) 
{
	if( m_doc != src.m_doc )
		m_doc = src.m_doc;
	if( m_doc == nullptr )
		m_doc.reset( new CDocData() );
	return *this;
}

pdfReferences& pdfReferences::operator=(const CDocData::Ptr& src) 
{
	if( m_doc != src )
		m_doc = src;
	if( m_doc == nullptr )
		m_doc.reset( new CDocData() );
	return *this;
}

pdfReference pdfReferences::operator[]( const size_t idx )
{
	CheckIsValid();

	if( idx >= m_doc->m_xref.size() ) 
		throw pdfBadIndexException();
	size_t i = 0;
	for( auto it = m_doc->m_xref.begin(); it != m_doc->m_xref.end(); ++it )
		if( i++ == idx )
			return it->second;
	return pdfReference();
}

void pdfReferences::CheckIsValid() const
{
	if( m_doc == nullptr )// || !m_doc->IsOpen() )
		throw pdfObjectUninitializedEx();
}
pdfReference pdfReferences::operator[]( const pdfObjId& id )
{
	CheckIsValid();
	auto it = m_doc->m_xref.find(id);
	if( it != m_doc->m_xref.end() )
		return it->second ;
	return pdfReference();
}

pdfReference pdfReferences::Add(pdfAtom atm, int64 iFilePos)
{
	CheckIsValid();

	//Debug.Assert(atm.Type != AtomType.Reference);
	pdfObjId id(NextFreeID());

	id.Set( 0, 0, false );

	//Debug.Assert(!dictTable.ContainsKey(id));

	bool bFound = false;
	for( auto it = m_doc->m_xref.begin(); it != m_doc->m_xref.end(); ++it )
	{
		id = it->first;
		if (it->first.IsFree())
		{
			bFound = true;
			//id.GenNum++;
			break;
		}
	}
	id.SetFree( false );
	if (!bFound)
		id.SetId( id.GetId()+1 );

	pdfDocument doc(m_doc);
	pdfReference newRef( doc, &id, &atm, iFilePos);
	m_doc->m_xref[id] = newRef; //dictTable.Add(id, newRef);
	//if (Owner.Trailer.dict.ContainsKey(Keys.Size))
	//    (Owner.Trailer[Keys.Size] as Integer).Value = dictTable.Count+1;
	//else
	//    Owner.Trailer[Keys.Size] = new Integer(dictTable.Count+1, Owner);
	return newRef;
}
pdfReference pdfReferences::Add(pdfObjId& id, pdfAtom atm, int64 iFilePos)
{
	CheckIsValid();

	//Debug.Assert(!dictTable.ContainsKey(id));

	pdfDocument doc(m_doc);
	pdfReference newRef( doc, &id, atm.IsNull() ? nullptr : &atm , iFilePos);
	m_doc->m_xref[id] = newRef; //dictTable.Add(id, newRef);
	return newRef;
}
pdfReference pdfReferences::Add(int iID, unsigned short genID, int64 iFilePos, pdfAtom atm)
{
	CheckIsValid();

	pdfObjId id(NextFreeID());

	id.Set( iID, genID, false );

	//Debug.Assert(!dictTable.ContainsKey(id));

	pdfDocument doc(m_doc);
	pdfReference newRef( doc, &id, atm.IsNull() ? nullptr : &atm , iFilePos);
	m_doc->m_xref[id] = newRef;
	return newRef;
}

pdfObjId& pdfReferences::NextFreeID()
{
	CheckIsValid();

	return m_doc->m_NextFreeID;
}

void pdfReferences::SetNextFreeID(pdfObjId& src)
{
	CheckIsValid();

	m_doc->m_NextFreeID = src;
}

pdfReference pdfReferences::Get( const size_t i )
{
	return operator[](i);
}
//	void Set( const size_t idx, const pdfAtom& atm );
size_t pdfReferences::GetCount() const
{
	CheckIsValid();
	return m_doc->m_xref.size();
}

void pdfReferences::Remove( const size_t idx )
{
	/*if( idx >= m_doc->m_xref.size() ) 
		throw pdfBadIndexException();
*/
}

void pdfReferences::Clear()
{
	CheckIsValid();
	//if( m_doc->IsOpen() )
	{
		m_doc->m_xref.clear();
		m_doc->m_NextFreeID.Set(0,0,false);
	}
}
};// namespace Pdf
