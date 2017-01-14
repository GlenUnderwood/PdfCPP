#pragma once
//#include <iosfwd>
//#include <sstream>
//typedef std::basic_stringstream<byte> bytestream;


#ifdef PDF_EXPORTS
#define PDF_API __declspec(dllexport)
#else
#define PDF_API __declspec(dllimport)
#endif

namespace Pdf{

namespace Utils{

class CMemoryStream
{
public:
	CMemoryStream(void);
	CMemoryStream(size_t szInitialSize);
	virtual ~CMemoryStream(void);

	inline size_t GetAllocLength() const throw();
	inline size_t GetLength() const throw();
	size_t Resize(size_t newSize);
	void Clear();
	void Free();
	inline byte* GetBuffer();
	inline byte* Detach();
	inline size_t Pos() const;
	inline void SetPos(size_t iNew);
	byte ReadByte();
	void WriteByte(byte c);
	void ToArray(byte** ppNew, size_t * pnLen);

protected:
	size_t m_iAllocLen, m_iLen, m_iPos; 
	byte* m_pData; 
};

}; //namespace Utils
}; //namespace Pdf

