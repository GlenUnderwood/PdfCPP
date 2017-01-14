#include "stdafx.h"
#include "RC4.h"

namespace Pdf {
namespace Security {

RC4::RC4(void)
{
	// set to identity.  i.e., fill will a value of 0 to 255
	for ( int idx = 0; idx < 256; idx++ )
		m_bState[ idx ] = (byte)idx;
}

RC4::RC4( const RC4& src )
{
	operator=( src );
}

RC4::RC4( byte* pwd, size_t length )
{
	Initialize( pwd, length );
}

RC4::~RC4(void)
{
}

RC4& RC4::operator=( const RC4& src )
{
	if( this != &src )
	{
		memcpy( m_bState, src.m_bState, sizeof( m_bState) );
	}
	return *this;
}

/// <summary>
/// Part of Standard Security Encryption Algorithm 3.1 - Step 1 - Create an RC4 key based on a byte array 
/// </summary>
/// <param name="key"></param>
/// <param name="offset"></param>
/// <param name="length"></param>
/// <returns></returns>
void RC4::Initialize( byte* pwd, size_t length )
{
	int idx1 = 0, idx2 = 0;
	// set to identity.  i.e., fill will a value of 0 to 255
	for ( int idx = 0; idx < 256; idx++ )
		m_bState[ idx ] = (byte)idx;

	byte tmp;
	for ( int idx = 0; idx < 256; idx++ )
	{
		idx2 = ( pwd[ idx1  ] + m_bState[ idx ] + idx2 ) & 255;
		tmp = m_bState[ idx ];
		m_bState[ idx ] = m_bState[ idx2 ];
		m_bState[ idx2 ] = tmp;
		idx1 = ( idx1 + 1 ) % length;
	}
}

/*byte[] Encrypt( byte[] inputData, size_t length )
{
	byte[] b = new byte[ length ];
	Encrypt( inputData, length, b );
	return b;
}*/
/// <summary>
/// Part of Standard Security Encryption Algorithm 3.1 
/// </summary>
/// <param name="m_bState"></param>
/// <param name="inputData"></param>
/// <param name="offset"></param>
/// <param name="length"></param>
/// <param name="outputData"></param>
void RC4::Transform( byte* inputData, size_t length, byte* outputData )
{
	int x = 0, y = 0;
	byte b;
	for ( size_t idx = 0; idx < length; idx++ )
	{
		x = ( x + 1 ) & 255;
		y = ( m_bState[ x ] + y ) & 255;
		b = m_bState[ x ];
		m_bState[ x ] = m_bState[ y ];
		m_bState[ y ] = b;
		outputData[ idx ] = (byte)( inputData[ idx ] ^ m_bState[ ( m_bState[ x ] + m_bState[ y ] ) & 255 ] );
	}
}

// call once to encrypt, again to decrypt
void RC4::Transform( byte* bKey, int iKeyLen, byte* inputData, size_t length, byte* outputData )
{
	Initialize( bKey, iKeyLen );
	Transform( inputData, length, outputData );
}

void RC4::Transform( byte* inputData, size_t length  )
{
	Transform( inputData, length, inputData );
}

void RC4::Transform( byte* bKey, int iKeyLen, byte* inputData, size_t length  )
{
	Initialize( bKey, iKeyLen );
	Transform( inputData, length, inputData );
}

};
};