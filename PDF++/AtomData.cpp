#include "stdafx.h"
#include "DocData.h"
#include "AtomData.h"
#include "Document.h"

namespace Pdf{

CAtomData::CAtomData(void)// : CAtomData(nullptr)
{
	//m_doc = nullptr;
	Initialize(nullptr);
}
CAtomData::CAtomData(const std::shared_ptr<CDocData>& doc)
{
	//m_doc = nullptr;
	Initialize(&doc);
}
CAtomData::CAtomData(const std::weak_ptr<CDocData>& doc)
{
	if( doc.expired() )
		Initialize( nullptr );
	else
	{
		CDocData::Ptr docData(doc.lock());
		Initialize( &docData );
	}
}

CAtomData::CAtomData(const CAtomData& src)
{
	//m_doc = nullptr;
	Initialize(nullptr);
	operator=( src );
}

CAtomData::~CAtomData(void)
{
	try{
	Initialize(nullptr);
	}
	catch(...){}
}

CAtomData& CAtomData::operator=(const CAtomData& _src)
{
		//std::clog << "CAtomData::operator=(const CAtomData& _src)\n";
	/*if( m_doc != nullptr )
		m_doc->RemoveAtomData(this);
	try{
		Disconnect();}
	catch(...){}

	m_dictItems.clear();
	m_vecArray.clear();
	m_hexData.Clear();
	m_stmBuff.Clear();
	m_strVal.clear();
	m_wstrVal.clear();

	///
	CAtomData* src( &(const_cast<CAtomData&>(_src)) );

	//if(src->m_RefTarget)
	//	src = src->m_RefTarget.get();

	m_doc = src->m_doc;

	m_type = src->m_type;
	m_id = src->m_id;
	m_bEncrypted = src->m_bEncrypted;

	m_sKey = src->m_sKey;

	m_pParent = src->m_pParent;

	//if( src->m_ref != nullptr )
	//	m_ref.reset( new CAtomData( *(src->m_ref.get()) ) );//This is the reference object that is pointing to us (and we could be anything)
	m_ref = src->m_ref;
	
	if( m_RefTarget != nullptr )
	{
		//std::clog << "\tAbout to copy m_RefTarget\n";
		//m_RefTarget.reset( new CAtomData( *(src->m_RefTarget.get()) ) );//This is the reference object that is pointing to us (and we could be anything)
		//m_iRefs = 1;

		m_RefTarget = src->m_RefTarget;// Reference. This is the object we point to when WE are the reference obejct.
		m_iRefs = src->m_iRefs;
	}

	m_FilePos = src->m_FilePos;

	m_Val = src->m_Val;
	m_iPrecision = src->m_iPrecision;

	m_strVal = src->m_strVal;
	m_wstrVal = src->m_wstrVal;
	m_StringEncoding = src->m_StringEncoding;
///	m_hexData = src->m_hexData;

///	m_dictItems = src->m_dictItems; // store CData instead of pdfAtom?
	for( auto it = src->m_dictItems.begin(); it!= src->m_dictItems.end(); ++it )
		m_dictItems[ it->first ].reset( new CAtomData( *(it->second.get()) ) );

///	m_stmBuff = src->m_stmBuff;
///	m_nStmBytes = src->m_nStmBytes;

///	m_vecArray = src->m_vecArray;// store CData instead of pdfAtom?
	for( auto it = src->m_vecArray.begin(); it!= src->m_vecArray.end(); ++it )
		m_vecArray.push_back( Ptr(new CAtomData( *(*it) )) );

	// tell the document we exist!
	if( m_doc != nullptr )
	{
		m_doc->AtomDataCreated( this );
	}*/
	return *this;

}

void CAtomData::Initialize(const std::shared_ptr<CDocData>* doc)
{
	//if( !m_doc.expired() )
	//	m_doc.lock()->RemoveAtomData(this);
	try{
		//	Disconnect();
		m_dictItems.clear();

		m_vecArray.clear();

		m_hexData.Clear();

		m_stmBuff.Clear();	//m_stmBuff.reset();

		m_doc.reset();
	}
	catch(...){}

	//m_doc = (pdfDocument*)doc;
	if( doc != nullptr )
		m_doc = *doc;

	m_type = Null;
	m_bEncrypted = false;

	m_ref.reset();// = nullptr;
	m_Val.i64Val = 0L;
	m_iPrecision = 6;
	
	m_pParent.reset();//m_pParent = nullptr;

	// Name/Keyword
	m_strName.clear();

	//String
	m_StringEncoding = ASCII ;

	// reference.  The object we point to. Could still be in the stream if m_FilePos is a valid position.
	// loaded only when requested
	m_RefTarget = nullptr;
	m_FilePos = 0xFFFFFFFFFFFFFFFFL;
	m_iRefs	= 0;

	// Stream
	//m_iStreamPos = 0;
	//m_nStmBytes = 0;

	// tell the document we exist!
	/*if( doc != nullptr )
	{
		doc->get()->AtomDataCreated( this );
	}*/
}
/*
void CAtomData::Disconnect()
{
	// Most importantly here, we want to disconnect from pdfDocument

	// do NOT release pointers here, could trigger deletes that foul things up in pdfDocument.Close().  But, we can release memory
	m_iRefs = 0;
	//m_ref.reset();
	//m_RefTarget.reset();

	m_dictItems.clear();

	m_vecArray.clear();

	m_hexData.Clear();

	m_stmBuff.Clear();	//m_stmBuff.reset();

	m_type = Null;

	m_doc.reset();
}*/

};//namespace Pdf
