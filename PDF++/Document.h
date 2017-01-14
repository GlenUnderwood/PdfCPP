#pragma once
#include "ExportDef.h"
#include "Page.h"
#include "NameTree.h"
#include "References.h"
#include "DocData.h"
#include "pdfException.h"

namespace Pdf{

class PDF_API pdfDocument  //: public CSerializable
{
	friend class pdfAtom;
	friend class CAtomData;
	friend class pdfString;
	friend class pdfBlob;
	friend class pdfReader;
	friend class pdfReferences;
public:
	pdfDocument(void);
	pdfDocument(const pdfDocument& src);
	pdfDocument(const CDocData::Ptr& src);// TODO keep this public?
	virtual ~pdfDocument(void);

	pdfDocument& operator=(const pdfDocument& src);
	pdfDocument& operator=(const CDocData::Ptr& src);// TODO keep this public?
	pdfDocument& operator=(const std::nullptr_t i);// specifically for disconnecting from the internal doc data - doesn't force a close

	inline bool operator==( const std::nullptr_t rhs ) const { return !IsOpen(); };
	inline bool operator!=( const std::nullptr_t rhs ) const { return IsOpen(); };

	inline bool operator!() const { return IsOpen(); };
	inline operator bool() const { return !IsOpen(); };

	static pdfDocument NewDoc();// TODO add a version with a ostream*
	//static pdfDocument* OpenDoc( const wchar_t* szFile, const char* szPwd  );
	//static pdfDocument* OpenDoc( const char* szFile, const char* szPwd  );

	// CSerializable
	//void Serialize( std::ostream& stm ) {} ;
	//void Deserialize( std::ostream& stm );
protected:
	virtual void CreateNew();// const char* szNewFile = nullptr, bool bTemp = true );
public:
	virtual void Open( const wchar_t* szFile, const char* szPwd  );
	virtual void Open( const char* szFile, const char* szPwd  );// add a version with a stream*
	virtual void Close();

	inline bool IsOpen() const		{  return m_data == nullptr ? false : m_data->IsOpen(); };
	inline bool IsEncrypted() const {  return m_data->m_bEncrypted; };
	inline bool IsDirty() const 	{  return m_data->m_bDirty; };
	inline bool IsLocked() const 	{  return m_data->m_bIsLocked; };
	inline bool IsReadOnly() const 	{  return m_data->m_bIsReadOnly; };

	inline const char* GetFileName() const 
									{  return m_data->m_filename.c_str(); };
	inline const char* GetHeaderComments() const 
									{  return m_data->m_sHeaderComments.c_str(); };
	inline void SetHeaderComments(const char* sz) 
									{  m_data->m_sHeaderComments = sz; };
	inline float GetVersion() const {  return m_data->m_fVersion; };
	inline void SetVersion(float ver)  {  m_data->m_fVersion = ver; };

	inline pdfPages Pages()			{ return pdfPages(m_data); };
	inline pdfReferences CrossReference()
									{ return pdfReferences(m_data); };
	inline pdfDictionary Trailer() const
			{
				if ( !IsOpen() )
					throw Pdf::Exception::pdfDocClosedException();
				return m_data->m_Trailer; 
			};
	inline pdfDictionary Catalog() const
			{
				return Trailer().Get("Root");
			};
	inline pdfDictionary Info() const
			{
				return Trailer().Get("Info");//uncomment the following if Get() ever throws when no key found

			};
	inline pdfNameTree Names() const
			{
				return Trailer().Get("Names");
			};

	Security::pdfSecurityHandler* GetSecurity();
	void SetSecurity(Security::pdfSecurityHandler* pNew);

	std::vector<byte> GetID1();
	std::vector<byte> GetID2();

	// IDocument
	//void AtomDataCreated(CAtomData* pNew);
	//void RemoveAtomData(CAtomData* pDel);

	pdfInt		CreateInteger(int i, bool bIndirect = false);
	pdfInt64	CreateInteger64(int64 i, bool bIndirect = false);
	pdfReal		CreateReal(float d, int iPrecision=8, bool bIndirect = false);
	pdfReal		CreateReal(double d, int iPrecision=8, bool bIndirect = false);
	pdfBool		CreateBoolean(bool Val, bool bIndirect = false);
	pdfString	CreateString(const char* Val, StringEncoding encoding, bool bIndirect = false);
	pdfString	CreateHexString( const byte* Val, size_t iSize/*, bool bTakeOwnership*/, bool bIndirect = false);
	pdfName		CreateName(const char* Val, bool bIndirect = false);
	pdfDictionary CreateDict(bool bIndirect);
	pdfBlob		CreateStream();
	pdfArray	CreateArray(const size_t iInitialCount, bool bIndirect = false);

	// TODO remove this.
	inline const CDocData* GetData() const throw() { return m_data!=nullptr ? m_data.get() : nullptr; };

protected:
	void FillInfo();
	bool GenerateIDs();
	bool ValidPDF();
	bool Parse();
	void ReadXRef();
	void ReadTrailer();
	int FindInBuff(const char* str, unsigned char* buff, size_t iBuffSize, int iStart);

	//
	CDocData::Ptr m_data;
};

};// Pdf
