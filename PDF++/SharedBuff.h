#include "ExportDef.h"
#include <memory>
#include "streams.h"
#include "ObjId.h"

#define USE_STREAMS

namespace Pdf{

// uses shared_ptr to hold onto a bytestream object.  Can grow memory either with Resize or with the ostream pointer/reference operators
class PDF_API CSharedBuff// : public std::shared_ptr<byte>
{
public:
	CSharedBuff();
	CSharedBuff(  const CSharedBuff& src  );
	CSharedBuff(  CSharedBuff&& src  );
	CSharedBuff( size_t nCount );
	virtual ~CSharedBuff( );

	CSharedBuff& operator=( const CSharedBuff& src );
	CSharedBuff& operator=( CSharedBuff&& src );

	void Resize( size_t nCount, bool bPreserve = true );

	void Clear();

#ifdef USE_STREAMS
	operator std::istream&();
	operator std::ostream&(); 
	operator std::istream*();
	operator std::ostream*(); 

	inline byte* get() const
		{return (m_pStm.get() == nullptr ? nullptr : (byte*)(m_pStm.get()->GetBuffer())); }; 
	inline size_t GetLength() const
		{return m_pStm.get() == nullptr ? 0 : m_pStm.get()->GetLength(); }; 
#else
	inline byte* get() const
		{return m_buff.get();}; 
	inline size_t GetLength() const throw()
		{return m_Length;}; 
#endif

protected:
#ifdef USE_STREAMS
	std::shared_ptr<bytestream> m_pStm;
#else
	std::shared_ptr<byte> m_buff;
	size_t m_Length;
#endif
};

};