#include "stdafx.h"
#include "Resources.h"
#include "Enum.h"
#include "Document.h"
#include "Dictionary.h"
#include "Array.h"

using namespace Pdf::Geometry;

namespace Pdf {

pdfResources::pdfResources(void)// : pdfObject()
{
}

pdfResources::pdfResources(const pdfAtom& src) : pdfDictionary(src)
{
	if( !IsNull() )
	{
		if (GetType() != AtomType::Dictionary )
			throw pdfTypeMismatchException();

		//bool bIsResources = Get("Type").Str() == "Resources";
		//if( !bIsResources )
		//	throw pdfObjectInvalidEx();
	}
}

pdfResources::pdfResources(pdfDocument& doc)  : pdfDictionary(doc)
{

}

pdfResources::~pdfResources(void)
{
}

pdfResources& pdfResources::operator=(const pdfAtom& src)
{
	pdfDictionary::operator=(src);
	if( !IsNull() )
	{
		if (GetType() != AtomType::Dictionary )
			throw pdfTypeMismatchException();

		//bool bIsResources = Get("Type").Str() == "Resources";
		//if( !bIsResources )
		//	throw pdfObjectInvalidEx();
	}
	return *this;
}

string pdfResources::Add( pdfObject pObj )
{
	if (pObj.GetAtom() == nullptr)
		throw pdfObjectUninitializedEx();
	return Add( pObj.GetAtom() );
}
string pdfResources::Add( pdfAtom atmObj )
{
	if (atmObj == nullptr)
		throw pdfObjectUninitializedEx();
	//			if (!atmObj.Indirect)
	//				throw new pdfMgrException("Cannot add direct object");
	if (GetDocument() == nullptr)
		throw pdfDocUndefinedException();
	if (!GetDocument().GetSecurity()->CanEdit() )
		throw pdfUnauthorizedEx();
	string sKey = "";
	pdfAtom atmDeref = atmObj.Dereference();// deref for easier use later on.
	pdfDictionary dictResources = atmObj;

	if (dictResources != nullptr)
	{
		string sDictName = "", sObjPrefix = "";
		if (atmDeref.IsStream() && pdfBlob(atmDeref)[Key_Subtype].Str() == Key_Image)// or XObject, etc.
		{
			sObjPrefix = "Im";
			sDictName = Key_XObject;
		}
		else if (atmDeref.IsDictionary() && pdfDictionary( atmDeref )[Key_Type].Str() == Key_Font)
		{
			sObjPrefix = "Fnt";
			sDictName = Key_Font;
		}
		else if (atmDeref.IsDictionary() && pdfDictionary( atmDeref )[Key_Type].Str() == Key_ExtGState)
		{
			sObjPrefix = "GS";
			sDictName = Key_ExtGState;
		}
		else if (atmDeref.IsDictionary() && pdfDictionary( atmDeref )[Key_Type].Str() == Key_Form)
		{
			sObjPrefix = Key_Form;
			sDictName = Key_XObject;
		}
		//else if (atm is pdfImage)// or XObject, etc.
		//	sDictName = Key_Font;
		// else ExtGState, ColorSpaces, etc.
		else
			throw pdfUnrecognizedItemTypeEx();


		// do we already reference this object? - must be an indirect one
		if (atmDeref.IsIndirect() )
		{
			try
			{
				pdfDictionary dictFonts = operator[](sDictName);// as pdfDictionary;
				if (dictFonts != nullptr)
				{
					pdfAtom atm = dictFonts[atmObj.GetId()];
					if( atm != nullptr )
						sKey = atm.GetKey();
				}
			}
			catch (...) {}
		}

/*
		if (sDictName.Length > 0)
		{
			if (!dictResources.Has( sDictName ))
				dictResources[sDictName] = Document.Factory.CreateDict( false );

			if (dictResources.Has( sDictName ))//create central func to get or create something if it doesn't exist.
			{
				Debug.Assert( dictResources[sDictName] != null && dictResources[sDictName] is pdfDictionary );
				pdfDictionary dict = dictResources[sDictName] as pdfDictionary;
				int idx = 0;
				sKey = string.Format( "{0}{1}", sObjPrefix, idx++ );
				while (dict.Has( sKey ))
				{
					sKey = string.Format( "{0}{1}", sObjPrefix, idx++ );
				}

				dict[sKey] = atmObj;// make sure this is atmObj (andnot atm) because is may be a reference.
			}
		}*/
	}

	return sKey;
}

};// namespace Pdf
