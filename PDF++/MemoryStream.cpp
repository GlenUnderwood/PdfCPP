#include "stdafx"
#include "MemoryStream.h"
#include "pdfException.h"

using namespace Pdf::Exception;

// TODO Where is this class getting used?

namespace Pdf{
namespace Utils{

CMemoryStream::CMemoryStream(void) 
{
	m_pData = nullptr;
	m_iLen = m_iAllocLen = m_iPos = 0L;
}

CMemoryStream::CMemoryStream(size_t szInitialSize)
{
	m_pData = nullptr;
	m_iLen = m_iAllocLen = m_iPos = 0L;
	Resize(szInitialSize);
}

CMemoryStream::~CMemoryStream(void)
{
	Free();
}

size_t CMemoryStream::GetLength() const  throw()
{
	if( m_pData != nullptr )
		return m_iPos;
	return 0L;
}
size_t CMemoryStream::GetAllocLength() const throw()
{
	if( m_pData != nullptr )
		return m_iAllocLen;
	return 0L;
}

size_t CMemoryStream::Resize(size_t newSize)
{
	byte* pOld = m_pData; 
	m_pData = nullptr;
	size_t iAllocLen = m_iAllocLen ; 
	size_t iPos = m_iPos;

	//
	byte* pNew = new byte[newSize];
	try{ memset( pNew, 0, newSize ); } catch(...){};

	m_pData = pNew;
	m_iAllocLen = newSize;
	m_iPos = std::min( m_iPos, newSize-1 );
	m_iLen = std::max( m_iLen, m_iPos );

	if( pOld )
	{
		//if( iPos == m_iPos )// didn't shorten
		size_t c =  std::min( m_iAllocLen, iAllocLen );
		try{ memcpy( pNew, pOld, c ); } catch(...){};

		try{ delete [] pOld; } catch(...){};
	}
	return iAllocLen;
}

void CMemoryStream::Clear()
{
	if( m_pData != nullptr )
	{
		try{ memset( m_pData, 0, m_iAllocLen ); } catch(...){};
	}
}

void CMemoryStream::Free()
{
	if( m_pData != nullptr )
	{
		m_iLen = m_iAllocLen = m_iPos = 0L;
		try{ delete [] m_pData; } catch(...){};
		m_pData = nullptr;
	}
}

byte* CMemoryStream::GetBuffer()
{ return m_pData; };

byte* CMemoryStream::Detach() 
{ 
	byte* pData = m_pData; 
	m_pData = nullptr;
	m_iLen = m_iAllocLen = m_iPos = 0L; 
	return pData;
};

size_t CMemoryStream::Pos() const
{
	if( m_pData != nullptr )
		return m_iPos;
	return 0;
}

void CMemoryStream::SetPos(size_t iNew)
{
	if( m_pData != nullptr && m_iAllocLen > 0 )
	{
		m_iPos = std::min( iNew, m_iAllocLen );
		m_iLen = std::max( m_iLen, m_iPos );
	}
}

byte CMemoryStream::ReadByte()
{
	byte c = 0;
	if( m_pData != nullptr && m_iAllocLen > 0 )
	{
		if( m_iPos < m_iAllocLen )
		{
			c=m_pData[ m_iPos++ ];
			m_iLen = std::max( m_iLen, m_iPos );
		}
	}
	return c;
}

void CMemoryStream::WriteByte(byte c)
{
//	if( m_iAllocLen == 0 )
	//	Resize( 1 << 24 );

	if( m_pData == nullptr || m_iPos >= m_iAllocLen )
		Resize( 1 << 9 );

	if( m_iPos < m_iAllocLen )
	{
		m_pData[ m_iPos++ ] = c;
		m_iLen = std::max( m_iLen, m_iPos );
	}
}

void CMemoryStream::ToArray(byte** ppNew, size_t * pnLen)
{
	*ppNew = nullptr;
	if( pnLen )
		*pnLen = 0;
	if( ppNew == nullptr || /*m_iPos*/m_iLen == 0 )
		return ;
	if( m_pData == nullptr || /*m_iPos*/m_iLen > m_iAllocLen )
		return ;

	size_t iBytes = /*m_iPos*/m_iLen;
	byte* pNew = new byte[iBytes];
	try{ memcpy( pNew, m_pData, iBytes ); } catch(...){};
	(*ppNew) = pNew;	
	if( pnLen )
		*pnLen = iBytes;
}

}; //namespace Utils
}; //namespace Pdf

