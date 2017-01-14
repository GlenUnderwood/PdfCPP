#include "stdafx.h"
#include "StdSecurity.h"
#include "Model.h"
#include "Document.h"
#include "Enum.h"
#include "Utils.h"
#include "RC4.h"
#include "MD5.h"
#include <string>

using namespace std;

namespace Pdf{
namespace Security{

byte pdfStdSecurity::m_bPwdPadding[32] = {
								0x28, 0xBF, 0x4E, 0x5E, 0x4E, 0x75, 0x8A, 0x41, 
								0x64, 0x00, 0x4E, 0x56, 0xFF, 0xFA, 0x01, 0x08,
								0x2E, 0x2E, 0x00, 0xB6, 0xD0, 0x68, 0x3E, 0x80,
								0x2F, 0x0C, 0xA9, 0xFE, 0x64, 0x53, 0x69, 0x7A };
#define PADDED_PWD_SIZE (sizeof(m_bPwdPadding)/sizeof(m_bPwdPadding[0]))

pdfStdSecurity::pdfStdSecurity(void)// : m_doc(nullptr)
{
	m_rc4 = nullptr;
	m_UserValidated = m_OwnerValidated = false;
}

pdfStdSecurity::pdfStdSecurity (const pdfDocument& doc)
{
	m_rc4 = nullptr;
	m_UserValidated = m_OwnerValidated = false;

	pdfDictionary dictTrailer( doc.Trailer() );
	if (!dictTrailer.IsNull() && dictTrailer.Has( "Encrypt" ) )
	{
		//m_CurrPermissions = Permissions::None;

		pdfDictionary enc( dictTrailer[ "Encrypt" ]);

		if ( !enc.IsNull()) // otherwise throw an except!
		{
			string sFilter = enc[ "Filter" ].Str();
			int V = enc.Has( "V" ) ? enc[ "V" ].Int() : 0;
			if ( sFilter != "Standard" || !( V >= 1 && V <= 3 ) )
				throw pdfUnhandledSecurityEx( sFilter );

			m_dictEnc = enc;

			//Revision = enc[ "R" ].Int;
			//m_CurrPermissions = (Permissions)( enc[ "P" ].Int );
		}
		else
			throw pdfUnhandledSecurityEx( "Invalid encryption object. Not a dictionary" );
	}
}

pdfStdSecurity::pdfStdSecurity(const pdfStdSecurity& src)
{
	m_rc4 = nullptr;
	m_UserValidated = m_OwnerValidated = false;
	operator=(src);
}

pdfStdSecurity::pdfStdSecurity(pdfStdSecurity&& src)
{
	m_rc4 = nullptr;
	operator=(src);
}

pdfStdSecurity::~pdfStdSecurity(void)
{
	if( m_rc4 != nullptr )
	{	
		delete 	m_rc4;
		m_rc4 = nullptr;
	}
}

pdfStdSecurity& pdfStdSecurity::operator=(const pdfStdSecurity& src)
{
	if( this != &src )
	{
		m_dictEnc = src.m_dictEnc;
		m_bEncKey = src.m_bEncKey;
		m_bNewID1 = src.m_bNewID1;
		m_bNewID2 = src.m_bNewID2;
		m_sOwnerPwd = src.m_sOwnerPwd;
		m_sUserPwd = src.m_sUserPwd;
		m_UserValidated = src.m_UserValidated;
		m_OwnerValidated = src.m_OwnerValidated;

		if( m_rc4 != nullptr && src.m_rc4 != nullptr )
			*m_rc4 = *src.m_rc4;
		else if( m_rc4 == nullptr && src.m_rc4 != nullptr )
			m_rc4 = new RC4( *src.m_rc4 );
		else if( m_rc4 != nullptr )
		{	
			delete 	m_rc4;
			m_rc4 = nullptr;
		}
	}
	return *this;
}

pdfStdSecurity& pdfStdSecurity::operator=(pdfStdSecurity&& src)
{
	// move without copy

	// TODO verify that this is really using move semantics otherwise need to use std::move

	m_dictEnc = src.m_dictEnc;
	m_bEncKey = src.m_bEncKey;
	m_bNewID1 = src.m_bNewID1;
	m_bNewID2 = src.m_bNewID2;
	m_sOwnerPwd = src.m_sOwnerPwd;
	m_sUserPwd = src.m_sUserPwd;
	m_UserValidated = src.m_UserValidated;
	m_OwnerValidated = src.m_OwnerValidated;
	if( m_rc4 != nullptr )
	{	
		delete 	m_rc4;
		m_rc4 = nullptr;
	}
	m_rc4 = src.m_rc4;
	/*if( m_md5 != nullptr )
	{	
		delete 	m_md5;
		m_md5 = nullptr;
	}
	m_md5 = src.m_md5;*/

	// now, reset the other guy's value
	src.m_rc4 = nullptr;
	//src.m_md5 = nullptr;
	src.m_dictEnc = pdfAtom();
	src.m_bEncKey.clear();
	src.m_bNewID1.clear();
	src.m_bNewID2.clear();
	src.m_sOwnerPwd.clear();
	src.m_sUserPwd.clear();
	src.m_UserValidated = false;
	src.m_OwnerValidated = false;

	return *this;
}

/*static*/ pdfStdSecurity pdfStdSecurity::NewSecurity( pdfDocument& doc, const char* sUser, const char* sOwner, Permissions Perms, bool b128Bit )
{
	pdfStdSecurity sec( doc );
	//sec.Owner = doc;

	unsigned int perms = (unsigned int)Perms;
	perms &= ~Permissions::Reserved1_2;// must be zero here;
	perms |= Permissions::Reserved7_8 | Permissions::Reserved13_32;// must be 1 here;

	//m_CurrPermissions = perms;
	int R = !b128Bit ? 2 : 3;

	sec.m_dictEnc = doc.CreateDict( true );

	sec.m_dictEnc.SetName( "Filter", "Standard" );//[ "Filter" ] = new pdfString( "Standard", Owner );
	sec.m_dictEnc.SetNumber( "V", b128Bit ? 2 : 1 );
	sec.m_dictEnc.SetNumber( "Length", b128Bit ? 128 : 40 );
	sec.m_dictEnc.SetNumber( "R", R );
	sec.m_dictEnc.SetNumber( "P", (int)perms );

	// 
	//sec.m_bNewID1 = Guid.NewGuid().ToByteArray();
	//sec.m_bNewID2 = Guid.NewGuid().ToByteArray();


	//doc.m_bEncrypted = true;
	vector<byte> bTmpKey = sec.CalculateOwnerKey( sOwner, sUser, R );
	pdfString psOwner( doc, &bTmpKey[0], bTmpKey.size(), StringEncoding::ByteAsChars );
	psOwner.SetEncrypted( false );
	sec.m_dictEnc.Set( "O", psOwner );

	bTmpKey = sec.CalculateUserKey( sUser, R );
	//bTmpKey = (byte[])m_bUserKey.Clone();
	pdfString psUser( doc, &bTmpKey[0], bTmpKey.size(), StringEncoding::ByteAsChars );
	psUser.SetEncrypted( false );
	sec.m_dictEnc.Set( "U", psUser );

	sec.m_UserValidated = true;
	sec.m_OwnerValidated = true;
	if( sUser != nullptr && sUser[0]!='\0' )
		sec.m_sUserPwd = sUser;
	if( sOwner != nullptr && sOwner[0]!='\0' )
		sec.m_sOwnerPwd = sOwner;
	return sec;
}

pdfDocument pdfStdSecurity::GetDocument() const
{
	return m_dictEnc.GetDocument();
}

#pragma region Permissions
Permissions pdfStdSecurity::GetPermissions() const
{
	Permissions perms = Permissions::All;
	if ( !m_dictEnc.IsNull() && m_dictEnc.Has("P") )
		perms = (Permissions) m_dictEnc.Get( "P" ).Int();
	return perms;
}

/// <summary>
/// 
/// </summary>
bool pdfStdSecurity::CanPrint() const
{
	if ( m_OwnerValidated )
		return true;
	Permissions perms = GetPermissions();
	bool b = ( perms & Permissions::Print ) != 0;
	return b;
}
/// <summary>
/// 
/// </summary>
bool pdfStdSecurity::CanPrintHighQuality() const
{
	if ( m_OwnerValidated )
		return true;
	Permissions perms = GetPermissions();
	bool b = ( perms & Permissions::Print ) != 0;
	if ( b && GetRevision() >= 3 )
	{
		b = ( perms & Permissions::PrintHighQuality ) != 0;
	}
	return b;
}
/// <summary>
/// 4 Modify the contents of the document by operations other than
/// those controlled by bits 6, 9, and 11.
/// </summary>
bool pdfStdSecurity::CanEdit() const
{
	if ( m_OwnerValidated )
		return true;
	Permissions perms = GetPermissions();
	bool b = ( perms & Permissions::EditDoc ) != 0;
	return b;
}
/// <summary>
/// 5 (Revision 2) Copy or otherwise extract text and graphics from the
/// document, including extracting text and graphics (in support of ac-
/// cessibility to users with disabilities or for other purposes).
/// (Revision 3 or greater) Copy or otherwise extract text and graphics
/// from the document by operations other than that controlled by bit 10.
/// </summary>
bool pdfStdSecurity::CanCopyContent() const
{
	if ( m_OwnerValidated )
		return true;
	Permissions perms = GetPermissions();
	bool b = ( perms & Permissions::CopyContent ) != 0;
	return b;

}
/// <summary>
/// 6 Add or modify text annotations, fill in interactive form fields, and,
/// if bit 4 is also set, create or modify interactive form fields (including
/// signature fields).
/// </summary>
bool pdfStdSecurity::CanAddEditTextAnnotations() const
{
	if ( m_OwnerValidated )
		return true;
	Permissions perms = GetPermissions();
	bool b = ( perms & Permissions::TextAnnotations ) != 0;
	return b;
}
/// <summary>
/// 9 (Revision 3 or greater) Fill in existing interactive form fields (includ-
/// ing signature fields), even if bit 6 is clear.
/// </summary>
bool pdfStdSecurity::CanFillForms() const
{
	if ( m_OwnerValidated )
		return true;
	Permissions perms = GetPermissions();
	bool b = ( perms & Permissions::FillForms ) != 0;
	return b;
}
/// <summary>
/// 10 (Revision 3 or greater) Extract text and graphics (in support of ac-
/// cessibility to users with disabilities or for other purposes).
/// </summary>
bool pdfStdSecurity::CanCopyAccessibleContent() const
{
	if ( m_OwnerValidated )
		return true;
	Permissions perms = GetPermissions();
	bool b = ( perms & Permissions::AccessibilityCopyContent ) != 0;
	return b;
}
/// <summary>
/// 11 (Revision 3 or greater) Assemble the document (insert, rotate, or de-
/// lete pages and create bookmarks or thumbnail images), even if bit 4
/// is clear.
/// </summary>
bool pdfStdSecurity::CanAssembleDoc() const
{
	if ( m_OwnerValidated )
		return true;
	Permissions perms = GetPermissions();
	bool b = ( perms & Permissions::DocAssembly ) != 0;
	return b;
}
/// <summary>
/// Unique to us, doesn't correspond to any perms bits.
/// </summary>
bool pdfStdSecurity::CanChangePerms() const
{
	return m_OwnerValidated;
}
#pragma endregion

short pdfStdSecurity::GetRevision() const
{
	short R = 3;
	if ( !m_dictEnc.IsNull() && m_dictEnc.Has("R") )
		R = m_dictEnc.Get( "R" ).Int();
	return R;
}

const char* pdfStdSecurity::GetOwnerPwd() const
{
	return m_sOwnerPwd.c_str();
}

const char* pdfStdSecurity::GetUserPwd() const
{
	return m_sUserPwd.c_str();
}

bool pdfStdSecurity::GetUserValidated() const
{
	return m_UserValidated;
}

bool pdfStdSecurity::GetOwnerValidated() const
{
	return m_OwnerValidated;
}

PasswordType pdfStdSecurity::Validate( const char* sPwdIn )
{
	//if ( sPwd.length() == 0 )
	//	sPwd = "";

	pdfDocument doc( GetDocument() );

	if ( doc.IsOpen() && doc.IsEncrypted() )
	{
		string sPwd( sPwdIn == nullptr ? "" : sPwdIn );
		if ( IsUserPwd( sPwd.c_str() ) )
		{
			// Now check do see if the owner password is blank.
			// Do this by checking the passed-in password to see if it passes the owner test.
			// remember to store teh encryption key first, then restore, regardless
			std::vector<byte> b = m_bEncKey;

			if ( IsOwnerPwd( sPwd.c_str() ) )// if there's no owner password then this will open everthing up
			{
				// this val is set in the func, so set it to blank.
				m_sOwnerPwd.clear();
			}
			m_bEncKey = b;

			return PasswordType::User;
		}

		if ( IsOwnerPwd( sPwd.c_str() ) )
			return PasswordType::Owner;

		return PasswordType::Invalid;
	}
	return PasswordType::None;
}

bool pdfStdSecurity::EncryptObject( const pdfAtom& atm, CSharedBuff& bBytes )
{
	return EndecryptObject( atm, bBytes, false );
}

bool pdfStdSecurity::DecryptObject( const pdfAtom& atm, CSharedBuff& bBytes )
{
	return EndecryptObject( atm, bBytes, true );
}

bool pdfStdSecurity::EndecryptObject( const pdfAtom& atm, /*in/out*/ CSharedBuff& bBytes, bool bDecrypting )
{
	if ( !GetDocument().IsOpen() )
		return false;

	if ( bBytes.GetLength() == 0 )
		return true;

	//if ( bMainKey == null )
	//	throw pdfUnauthorizedEx();

	if ( atm.GetType() != AtomType::Stream && atm.GetType() != AtomType::String)
		throw pdfException( "Can only encrypt/decrypt strings and streams." );

	// Step 1. Obtain the object number and generation number from the object identifier of the
	// string or stream to be encrypted (see Section 3.2.9, 'Indirect Objects'). If the
	// string is a direct object, use the identifier of the indirect object containing it.
	pdfObjId id;

	if ( atm.GetId().IsValid() )
	{
		id = atm.GetId();
	}
	else
	{
		// if this object doesn't have an ID it's because it is contained with another object (Dictionary or stream)
		pdfAtom atmParent(atm.GetParent());
		id = atmParent.GetId();
		while ( !atm.GetId().IsValid() && 
				atmParent.GetType() != AtomType::Null )//atmParent != nullptr )
		{
			pdfAtom ref(atmParent.GetRef());
			if ( ref.GetType() != AtomType::Null )//atmParent.GetRef() != nullptr )
				id = ref.GetId();
			else
				id = atmParent.GetId();
			atmParent = atmParent.GetParent();
		}
	}

	if ( id.IsValid() )
	{
		std::vector<byte>& bMainEncKey = m_bEncKey;

		// Step 2. Treating the object number and generation number as binary integers, extend the
		// original n-byte encryption key to n + 5 bytes by appending the low-order 3 bytes
		// of the object number and the low-order 2 bytes of the generation number in that
		// order, low-order byte first. (n is 5 unless the value of V in the encryption dictio-
		// nary is greater than 1, in which case n is the value of Length divided by 8.)
		// If using the AES algorithm, extend the encryption key an additional 4 bytes by
		// adding the value "sAlT", which corresponds to the hexadecimal values 0x73, 0x41,
		// 0x6C, 0x54. (This addition is done for backward compatibility and is not intended
		// to provide additional security.)
		// 3. Initialize the MD5 hash function and pass the result of step 2 as input to this function.
		std::vector<byte> bObjEncKey = CalculateObjKey( bMainEncKey, id );
		if ( bObjEncKey.empty() )
			return false;

		//Step 4. Use the first (n + 5) bytes, up to a maximum of 16, of the output from the MD5
		//hash as the key for the RC4 or AES symmetric key algorithms, along with the
		//string or stream data to be encrypted.
		//If using the AES algorithm, the Cipher Block Chaining (CBC) mode, which re-
		//quires an initialization vector, is used. The block size parameter is set to 16 bytes,
		//and the initialization vector is a 16-byte random number that is stored as the first
		//16 bytes of the encrypted stream or string.
		int keySize = bMainEncKey.size() + 5;
		if ( keySize > 16 )
			keySize = 16;

		//
		if( m_rc4 == nullptr )
			m_rc4 = new RC4();
		m_rc4->Transform( &bObjEncKey[0], keySize, bBytes.get(), bBytes.GetLength() );

		//atm.Encrypted = !atm.Encrypted;
		return true;
	}
	return false;
}

void pdfStdSecurity::GetNewID1(vector<byte>& newId)
{
	newId = m_bNewID1;
}

void pdfStdSecurity::GetNewID2(vector<byte>& newId)
{
	newId = m_bNewID2;
}

pdfDictionary pdfStdSecurity::EncryptionDictionary()
{
	return m_dictEnc;
}


/////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// Algorithm 3.6 Authenticating the user password
/// </summary>
/// <param name="sPwd"></param>
/// <returns></returns>
bool pdfStdSecurity::IsUserPwd( const char* sPwd )
{
	if( m_UserValidated )
		return true;
	pdfDocument doc( GetDocument() );

	if ( doc.IsOpen() && doc.IsEncrypted() )
	{
		if ( !m_dictEnc.IsNull() ) // TODO otherwise throw an except!
		{
			string sFilter = m_dictEnc[ "Filter" ].Str();
			int V = m_dictEnc[ "V" ].Int();
			if ( sFilter != "Standard" || !( V >= 1 && V <= 3 ) )
				throw pdfUnhandledSecurityEx( sFilter );

			pdfString strU( m_dictEnc[ "U" ] );
			pdfString strO( m_dictEnc[ "O" ] );
			int R = GetRevision(), iKeyLen = R >= 3 ? 16 : 5;
			int iLen = m_dictEnc[ "Length" ].Int();
			iLen/=8;
			// Step 1. Perform all but the last step of Algorithm 3.4 (Revision 2) or Algorithm 3.5 (Revi-
			// sion 3 or greater) using the supplied password string.
			vector<byte> bUserKey = CalculateUserKey( sPwd, R );

			// Step 2. If the result of step 1 is equal to the value of the encryption dictionary's U entry
			//(comparing on the first 16 bytes in the case of Revision 3 or greater), the password
			//supplied is the correct user password. The key obtained in step 1 (that is, in the
			//first step of Algorithm 3.4 or 3.5) can be used to decrypt the document using Al-
			//gorithm 3.1 on page 119.
			if ( memcmp( &bUserKey[0], strU.GetStringBytes(true).get(), iKeyLen ) == 0 )
			{
				/*m_bOwnerKey = (byte[])strO.m_HexBytes.Clone();*/
				m_sUserPwd = sPwd;

				m_UserValidated = true;
				return true;
			}
		}
	}
	return false;
}

/// <summary>
/// Algorithm 3.7 Authenticating the owner password
/// </summary>
/// <param name="sPwd"></param>
/// <returns></returns>
bool pdfStdSecurity::IsOwnerPwd( const char* sPwd )
{
	if ( m_OwnerValidated )
		return true;
	pdfDocument doc( GetDocument() );
	if ( doc.IsOpen() && doc.IsEncrypted() && !m_dictEnc.IsNull() )
	{
		string sFilter = m_dictEnc[ "Filter" ].Str();
		int V = m_dictEnc.Has( "V" ) ? m_dictEnc[ "V" ].Int() : 0;
		if ( sFilter != "Standard" || !( V >= 1 && V <= 3 ) )
			throw pdfUnhandledSecurityEx( sFilter );// Exception( string.Format( "Unknown Encryption: '{0}'", sFilter ) );

		if( m_rc4 == nullptr )
			m_rc4 = new RC4();

		pdfString strU( m_dictEnc[ "U" ] );
		pdfString strO( m_dictEnc[ "O" ] );
		int R = GetRevision(), iKeyLen = R >= 3 ? 16 : 5;

		//////////
		// Step 1. Compute an encryption key from the supplied password string, as described in
		// steps 1 to 4 of Algorithm 3.3.
		vector<byte> bHash;
		vector<byte> bPaddedUserPwd = CalculateOwnerKey( sPwd, R, &bHash ); // steps 1-4 which means bHash will serve as our encryption key

		//Step 2. (Revision 3 or greater) Do the following 20 times: Decrypt the value of the encryp-
		// tion dictionary's O entry (first iteration) or the output from the previous iteration
		// (all subsequent iterations), using an RC4 encryption function with a different en-
		// cryption key at each iteration. The key is generated by taking the original key (ob-
		// tained in step 1) and performing an XOR (exclusive or) operation between each
		// byte of the key and the single-byte value of the iteration counter (from 19 to 0).
		CSharedBuff buffShared( strO.GetStringBytes(true) );
		vector<byte> bOPwd( buffShared.GetLength() );// = (byte[])strO.m_HexBytes.Clone();
		memcpy( &bOPwd[0], buffShared.get(), min( buffShared.GetLength(), (size_t)32 ) );
		//
		if ( R >= 3 )
		{
			byte bXORKey[16] = "";
			for ( int i = 19; i >= 0; i-- )
			{
				// Create an XOR version of the key from step 4
				for ( int j = 0; j < 16; ++j )
					bXORKey[ j ] = (byte)( bHash[ j ] ^ i );
				// and encrypt result of step 6
				m_rc4->Transform( bXORKey, 16, &bOPwd[0], bOPwd.size(), &bOPwd[0] );
			}
		}
		else
		{
			// Step 2. (Revision 2 only) Decrypt the value of the encryption dictionary's O entry, using
			// an RC4 encryption function with the encryption key computed in step 1.
			m_rc4->Transform( &bHash[0], iKeyLen, &bOPwd[0], bOPwd.size(), &bOPwd[0] );
		}
		//
		//string s = UnpadPassword( bOPwd );

		// Step 3. The result of step 2 purports to be the user password. Authenticate this user pass-
		// word using Algorithm 3.6. If it is correct, the password supplied is the correct
		// owner password.
		vector<byte> bUserKey = CalculateUserKey( bOPwd, R );//Algorithm 3.6
		if ( memcmp( &bUserKey[0], strU.GetStringBytes(true).get(), iKeyLen ) == 0 )//		if ( Utils.ArraysMatch( bUserKey, strU.m_HexBytes, iKeyLen ) )
		{
			//update our own copy
			/*m_bOwnerKey = (byte[])strO.m_HexBytes.Clone();*/
			m_sUserPwd = UnpadPassword( bOPwd );
			m_sOwnerPwd = sPwd;

			//m_CurrPermissions =  (Permissions)(enc[ "P" ].Int());
			//Revision = R;
			m_OwnerValidated = m_UserValidated = true;
						
			return true;
		}
	}
	return false;
}

#pragma region Internal Key Calculators
/// <summary>
/// Algorithm 3.1 Encryption of data using the RC4 or AES algorithms
/// Steps 2 - 3
/// </summary>
/// <param name="id"></param>
/// <returns></returns>
std::vector<byte> pdfStdSecurity::CalculateObjKey( std::vector<byte>& bEncKey, const pdfObjId& id )
{
	if ( bEncKey.size() == 0 )
		throw pdfUnauthorizedEx();
	MD5 md5;

	// Step 2. Treating the object number and generation number as binary integers, extend the
	// original n-byte encryption key to n + 5 bytes by appending the low-order 3 bytes
	// of the object number and the low-order 2 bytes of the generation number in that
	// order, low-order byte first. (n is 5 unless the value of V in the encryption dictio-
	// nary is greater than 1, in which case n is the value of Length divided by 8.)
	// If using the AES algorithm, extend the encryption key an additional 4 bytes by
	// adding the value "sAlT", which corresponds to the hexadecimal values 0x73, 0x41,
	// 0x6C, 0x54. (This addition is done for backward compatibility and is not intended
	// to provide additional security.)
	byte bID[ 5 ] = { (byte)id.GetId(), (byte)( id.GetId() >> 8 ), (byte)( id.GetId() >> 16 ), (byte)id.GetGenNum(), (byte)( id.GetGenNum() >> 8 ) };

	// 3. Initialize the MD5 hash function and pass the result of step 2 as input to this function.
	md5.update( bEncKey );
	md5.update( bID, 5 );
	md5.finalize();

	vector<byte> bKey = md5.bytedigest();

	return bKey;
}

/// <summary>
/// Algorithm 3.3 Computing the encryption dictionary's O (owner password) value
/// Steps 1 through 4
/// </summary>
/// <param name="sOwnerPwd"></param>
/// <param name="iRevision"></param>
/// <returns></returns>
std::vector<byte> pdfStdSecurity::CalculateOwnerKey( const char* sOwnerPwd, int iRevision,std::vector<byte>* outHash )
{
	int iRC4Keylen = 5;
	std::vector<byte> bOwnerKey( 32 );

	if ( m_rc4 == nullptr )
		m_rc4 = new RC4();

	//  Step 1. Pad the password
	std::vector<byte> bPaddedOwnerPwd = PadPassword( sOwnerPwd );
//string xxxxx = UnpadPassword( bPaddedOwnerPwd );

	// Step 2.  Pass the result as input to the MD5 hash

	std::vector<byte> bHash = ComputeHash( bPaddedOwnerPwd );

	// Step 3. (Revision 3 or greater) Hash the padded owner 50 times - For 128-bit keys only
	if ( iRevision >= 3 )
	{
		iRC4Keylen = 16;// ought to be Length/8 - which is normally 16;

		for ( int i = 0; i < 50; i++ )
			bHash = ComputeHash( bHash );
	}

	// Step 4. Encrypt the padded owner pwd using the result from the previous Step 2 as a key
	m_rc4->Transform( &bHash[0], iRC4Keylen, &bPaddedOwnerPwd[0], bPaddedOwnerPwd.size(), &bOwnerKey[0] );

	if( outHash )
		*outHash = std::move( bHash );

	return bOwnerKey;//should now be == to the padded user password.
}

/// <summary>
/// Algorithm 3.3 Computing the encryption dictionary's O (owner password) value
/// Steps 5 through 7
/// </summary>
/// <param name="sOwnerPwd"></param>
/// <param name="sUserPwd"></param>
/// <param name="iRevision"></param>
/// <returns></returns>
std::vector<byte> pdfStdSecurity::CalculateOwnerKey( const char* sOwnerPwd, const char* sUserPwd, int iRevision )
{
	std::vector<byte> bOwnerKey( 32 );

	// Steps 1-4
	std::vector<byte> bPaddedOwnerPwd = PadPassword( sOwnerPwd );//doing this here again
	std::vector<byte> bHash;
	CalculateOwnerKey( sOwnerPwd, iRevision, &bHash );


	//  Step 5. Pad the USER password
	std::vector<byte> bPaddedUserPwd = PadPassword( sUserPwd );

	//  Step 6. Encrypt the padded user password using the owner from step 4 as a key
	m_rc4->Transform( &bHash[0], bHash.size(), &bPaddedOwnerPwd[0], bPaddedUserPwd.size(), &bOwnerKey[0] );

	// Step 7. - 128 bit only - Do the following 19 times: Take the output from the 
	// previous invocation of the RC4 function and pass it as input to a new invocation of the
	// function; use an encryption key generated by taking each byte of the encryption
	// key obtained in step 4 and performing an XOR (exclusive or) operation between
	// that byte and the single-byte value of the iteration counter (from 1 to 19).
	if ( iRevision >= 3 )
	{
		byte bXORKey[16] = {0};
		for ( int i = 1; i <= 19; i++ )
		{
			// Create an XOR version of the key from step 4
			for ( int j = 0; j < 16; ++j )
				bXORKey[ j ] = (byte)( bHash[ j ] ^ i );
			// and encrypt result of step 6
			m_rc4->Transform( bXORKey, 16, &bOwnerKey[0], bOwnerKey.size(), &bOwnerKey[0] );
		}
	}

	return bOwnerKey;
}

/// <summary>
/// Algorithm 3.2 Computing an encryption key. Used for encrypting/decrypting data in the file.
/// </summary>
/// <param name="sUserPwd"></param>
/// <param name="iRevision"></param>
void pdfStdSecurity::CalculateEncryptionKey( const char* sUserPwd, int iRevision )
{
	std::vector<byte> vecPwd(PadPassword( sUserPwd ));
	CalculateEncryptionKey( vecPwd, iRevision );
}

/// <summary>
/// Algorithm 3.2 Computing an encryption key. Used for encrypting/decrypting data in the file.
/// </summary>
/// <param name="sUserPwd"></param>
/// <param name="iRevision"></param>
void pdfStdSecurity::CalculateEncryptionKey( std::vector<byte>& bPaddedUserPwd, int iRevision )
{
	if ( !GetDocument().IsOpen() )
		return;

	pdfString strO(m_dictEnc[ "O" ]);
	MD5 md5;

	m_bEncKey.resize( iRevision >= 3 ? 16 : 5 );

	if ( m_rc4 == nullptr )
		m_rc4 = new RC4();

	//  Step 1. Pad the USER password
	//byte[] bPaddedUserPwd = PadPassword( sUserPwd );

	// Step 2. Initialize the MD5 hash function and pass the result of step 1 as input to this function.
	md5.update( bPaddedUserPwd );
//	md5.TransformBlock( bPaddedUserPwd, 0, bPaddedUserPwd.Length, bPaddedUserPwd, 0 );

	// Step 3. Pass the value of the encryption dictionary's O entry to the MD5 hash function.  (Algorithm 3.3 shows how the O value is computed.)
	CSharedBuff buffShared( strO.GetStringBytes(true) );
	vector<byte> bTmp( buffShared.GetLength() );// = (byte[])strO.m_HexBytes.Clone();
	memcpy( &bTmp[0], buffShared.get(),bTmp.size() );
	//make a copy so the doesn't get changed.
	vector<byte>  bOEntry = bTmp;
	md5.update( bOEntry );
//	md5.TransformBlock( bOEntry, 0, bOEntry.Length, bOEntry, 0 );

	// Step 4. Treat the value of the P entry as an unsigned 4-byte integer and pass these bytes to
	// the MD5 hash function, low-order byte first.
	int iPerms = (int)GetPermissions();
	bTmp.resize(4);
	bTmp[0] = (byte)( iPerms & 0xFF ); bTmp[1] = (byte)( ( iPerms & 0xFF00 ) >> 8 ); bTmp[2] = (byte)( ( iPerms & 0xFF0000 ) >> 16 ); bTmp[3] = (byte)( ( iPerms & 0xFF000000 ) >> 24 ) ;
	md5.update( bTmp );


	// Step 5. Pass the first element of the file's file identifier array (the value of the ID entry in
	// the document's trailer dictionary; see Table 3.13 on page 97) to the MD5 hash
	// function. (See implementation note 26 in Appendix H.)
	vector<byte> bDocID1 = m_bNewID1.size()>0 ? m_bNewID1 : GetDocument().GetID1();// GuidID1.ToByteArray();
	md5.update( &bDocID1[0], bDocID1.size() );
	if ( iRevision >= 4 )
	{
		// Step 6. (Revision 4 or greater) If document metadata is not being encrypted, pass 4 bytes
		// with the value 0xFFFFFFFF to the MD5 hash function.
		//bTmp = new byte[4] { 0xFF, 0xFF, 0xFF, 0xFF };
		bTmp[0] = bTmp[1] = bTmp[2] = bTmp[3] = 0xFF;
		md5.update( bTmp );
	}

	// Step 7. Finish the hash.
	md5.finalize();
	vector<byte> bHash = md5.bytedigest();

	// Step 8. (Revision 3 or greater) Do the following 50 times: Take the output from the previ-
	// ous MD5 hash and pass the first n bytes of the output as input into a new MD5
	// hash, where n is the number of bytes of the encryption key as defined by the value
	// of the encryption dictionary's Length entry.
	// Step 9. Set the encryption key to the first n bytes of the output from the final MD5 hash,
	// where n is always 5 for revision 2 but, for revision 3 or greater, depends on the val-
	// ue of the encryption dictionary's Length entry.	
	if ( iRevision >= 3 )
	{
		for ( int i = 0; i < 50; i++ )
		{
			bHash = ComputeHash( bHash );
		}
	}

	// Step 9. Set the encryption key to the first n bytes of the output from the final MD5 hash,
	// where n is always 5 for revision 2 but, for revision 3 or greater, depends on the val-
	// ue of the encryption dictionary's Length entry.
	memcpy( &m_bEncKey[0], &bHash[0], m_bEncKey.size() );
}

/// <summary>
/// Algorithm 3.4/3.5 Computing the encryption dictionary's U (user password) value
/// </summary>
/// <param name="sUserPwd"></param>
/// <param name="iRevision"></param>
std::vector<byte> pdfStdSecurity::CalculateUserKey( const char* sUserPwd, int iRevision )
{
	std::vector<byte> vecPwd(PadPassword( sUserPwd ));
	return CalculateUserKey( vecPwd, iRevision );
}

/// <summary>
/// Algorithm 3.4/3.5 Computing the encryption dictionary's U (user password) value
/// </summary>
/// <param name="sUserPwd"></param>
/// <param name="iRevision"></param>
std::vector<byte> pdfStdSecurity::CalculateUserKey( std::vector<byte>& bPaddedUserPwd, int iRevision )
{
	std::vector<byte> bUserKey( 32 );

	// Step 1. Create an encryption key based on the user password string, as described in Algorithm 3.2.
	CalculateEncryptionKey( bPaddedUserPwd/*sUserPwd*/, iRevision );

	if ( iRevision < 3 )
	{
		// Step 2. Encrypt the 32-byte padding string shown in step 1 of Algorithm 3.2, using an RC4 encryption function with the encryption key from the preceding step.
		// GU - Although the above isn't clear, the string here is not meant to be a padded input password, but the actual string that holds the padding chars
		//byte[] bPaddedUserPwd = PadPassword( sUserPwd );
		m_rc4->Transform( &m_bEncKey[0], m_bEncKey.size(), &m_bPwdPadding[0], 5/*m_bPwdPadding.Length*/, &bUserKey[0] );
	}
	else 
	{
		MD5 md5;
		// Step 2. Initialize the MD5 hash function and pass the 32-byte padding string shown in step 1 of Algorithm 3.2 as input to this function.
		// GU - Although the above isn't clear, the string here is not meant to be a padded input password, but the actual string that holds the padding chars
		//byte[] bPaddedUserPwd = PadPassword( sUserPwd );
		//md5.init();
		md5.update( m_bPwdPadding, PADDED_PWD_SIZE );

		// Step 3. Pass the first element of the file's file identifier array (the value of the ID entry in
		// the document's trailer dictionary; see Table 3.13 on page 97) to the MD5 hash
		// function. (See implementation note 26 in Appendix H.)
		vector<byte> bDocID1 = m_bNewID1.size()>0 ? m_bNewID1 : GetDocument().GetID1();// GuidID1.ToByteArray();
		md5.update( bDocID1 );
		md5.finalize();
		vector<byte> bHash = md5.bytedigest();

		// Step 4. Encrypt the 16-byte result of the hash, using an RC4 encryption function with theencryption key from step 1.
		m_rc4->Transform( &m_bEncKey[0], m_bEncKey.size(), &bHash[0], bHash.size(), &bUserKey[0] );

		// Step5. Do the following 19 times: Take the output from the previous invocation of the
		// RC4 function and pass it as input to a new invocation of the function; use an en-
		// cryption key generated by taking each byte of the original encryption key (ob-
		// tained in step 1) and performing an XOR (exclusive or) operation between that
		// byte and the single-byte value of the iteration counter (from 1 to 19).
		byte bXORKey[16] = {0};
		for ( int i = 1; i <= 19; i++ )
		{
			// Create an XOR version of the key from step 4
			for ( int j = 0; j < 16; ++j )
				bXORKey[ j ] = (byte)( m_bEncKey[ j ] ^ i );
			// and encrypt result of step 6
			m_rc4->Transform( bXORKey, 16, &bUserKey[0], bUserKey.size(), &bUserKey[0] );
		}

		// Step 6. Append 16 bytes of arbitrary padding to the output from the final invocation of
		// the RC4 function and store the 32-byte result as the value of the U entry in the en-
		// cryption dictionary.
		/*for ( byte i = 16; i < 32; i++ )
		{
			bUserKey[ i ] = 0;
		}*/
		memset( &bUserKey[16],0,16 );
	}

	return bUserKey;
}

std::vector<byte> pdfStdSecurity::PadPassword( const char* sPwd )
{
	std::vector<byte> bPaddedPwd( 32 );
	std::vector<byte> bKeyBytes;
	if ( sPwd && *sPwd )
	{
		bKeyBytes.resize( strlen(sPwd) );
		memcpy( &bKeyBytes[0], sPwd, bKeyBytes.size() );
		//bKeyBytes = Encoding.ASCII.GetBytes( sPwd );
		//		sVal.assign( (char*)atmThis.m_data->m_hexData.get(),atmThis.m_data->m_hexData.GetLength() );
	}
	int iLen = 0;
	if ( bKeyBytes.size() > 0 )
	{
		iLen = min( bKeyBytes.size(), (size_t)32 );
		memcpy( &bPaddedPwd[0], &bKeyBytes[0], iLen );
	}

	//Array.Copy( m_bPwdPadding, 0, bPaddedPwd, iLen, PADDED_PWD_SIZE - iLen );
	memcpy( &bPaddedPwd[iLen], &m_bPwdPadding[0], PADDED_PWD_SIZE - iLen );
	
	return bPaddedPwd;
}

std::string pdfStdSecurity::UnpadPassword( std::vector<byte>& sPaddedPwd )
{
	size_t i = 0;
	for ( i = 0; i < sPaddedPwd.size(); i++ )
	{
		if ( memcmp( &m_bPwdPadding[0], &sPaddedPwd[i], PADDED_PWD_SIZE - i ) )
			break;
		//sb.Append( (char)
	}
	std::string sVal;
	if( i > 0 )
		sVal.assign( (char*)&sPaddedPwd[0], i );
		//return Encoding.UTF8.GetString( sPaddedPwd, 0, i );
	return sVal;
}

//std::vector<byte> pdfStdSecurity::ComputeHash( byte* pBytes, size_t iLen )
//{
//	MD5 md5;
//	md5.update( pBytes, iLen );
//	md5.finalize();
//	return md5.bytedigest();
//}

std::vector<byte> pdfStdSecurity::ComputeHash( std::vector<byte>& vecBytes )
{
	MD5 md5;
	md5.update( vecBytes );
	md5.finalize();
	return md5.bytedigest();
}
#pragma endregion

} ; //namespace Security
} ; //namespace Pdf
