#pragma once

namespace Pdf {
namespace Security {

class RC4
{
public:
	RC4(void);
	RC4( const RC4& src );
	RC4( byte* pwd, size_t length );
	virtual ~RC4(void);

	RC4& operator=( const RC4& src );

	/// <summary>
	/// Part of Standard Security Encryption Algorithm 3.1 - Step 1 - Create an RC4 key based on a byte array 
	/// </summary>
	/// <param name="key"></param>
	/// <param name="offset"></param>
	/// <param name="length"></param>
	/// <returns></returns>
	void Initialize( byte* pwd, size_t length );

	/// <summary>
	/// Part of Standard Security Encryption Algorithm 3.1 
	/// </summary>
	/// <param name="m_bState"></param>
	/// <param name="inputData"></param>
	/// <param name="offset"></param>
	/// <param name="length"></param>
	/// <param name="outputData"></param>
	void Transform( byte* inputData, size_t length, byte* outputData );

	// call once to encrypt, again to decrypt
	void Transform( byte* bKey, int iKeyLen, byte* inputData, size_t length, byte* outputData );

	// in-line encrypt/decrypt
	void Transform( byte* inputData, size_t length  );
	void Transform( byte* bKey, int iKeyLen, byte* inputData, size_t length  );

protected:
	byte m_bState[ 256 ];
};


};};
