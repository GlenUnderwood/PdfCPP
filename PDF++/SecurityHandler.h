#pragma once
#include "Dictionary.h"

#include "ExportDef.h"

namespace Pdf{

class pdfDocument;

namespace Security{

class PDF_API pdfSecurityHandler
{
protected:
	pdfSecurityHandler(void);
public:
	//pdfSecurityHandler(pdfDocument& doc);
	virtual ~pdfSecurityHandler(void);

	//pdfDocument* GetDocument() const throw() { return m_doc; }

	/// <summary>
	/// 
	/// </summary>
	static bool IsStandard(pdfDocument& doc);

	/// <summary>
	/// 
	/// </summary>
	//bool Changed() const { return false; };

	/// <summary>
	/// 
	/// </summary>
	virtual bool CanPrint()  const = 0;
	/// <summary>
	/// 
	/// </summary>
	virtual bool CanPrintHighQuality()  const = 0;
	/// <summary>
	/// 4 Modify the contents of the document by operations other than
	/// those controlled by bits 6, 9, and 11.
	/// </summary>
	virtual bool CanEdit()  const = 0;
	/// <summary>
	/// 5 (Revision 2) Copy or otherwise extract text and graphics from the
	/// document, including extracting text and graphics (in support of ac-
	/// cessibility to users with disabilities or for other purposes).
	/// (Revision 3 or greater) Copy or otherwise extract text and graphics
	/// from the document by operations other than that controlled by bit 10.
	/// </summary>
	virtual bool CanCopyContent() const = 0;
	/// <summary>
	/// 6 Add or modify text annotations, fill in interactive form fields, and,
	/// if bit 4 is also set, create or modify interactive form fields (including
	/// signature fields).
	/// </summary>
	virtual bool CanAddEditTextAnnotations() const = 0;
	/// <summary>
	/// 9 (Revision 3 or greater) Fill in existing interactive form fields (includ-
	/// ing signature fields), even if bit 6 is clear.
	/// </summary>
	virtual bool CanFillForms() const = 0;
	/// <summary>
	/// 10 (Revision 3 or greater) Extract text and graphics (in support of ac-
	/// cessibility to users with disabilities or for other purposes).
	/// </summary>
	virtual bool CanCopyAccessibleContent() const = 0;
	/// <summary>
	/// 11 (Revision 3 or greater) Assemble the document (insert, rotate, or de-
	/// lete pages and create bookmarks or thumbnail images), even if bit 4
	/// is clear.
	/// </summary>
	virtual bool CanAssembleDoc() const = 0;

	/// <summary>
	/// Unique to us, doesn't correspond to any perms bits.
	/// </summary>
	virtual bool CanChangePerms() const = 0;

	/// <summary>
	/// Decrypts and returns which kind of password
	/// </summary>
	/// <param name="sPwd"></param>
	/// <returns></returns>
	virtual PasswordType Validate( const char* sPwd ) = 0;

	virtual bool EncryptObject( const pdfAtom& atm, CSharedBuff& bBytes ) = 0;

	virtual bool DecryptObject( const pdfAtom& atm, CSharedBuff& bBytes ) = 0;

	// returns a copy
	virtual void GetNewID1(std::vector<byte>& newId) = 0;
	virtual void GetNewID2(std::vector<byte>& newId) = 0;

	virtual pdfDictionary EncryptionDictionary() = 0;

protected:
	//pdfDocument* m_doc;
	//CDocData::WeakPtr m_doc;
};

} ; //namespace Security
} ; //namespace Pdf
