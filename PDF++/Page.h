#pragma once
#include <memory>
#include "Object.h"
#include "Geometry.h"
#include "Contents.h"

#include "ExportDef.h"

namespace Pdf {

class pdfDocument;

class PDF_API pdfPage : public pdfObject
{
protected:
	//pdfPage(pdfDocument& doc);
public:
	pdfPage(void);
	pdfPage(const pdfAtom& src);
	virtual ~pdfPage(void);

	pdfPage& operator=(const pdfAtom& src);
	inline pdfPage& operator=(const std::nullptr_t i) { m_atm.SetValue( i ); return *this;}

	bool operator==( const pdfPage& src ) const;
	bool operator==( const pdfObjId& src ) const;

	inline pdfAtom Get( const char* szKey ) const
	{		return GetDict().Get( szKey );		}
	inline pdfAtom Get( const pdfObjId& id ) const
	{		return GetDict().Get( id );		}
	inline pdfAtom Get( const std::string& szKey ) const
	{		return GetDict().Get( szKey );		}
	inline pdfAtom Set( const char* szKey, const pdfAtom& atm )
	{		return GetDict().Set( szKey, atm );		}
	inline pdfAtom Set( const std::string& szKey, const pdfAtom& atm )
	{		return GetDict().Set( szKey.c_str(), atm );		}
	
	Geometry::pdfRect GetMediaBox();
	void SetMediaBox( const Geometry::pdfRect& rect);
	Geometry::pdfRect GetCropBox();
	void SetCropBox( const Geometry::pdfRect& rect);

	inline pdfAtom Annotations();
	inline pdfContents Contents();
	inline pdfResources Resources();

protected:
	Geometry::pdfRect GetBox(const char* szName);
	void SetBox(const char* szName, const Geometry::pdfRect& rect );
};

};// namespace Exception
