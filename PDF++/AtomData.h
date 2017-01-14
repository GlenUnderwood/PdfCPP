#pragma once
#include <memory>
#include <string>
#include <vector>
#include <map>
#include "Enum.h"
#include "ObjId.h"
#include "SharedBuff.h"

namespace Pdf{

//class pdfDocument;
class CDocData;

class CAtomData //: public IAtomData
{
public:
	CAtomData(void);
	CAtomData(const std::shared_ptr<CDocData>& doc);
	CAtomData(const std::weak_ptr<CDocData>& doc);
	CAtomData(const CAtomData& src);
	virtual ~CAtomData(void);

	typedef std::shared_ptr<CAtomData> Ptr;
	typedef std::weak_ptr<CAtomData> WeakPtr;

	CAtomData& operator=(const CAtomData& src);

	// IAtomData
	void Initialize(const std::shared_ptr<CDocData>* doc);
	//void Disconnect();

	//pdfDocument* m_doc;

	AtomType m_type;
	pdfObjId m_id;

	std::weak_ptr<CDocData> m_doc;

	bool m_bEncrypted;

	std::string m_sKey;

	// pointer to our parent. 
	WeakPtr m_pParent;

	// This is the reference object that is pointing to us (and we could be anything)
	// TODO Wonder if this can be a weak ref 
	Ptr m_ref;//pdfAtom* m_ref;

	// Reference. This is the object we point to.
	Ptr m_RefTarget;// TODO Wonder if this can be a weak ref 
	unsigned int m_iRefs;
	// Used by reference objects so we can tell where to go to load the atom data
	// but also used by stream objects to remember where the byte block starts in the stream
	int64 m_FilePos;

	// basic types
	union
	{
		bool bVal;
		int iVal;
		int64 i64Val;
		double dVal;
		//float fVal;
	} m_Val;
	byte m_iPrecision ;

	// Name/Keyword
	std::string m_strName;
	StringEncoding m_StringEncoding;
	CSharedBuff m_hexData;// TODO Combine with m_stmBuff - don't need two.
	//size_t m_nHexBytes;

	//Dictionary
	std::map<std::string, Ptr> m_dictItems; // store CData instead of pdfAtom?

	// Stream
	//bytestream m_stmBuff;
	CSharedBuff m_stmBuff;
	size_t m_nStmBytes;
	//int64 m_iStreamPos;

	// Array
	std::vector<Ptr> m_vecArray;// store CData instead of pdfAtom?
};

};//namespace Pdf
