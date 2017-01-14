#pragma once
#include <memory>
#include <string>
#include <list>
#include <map>
#include <fstream> 
#include "Dictionary.h"
#include "Pages.h"
#include "SecurityHandler.h"

namespace Pdf{

//using namespace Security;

class pdfReader;
class pdfReference;

class CDocData //: public IDocData
{
public:
	CDocData(void);
	//CDocData( std::ifstream& doc );
	virtual ~CDocData(void);

	typedef std::shared_ptr<CDocData> Ptr;
	typedef std::weak_ptr<CDocData> WeakPtr;
	typedef std::map< pdfObjId, pdfReference > CMapIdToRef;
	typedef std::list< pdfPage > CPageList;// TODO Change this! Store only weak pointers

	bool IsOpen() const;
	void Close();

	///////////////////////////////////

	// TODO Need to use istream so as to support it all but need to be able to close/is_open/etc 
	// maybe another flag or a pointer or something.
	std::fstream m_stm;
	//
	std::string m_filename;

	bool m_bIsLocked, m_bIsReadOnly, m_bDirty, m_bEncrypted, m_bTemp, m_bNew;

	std::string m_sHeaderComments;

	float m_fVersion;

	pdfReader* m_Parser;
	pdfDictionary m_Trailer;
	Pdf::Security::pdfSecurityHandler* m_pSecurity;
	Pdf::Security::pdfSecurityHandler* m_pNewSecurity;

	std::list< Ptr > m_lstChildDocs;

	// xref
	pdfObjId	m_NextFreeID;
	CMapIdToRef	m_xref;

	// Pages
	CPageList m_pgs;
};

};//namespace Pdf
