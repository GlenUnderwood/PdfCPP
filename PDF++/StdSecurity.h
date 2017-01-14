#pragma once
#include "SecurityHandler.h"
#include "Enum.h"
#include <vector>

namespace Pdf{

class pdfDocument;

namespace Security{

class RC4;

class PDF_API pdfStdSecurity : public pdfSecurityHandler
{
	friend class pdfDocument;
protected:
	pdfStdSecurity(void);
	pdfStdSecurity(const pdfDocument& doc);
public:
	pdfStdSecurity(const pdfStdSecurity& src);
	pdfStdSecurity(const pdfSecurityHandler& src);
	pdfStdSecurity(pdfStdSecurity&& rhs);
	virtual ~pdfStdSecurity(void);

	pdfStdSecurity& operator=(const pdfStdSecurity& src);
	pdfStdSecurity& operator=(const pdfSecurityHandler& src);
	pdfStdSecurity& operator=(pdfStdSecurity&& rhs);

	static pdfStdSecurity NewSecurity( pdfDocument& doc, const char* sUser, const char* sOwner, Permissions Perms, bool b128Bit );

	pdfDocument GetDocument() const;

	Permissions GetPermissions() const;
	/// <summary>
	/// 
	/// </summary>
	bool CanPrint()  const;
	/// <summary>
	/// 
	/// </summary>
	bool CanPrintHighQuality()  const;
	/// <summary>
	/// 4 Modify the contents of the document by operations other than
	/// those controlled by bits 6, 9, and 11.
	/// </summary>
	bool CanEdit()  const;
	/// <summary>
	/// 5 (Revision 2) Copy or otherwise extract text and graphics from the
	/// document, including extracting text and graphics (in support of ac-
	/// cessibility to users with disabilities or for other purposes).
	/// (Revision 3 or greater) Copy or otherwise extract text and graphics
	/// from the document by operations other than that controlled by bit 10.
	/// </summary>
	bool CanCopyContent() const;
	/// <summary>
	/// 6 Add or modify text annotations, fill in interactive form fields, and,
	/// if bit 4 is also set, create or modify interactive form fields (including
	/// signature fields).
	/// </summary>
	bool CanAddEditTextAnnotations() const;
	/// <summary>
	/// 9 (Revision 3 or greater) Fill in existing interactive form fields (includ-
	/// ing signature fields), even if bit 6 is clear.
	/// </summary>
	bool CanFillForms() const;
	/// <summary>
	/// 10 (Revision 3 or greater) Extract text and graphics (in support of ac-
	/// cessibility to users with disabilities or for other purposes).
	/// </summary>
	bool CanCopyAccessibleContent() const;
	/// <summary>
	/// 11 (Revision 3 or greater) Assemble the document (insert, rotate, or de-
	/// lete pages and create bookmarks or thumbnail images), even if bit 4
	/// is clear.
	/// </summary>
	bool CanAssembleDoc() const;
	/// <summary>
	/// Unique to us, doesn't correspond to any perms bits.
	/// </summary>
	bool CanChangePerms() const;

	/// <summary>
	/// 
	/// </summary>
	short GetRevision() const;
	const char* GetOwnerPwd() const;
	const char* GetUserPwd() const;
	bool GetUserValidated() const;
	bool GetOwnerValidated() const;

	/// <summary>
	/// Decrypts and returns which kind of password
	/// </summary>
	/// <param name="sPwd"></param>
	/// <returns></returns>
	PasswordType Validate( const char* sPwd );

	bool EncryptObject( const pdfAtom& atm, CSharedBuff& bBytes );

	bool DecryptObject( const pdfAtom& atm, CSharedBuff& bBytes );

	// returns a copy
	void GetNewID1(std::vector<byte>& newId);
	void GetNewID2(std::vector<byte>& newId);

	pdfDictionary EncryptionDictionary();

protected:
	/// <summary>
	/// Algorithm 3.6 Authenticating the user password
	/// </summary>
	/// <param name="sPwd"></param>
	/// <returns></returns>
	bool IsUserPwd( const char* sPwd );
	/// <summary>
	/// Algorithm 3.7 Authenticating the owner password
	/// </summary>
	/// <param name="sPwd"></param>
	/// <returns></returns>
	bool IsOwnerPwd( const char* sPwd );

	#pragma region Internal Key Calculators
	/// <summary>
	/// Algorithm 3.1 Encryption of data using the RC4 or AES algorithms
	/// Steps 2 - 3
	/// </summary>
	/// <param name="id"></param>
	/// <returns></returns>
	std::vector<byte> CalculateObjKey( std::vector<byte>& bEncKey, const pdfObjId& id );

	/// <summary>
	/// Algorithm 3.3 Computing the encryption dictionary's O (owner password) value
	/// Steps 1 through 4
	/// </summary>
	/// <param name="sOwnerPwd"></param>
	/// <param name="iRevision"></param>
	/// <returns></returns>
	std::vector<byte> CalculateOwnerKey( const char* sOwnerPwd, int iRevision, std::vector<byte>* outHash );

	/// <summary>
	/// Algorithm 3.3 Computing the encryption dictionary�s O (owner password) value
	/// Steps 5 through 7
	/// </summary>
	/// <param name="sOwnerPwd"></param>
	/// <param name="sUserPwd"></param>
	/// <param name="iRevision"></param>
	/// <returns></returns>
	std::vector<byte> CalculateOwnerKey( const char* sOwnerPwd, const char* sUserPwd, int iRevision );

	/// <summary>
	/// Algorithm 3.2 Computing an encryption key. Used for encrypting/decrypting data in the file.
	/// </summary>
	/// <param name="sUserPwd"></param>
	/// <param name="iRevision"></param>
	void CalculateEncryptionKey( const char* sUserPwd, int iRevision );

	/// <summary>
	/// Algorithm 3.2 Computing an encryption key. Used for encrypting/decrypting data in the file.
	/// </summary>
	/// <param name="sUserPwd"></param>
	/// <param name="iRevision"></param>
	void CalculateEncryptionKey( std::vector<byte>& bPaddedUserPwd, int iRevision );

	/// <summary>
	/// Algorithm 3.4/3.5 Computing the encryption dictionary�s U (user password) value
	/// </summary>
	/// <param name="sUserPwd"></param>
	/// <param name="iRevision"></param>
	std::vector<byte> CalculateUserKey( const char* sUserPwd, int iRevision );

	/// <summary>
	/// Algorithm 3.4/3.5 Computing the encryption dictionary�s U (user password) value
	/// </summary>
	/// <param name="sUserPwd"></param>
	/// <param name="iRevision"></param>
	std::vector<byte> CalculateUserKey( std::vector<byte>& bPaddedUserPwd, int iRevision );

	std::vector<byte> PadPassword( const char* sPwd );
	std::string UnpadPassword( std::vector<byte>& sPaddedPwd );
	//std::vector<byte> ComputeHash( byte* pBytes, size_t iLen );
	std::vector<byte> ComputeHash( std::vector<byte>& vecBytes );

	#pragma endregion

	bool EndecryptObject( const pdfAtom& atm, /*in/out*/CSharedBuff& bBytes , bool bDecrypting );

	pdfDictionary m_dictEnc;

	std::vector<byte>	m_bEncKey,
						m_bNewID1,
						m_bNewID2;

	std::string m_sOwnerPwd,
				m_sUserPwd;

	bool		m_UserValidated, 
				m_OwnerValidated;

	RC4* m_rc4;

	static byte m_bPwdPadding[32];

};

} ; //namespace Security
} ; //namespace Pdf
