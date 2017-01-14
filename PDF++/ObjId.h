#pragma once
#include "ExportDef.h"

#include <string>


namespace Pdf {

class PDF_API pdfObjId : public CSerializable
{
public:
	pdfObjId(void);
	pdfObjId(int id, unsigned short ver, bool bFree = false);
	pdfObjId(const pdfObjId& rhs);
	virtual ~pdfObjId(void);

	// CSerializable
	void Serialize( std::ostream& stm );

	pdfObjId& operator=( const pdfObjId& rhs ) { 	if( this != &rhs ) { _Id = rhs._Id; _GenNum = rhs._GenNum;  _bFree = rhs._bFree; } return *this; };
	inline pdfObjId& operator=(const std::nullptr_t i)
	{
		_Id = 0; _GenNum=0; _bFree = false;
		return *this;
	}


	inline bool operator<( const pdfObjId& rhs ) const { return Compare(rhs) < 0; };
	inline bool operator<=( const pdfObjId& rhs ) const { return Compare(rhs) <= 0; };
	inline bool operator==( const pdfObjId& rhs ) const { return _Id == rhs._Id && _GenNum == rhs._GenNum; };
	inline bool operator!=( const pdfObjId& rhs ) const { return !operator==(rhs); };

	virtual int Compare(const pdfObjId& rhs) const;

	int				GetId() const		{ return _Id; };
	unsigned short	GetGenNum() const	{ return _GenNum; };
	bool			IsFree() const		{ return _bFree; };
	bool			IsValid() const		{ return _Id > 0 && _GenNum >= 0; };

	void Set(const int id, const unsigned short iGenNum, const bool bFree ) { _Id = id; _GenNum = iGenNum; _bFree = bFree; };
	void SetId(const int i) 				{ _Id = i; };
	void SetGenNum(const unsigned short i)	{ _GenNum = i; };
	void SetFree(const bool i)				{ _bFree = i; };

	std::string ToString();

protected:
	int _Id;
	unsigned short _GenNum;
	bool _bFree;
};

};// namespace Exception

//PDF_API std::ostream& operator<<( std::ostream& stm, const Pdf::pdfObjId& id );
