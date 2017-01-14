#include "stdafx.h"
#include "SharedBuff.h"

namespace Pdf{

#ifndef USE_STREAMS
template< typename T >
class array_deleter
{
public:
	void operator ()( T const * p)
	{ 
		delete[] p; 
	}
};
#endif

CSharedBuff::CSharedBuff()
{
#ifndef USE_STREAMS
	m_Length = 0;
#endif
};

CSharedBuff::CSharedBuff( const CSharedBuff& src )
{
#ifndef USE_STREAMS
	m_Length = 0;
#endif
	operator=(src);
};

CSharedBuff::CSharedBuff( CSharedBuff&& src )
{
#ifndef USE_STREAMS
	m_Length = 0;
#endif
	operator=(src);
};

CSharedBuff::CSharedBuff( size_t nCount ) //: std::shared_ptr<byte[]>(nCount)
{
#ifndef USE_STREAMS
	m_Length = 0;
#endif
	Resize(nCount);
};

CSharedBuff::~CSharedBuff( )
{
	Clear();
};

CSharedBuff& CSharedBuff::operator=( const CSharedBuff& src )
{
	if( this != &src )
	{
#ifdef USE_STREAMS
		m_pStm = src.m_pStm;
#else
		m_buff = src.m_buff;
		m_Length = src.m_Length;
#endif
	}
	return *this;
};

CSharedBuff& CSharedBuff::operator=( CSharedBuff&& src )
{
	if( this != &src )
	{
#ifdef USE_STREAMS
		m_pStm = src.m_pStm;
#else
		m_buff = src.m_buff;
		m_Length = src.m_Length;
#endif
		src.Clear();
	}
	return *this;
};

#ifdef USE_STREAMS
CSharedBuff::operator istream&()
{
	if( m_pStm == nullptr )
		m_pStm.reset( new bytestream() );//throw pdfException( "No stream allocated!" );

	bytestream* p = m_pStm.get();
	return *((istream*)p) ;
}
CSharedBuff::operator ostream&()
{
	if( m_pStm == nullptr )
		m_pStm.reset( new bytestream() );//throw pdfException( "No stream allocated!" );

	bytestream* p = m_pStm.get();
	return *((ostream*)p) ;
}
CSharedBuff::operator istream*()
{
	if( m_pStm == nullptr )
		m_pStm.reset( new bytestream() );//throw pdfException( "No stream allocated!" );

	bytestream* p = m_pStm.get();
	return ((istream*)p) ;
}
CSharedBuff::operator ostream*()
{
	if( m_pStm == nullptr )
		m_pStm.reset( new bytestream() );//throw pdfException( "No stream allocated!" );
	bytestream* p = m_pStm.get();
	return ((ostream*)p) ;
}

#endif

void CSharedBuff::Resize(  size_t nCount, bool bPreserve )
{
	if( nCount == 0 )
	{
		Clear();
		return;
	}
#ifdef USE_STREAMS
	if( m_pStm == nullptr )
		m_pStm.reset( new bytestream(nCount) );
	else
	{
		//m_pStm
		if( nCount == m_pStm->GetLength() )
		{
			if( !bPreserve && nCount>0 )
				memset( (void*)(m_pStm->GetBuffer()),0,nCount);
		}

		std::shared_ptr<bytestream> pTmpStm( new bytestream(nCount) );
		if( bPreserve )
		{
			nCount = Math::Min( nCount, m_pStm->GetLength() );
			memcpy( (void*)(pTmpStm->GetBuffer()), m_pStm->GetBuffer(), nCount);
		}
		m_pStm = pTmpStm;

		m_pStm->seekg( ios::beg );
		m_pStm->seekp( ios::beg );
	}
#else
	if( nCount == m_Length )
	{
		if( !bPreserve && m_Length>0 )
			memset( m_buff.get(),0,m_Length);
		return;
	}

	size_t lenOld = m_Length;

	m_Length = 0;

	std::shared_ptr<byte> buff( new byte[nCount], array_deleter<byte>() );

	if( bPreserve && m_buff.get()!=nullptr )
	{
		if( nCount > lenOld )
			memset( &buff.get()[lenOld], 0, nCount - lenOld );
		memcpy( buff.get(), m_buff.get(), min( nCount, lenOld ) );
	}
	else
		memset( buff.get(),0,nCount);

	m_buff = buff;//.reset( new byte[nCount], array_deleter<byte>() );  memset( get(),0,nCount);
	m_Length = nCount;
#endif
}

void CSharedBuff::Clear()// more of a detach, really.
{
#ifdef USE_STREAMS
	m_pStm.reset();
#else
	m_Length = 0;
	m_buff.reset();
#endif
}

};
