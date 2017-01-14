#include "stdafx.h"
#include "ObjId.h"
#include "Enum.h"
#include "Document.h"

namespace Pdf {

pdfObjId::pdfObjId(void) : _Id( 0 ), _GenNum(0), _bFree( false )
{
}

pdfObjId::pdfObjId( int id, unsigned short ver, bool bFree ) : _Id( id ), _GenNum(ver),	_bFree( bFree )
{
}
pdfObjId::pdfObjId(const pdfObjId& rhs)
{
	operator=(rhs);
}


pdfObjId::~pdfObjId(void)
{
}

void pdfObjId::Serialize(  std::ostream& stm )
{
	stm << _Id << ' ' << _GenNum ;
}

int pdfObjId::Compare(const pdfObjId& id) const
{
	if (_Id == id._Id)
		return _GenNum - id._GenNum;
	return _Id - id._Id;
}

std::string pdfObjId::ToString()
{
	char sz[48] = "";
#ifdef __GNUC__
	sprintf( sz, "%u %u", _Id, _GenNum );
#else
	sprintf_s( sz, sizeof(sz)/sizeof(sz[0]), "%u %u", _Id, _GenNum ); 
#endif
	return sz;
}

};// namespace Pdf
/*
std::ostream& operator<<( std::ostream& stm, const Pdf::pdfObjId& id )
{
	stm << id.GetId() << ' ' << id.GetGenNum();
	return stm;
}*/
