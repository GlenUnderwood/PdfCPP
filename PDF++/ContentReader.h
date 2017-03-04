#pragma once
#include "Atom.h"
#include "DocData.h"
#include "pdfReader.h"
#include "ContentItem.h"
#include "GraphicsStack.h"
//#include <string>
#include <iostream>

namespace Pdf {

class pdfContentReader : public pdfReader
{
public:
	pdfContentReader( const CDocData::Ptr& doc );
	pdfContentReader( const CDocData::Ptr& doc, std::istream* pStm );
	virtual ~pdfContentReader();

	bool IsValidKeyword( const char* sVal ) override;

	typedef std::shared_ptr<pdfGraphicsStack> GraphicsStatePtr;
	typedef std::shared_ptr<std::stack<GraphicsStatePtr>> GraphicsStackPtr;

	GraphicsStackPtr GetCurrentGraphicsStack();
	void SetCurrentGraphicsStack(GraphicsStackPtr gs);
	//pdfGraphicsStack& GetCurrentGraphicsState();
	///void SetCurrentGraphicsState(const pdfGraphicsStack& gs);
	typedef std::list<Content::pdfContentItemBase*> ContentItemList;
	ContentItemList Parse(const pdfDictionary& m_dictRes, std::istream* pStm );

private:
	pdfAtom FindXObject( const char* sKey, const pdfDictionary& m_dictRes );
	pdfAtom FindBDCProp( const char* sKey, const pdfDictionary& m_dictRes );
	pdfAtom FindFont( const char* sKey, const pdfDictionary& m_dictRes );
	pdfAtom FindExtGState( const char* sKey, const pdfDictionary& m_dictRes );
	pdfAtom Find( const char* sDictName, const char* sKey, const pdfDictionary& m_dictRes );

	bool IsImageElement( const pdfAtom& atm );
	bool IsFormElement( const pdfAtom& atm );
	bool IsFontElement( const pdfAtom& atm );
	bool IsExtGStateElement( const pdfAtom& atm );
	//bool IsMCElement(const pdfAtom& atm)
	//{
	//    if (atm != null && atm is pdfStream)
	//    {
	//        pdfStream pImg = atm as pdfStream;
	//        return pImg.Has(Keys.Subtype) && pImg[Keys.Subtype].Str == Keys.Form;
	//    }
	//    return false;
	//}
	bool IsXObjectElement( const pdfAtom& atm );

//protected:
	//typedef std::shared_ptr<Content::pdfContentItemBase> ContentItemPtr;

	GraphicsStackPtr m_stackGS;
	//pdfGraphicsStack stack;
	//pdfGraphicsStack sNewStack;
	//int iGStateStack;

	GraphicsOps StrToOps( const char* sOp );
	std::map<string, GraphicsOps> m_mapStrToOps;

};

}; // namespace Pdf 

