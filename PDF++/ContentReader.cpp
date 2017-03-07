#include "stdafx.h"
#include "Constants.h"
#include "ContentReader.h"
#include "ContentItem.h"
#include "TextItem.h"
#include "PathItem.h"
#include "ClipItem.h"
#include "ImageItem.h"
#include "ContainerItem.h"
#include "GraphicsStack.h"
#include "ExtGState.h"

#include <list>
#include <string>
#include <iostream>
#include <stack>

namespace Pdf {

using namespace Content;

pdfContentReader::pdfContentReader( const CDocData::Ptr& doc ) : pdfReader(doc)
{
	m_strDefStrEncoding = StringEncoding::StdEncoding;

	m_bAssumeDocIsEncrypted = false;
}

pdfContentReader::pdfContentReader( const CDocData::Ptr& doc, std::istream* pStm ) : pdfReader( doc, pStm )
{
	m_strDefStrEncoding = StringEncoding::StdEncoding;

	m_bAssumeDocIsEncrypted = false;
}

pdfContentReader::~pdfContentReader()
{
}

bool pdfContentReader::IsValidKeyword( const char* sVal )
{
	return true;//pdfOp.GetOpFromString(sVal)!=OpType.opUnknown;
	/* (sVal == "Q" || sVal == "q" || sVal == "l" || sVal == "m" || sVal == "c" ||
			sVal == "W" || sVal == "rg" || sVal == "RG" || sVal == "Do" || sVal == "cm" || sVal == "tm" ||
			sVal == "Tf" || sVal == "false" || sVal == "null");*/
}

pdfContentReader::GraphicsStackPtr pdfContentReader::GetCurrentGraphicsStack()
{
	if (m_stackGS.get() == nullptr)
		m_stackGS = std::make_shared<std::stack<GraphicsStatePtr>>();
	if (m_stackGS.get()->empty() )
		m_stackGS.get()->push( std::make_shared<pdfGraphicsStack>() );
	return m_stackGS;
}

void pdfContentReader::SetCurrentGraphicsStack( GraphicsStackPtr gs )
{
	m_stackGS = gs;
}

pdfAtom pdfContentReader::FindXObject( const char* sKey, const pdfDictionary& m_dictRes )
{
	return Find( Key_XObject, sKey, m_dictRes );
}
pdfAtom pdfContentReader::FindBDCProp( const char* sKey, const pdfDictionary& m_dictRes )
{
	return Find( Key_Properties, sKey, m_dictRes );
}
pdfAtom pdfContentReader::FindFont( const char* sKey, const pdfDictionary& m_dictRes )
{
	return Find( Key_Font, sKey, m_dictRes );
}
pdfAtom pdfContentReader::FindExtGState( const char* sKey, const pdfDictionary& m_dictRes )
{
	return Find( Key_ExtGState, sKey, m_dictRes );
}
pdfAtom pdfContentReader::Find( const char* sDictName, const char* sKey, const pdfDictionary& m_dictRes )
{
	if ( /*m_stm == nullptr || */m_doc.expired())
		throw pdfDocClosedException();
	if ( m_dictRes == nullptr )
		throw pdfException( "Resources are required" );
	if (m_dictRes.Has( sDictName ))
	{
		pdfDictionary dictFonts( m_dictRes[sDictName] );
		if (dictFonts != nullptr)
			return dictFonts[sKey];
	}
	return pdfAtom();
}

bool pdfContentReader::IsImageElement( const pdfAtom& atm )
{
	if (atm != nullptr && atm.IsStream() )
	{
		pdfBlob pImg( atm );
		return pImg.Has( Key_Subtype ) && pImg[Key_Subtype].Str() == Key_Image;
	}
	return false;
}
bool pdfContentReader::IsFormElement( const pdfAtom& atm )
{
	if (atm != nullptr && atm.IsStream() )
	{
		pdfBlob pImg( atm );
		return pImg.Has( Key_Subtype ) && pImg[Key_Subtype].Str() == Key_Form;
	}
	return false;
}
bool pdfContentReader::IsFontElement( const pdfAtom& atm )
{
	if (atm != nullptr && atm.IsDictionary())
	{
		pdfDictionary pImg( atm );
		return pImg.Has( Key_Type ) && pImg[Key_Type].Str() == Key_Font;
	}
	return false;
}
bool pdfContentReader::IsExtGStateElement( const pdfAtom& atm )
{
	if (atm != nullptr && atm.IsDictionary() )
	{
		pdfDictionary pImg( atm );
		return pImg.Has( Key_Type ) && pImg[Key_Type].Str() == Key_ExtGState;
	}
	return false;
}
//private bool IsMCElement(pdfAtom atm)
//{
//    if (atm != null && atm.IsStream())
//    {
//        pdfBlob pImg = atm as pdfBlob;
//        return pImg.Has(Key_Subtype) && pImg[Key_Subtype].Str() == Key_Form;
//    }
//    return false;
//}
bool pdfContentReader::IsXObjectElement( const pdfAtom& atm )
{
	if (atm != nullptr && atm.IsStream())
	{
		pdfBlob pImg( atm );
		return pImg.Has( Key_Type ) && pImg[Key_Type].Str() == Key_XObject;
	}
	return false;
}

/// <summary>
/// 
/// </summary>

GraphicsOps pdfContentReader:: StrToOps( const char* sOp )
{
	if (m_mapStrToOps.empty())
	{
		m_mapStrToOps[Key_Q] = op_Q; // "Q"
		m_mapStrToOps[Key_q] = op_q; // "q"
		m_mapStrToOps[Key_cm] = op_cm; // "cm"
		m_mapStrToOps[Key_Tm] = op_Tm; // "Tm"
		m_mapStrToOps[Key_Do] = op_Do; // "Do"
		m_mapStrToOps[Key_BT] = op_BT; // "BT"
		m_mapStrToOps[Key_ET] = op_ET; // "ET"
		m_mapStrToOps[Key_BMC] = op_BMC; // "BMC"
		m_mapStrToOps[Key_BDC] = op_BDC; // "BDC"
		m_mapStrToOps[Key_EMC] = op_EMC; // "EMC"
		m_mapStrToOps[Key_DP] = op_DP; // "DP"
		m_mapStrToOps[Key_MP] = op_MP; // "MP"
		m_mapStrToOps[Key_Tf] = op_Tf; // "Tf"
		m_mapStrToOps[Key_Tj] = op_Tj; // "Tj"
		m_mapStrToOps[Key_TJ] = op_TJ; // "TJ"
		m_mapStrToOps[Key_Tstar] = op_Tstar; // "T*"
		m_mapStrToOps[Key_TD] = op_TD; // "TD"
		m_mapStrToOps[Key_Td] = op_Td; // "Td"
		m_mapStrToOps[Key_Tr] = op_Tr; // "Tr"
		m_mapStrToOps[Key_Th] = op_Th; // "Th"
		m_mapStrToOps[Key_Tw] = op_Tw; // "Tw"
		m_mapStrToOps[Key_Tc] = op_Tc; // "Tc"
		m_mapStrToOps[Key_Ts] = op_Ts; // "Ts"
		m_mapStrToOps[Key_Tz] = op_Tz; // "Tz"
		m_mapStrToOps[Key_TL] = op_TL; // "TL"
		m_mapStrToOps[Key_g] = op_g; // "g"
		m_mapStrToOps[Key_G] = op_G; // "G"
		m_mapStrToOps[Key_rg] = op_rg; // "rg"
		m_mapStrToOps[Key_RG] = op_RG; // "RG"
		m_mapStrToOps[Key_K] = op_K; // "K"
		m_mapStrToOps[Key_k] = op_k; // "k"
		m_mapStrToOps[Key_m] = op_m; // "m"
		m_mapStrToOps[Key_l] = op_l; // "l"
		m_mapStrToOps[Key_re] = op_re; // "re"
		m_mapStrToOps[Key_n] = op_n; // "n"
		m_mapStrToOps[Key_f] = op_f; // "f"
		m_mapStrToOps[Key_Wstar] = op_Wstar; // "W*"
		m_mapStrToOps[Key_w] = op_w; // "w"
		m_mapStrToOps[Key_W] = op_W; // "W"
		m_mapStrToOps[Key_gs] = op_gs; // "gs"
		m_mapStrToOps[Key_d] = op_d; // "d"
		m_mapStrToOps[Key_d] = op_c; // "c"
		m_mapStrToOps[Key_ri] = op_ri; // "ri"
		m_mapStrToOps[Key_bstar] = op_bstar; // "b*"
		m_mapStrToOps[Key_quote] = op_quote; // "'"
		m_mapStrToOps[Key_dblquote] = op_dblquote;
	}
	auto it = m_mapStrToOps.find(sOp);
	if (it != m_mapStrToOps.end())
		return it->second;
	return op_bad;
}

pdfContentReader::ContentItemList pdfContentReader::Parse( const pdfDictionary& m_dictRes, std::istream* pStm )
{
	if ( /*m_stm == nullptr || */m_doc.expired())
		throw pdfDocClosedException();
	if (m_dictRes == nullptr)
		throw pdfException( "Resources are required" );

	Reset( pStm );
//	Cursor = (int)stm.Position;

	GraphicsStatePtr pCurrState = GetCurrentGraphicsStack()->top();

	ContentItemList lstObjects;
	std::vector<pdfAtom> lstTokens;

	std::string sLastOp = "";

	// Items we are currently constructing....
	pdfTextItem* pTextItem = nullptr;
	pdfPathItem* pPathItem = nullptr;

	//pCurrState->GetTState().Font = pdfDocument.UnknownFont;
	//GraphicsStackPtr sNewStack;
	int iGStateStack = 0;

	//
	while (_Current != (char)0xFFFF)
	{
		_int64 iC = _iCursor;
		pdfAtom atm = ReadObj();
		if (atm == nullptr || atm.GetType() == AtomType::Null)
			break;

		lstTokens.push_back( atm );

		//OutputDebugStringA(atm.Obj.ToString());
		//	OutputDebugStringA(sOp);

		if (atm.GetType() == AtomType::Keyword)
		{
			atm = nullptr;

			int i = 0;
			string sOp = lstTokens.back().Str();

		tryagain:
			///  TAKE MATRICES INTO ACCOUTN WHEN LOADING numbers - we want everything to have as level a playing field as can be
			///  i.e.  all in 72dpi
			switch (StrToOps(sOp.c_str()))
			{
			////////////////////////////////////////////////////////////////////////////////////////////////////////////
			#pragma region Graphics State Operators
			////////////////////////////////////////////////////////////////////////////////////////////////////////////
			#pragma region Save state
			case GraphicsOps::op_q://"q":// opType.opPushStack

				pCurrState = std::make_shared<pdfGraphicsStack>();
				m_stackGS.get()->push( pCurrState );
				iGStateStack++;
				/*sNewStack = new pdfGraphicsStack( stack );
				sNewStack.Prev = stack;
				stack = sNewStack;*/

				lstTokens.clear();

				// finish of any objects that were under construction
				if (pPathItem != nullptr)
				{
					if (!pPathItem->IsClipping())
						lstObjects.push_back( pPathItem );
					else
					{
						pdfClipItem clip( m_dictRes );
						clip.Contents.InternalAdd( pPathItem );
						pCurrState->GetGState().Clip = clip;
					}
					pPathItem = nullptr;
				}

				// Set a marker so we know how to transition graphics states when/if inserting new objects 
				//lstObjects.push_back( new pdfMarkerItem( pCurrState->GetGState(), pdfMarker.GfxPush ) );

				break;

			#pragma endregion 
			////////////////////////////////////////////////////////////////////////////////////////////////////////////
			#pragma region Restore state.Must pair with a save
			case GraphicsOps::op_Q://"Q":// opType.opPopStack
				iGStateStack--;
				_ASSERT( iGStateStack > 0 );
				//_ASSERT( stack.Prev != null /*&& iGStateStack > 0*/ );
				//stack = stack.Prev;

				if (m_stackGS->size() > 1)
				{
					m_stackGS->pop();
					pCurrState = m_stackGS->top();
				}

				lstTokens.clear();

				// finish of any objects that were under construction
				if (pPathItem != nullptr)
				{
					lstObjects.push_back( pPathItem );
					pPathItem = nullptr;
				}

				// Set a marker so we know how to transition graphics states when/if inserting new objects 
				//lstObjects.push_back( new pdfMarkerItem( pCurrState->GetGState(), pdfMarker.GfxPop ) );

				break;

			#pragma endregion 
			////////////////////////////////////////////////////////////////////////////////////////////////////////////
			#pragma region ExtGState
			case GraphicsOps::op_gs://"gs":// ExtGState
				_ASSERT( lstTokens.size() >= 2 );
				i = lstTokens.size() - 2;

				//behaves similar to a pop stack, but we don't pop it, we just create a new one
				// which stays in effect from here on.
				atm = FindExtGState( lstTokens[i].Str().c_str(), m_dictRes );
				if (IsExtGStateElement( atm ))
				{
					//sNewGstate = new pdfGState(GState);
					//sNewGstate.Prev = GState;
					//GState = sNewGstate;

					pCurrState->GetGState().SetExtGState( pdfExtGState( atm ) );// as pdfDictionary;
				}

				lstTokens.clear();
				break;
				#pragma endregion 
			////////////////////////////////////////////////////////////////////////////////////////////////////////////
			#pragma region fill color - DeviceGray
			case GraphicsOps::op_g://"g":// fill color - DeviceGray
				_ASSERT( lstTokens.size() >= 2 );
				i = lstTokens.size() - 2;

				// throw or assert if the val isn't a number
				//i = (int)(lstTokens[i].Real * 255.0);
				//GState.FillColor = System.Drawing.Color.FromArgb(i, i, i);
				pCurrState->GetGState().SetFillColor( pdfColor( 1.0F, lstTokens[i].RealF() ) );

				//if (pTextItem != nullptr )
				//	pTextItem.m_runs[pTextItem.m_runs.Count - 1].GState.FillColor = new pdfColor(1.0F, lstTokens[i].RealF());

				lstTokens.clear();
				break;
				#pragma endregion 
			////////////////////////////////////////////////////////////////////////////////////////////////////////////
			#pragma region fill color - DeviceRGB
			case GraphicsOps::op_rg://"rg":// fill color - DeviceRGB
				_ASSERT( lstTokens.size() >= 4 );
				i = lstTokens.size() - 4;

				// throw or assert if the val isn't a number
				//GState.FillColor = System.Drawing.Color.FromArgb((int)(lstTokens[i].Real * 255.0), (int)(lstTokens[i + 1].Real * 255.0), (int)(lstTokens[i + 2].Real * 255.0));
				pCurrState->GetGState().SetFillColor( pdfColor( 1.0F, lstTokens[i].RealF(), lstTokens[i + 1].RealF(), lstTokens[i + 2].RealF() ) );

				//if (pTextItem != nullptr )
				//	pTextItem.m_runs[pTextItem.m_runs.Count - 1].GState.FillColor = new pdfColor(1.0F, lstTokens[i].RealF(), lstTokens[i + 1].RealF(), lstTokens[i + 2].RealF());

				lstTokens.clear();
				break;
				#pragma endregion 
			////////////////////////////////////////////////////////////////////////////////////////////////////////////
			#pragma region fill color - DeviceCMYK
			case GraphicsOps::op_k://"k":// fill color - DeviceCMYK
				_ASSERT( lstTokens.size() >= 5 );
				i = lstTokens.size() - 5;

				// throw or assert if the val isn't a number
				pCurrState->GetGState().SetFillColor( pdfColor( 1.0F, lstTokens[i].RealF(), lstTokens[i + 1].RealF(), lstTokens[i + 2].RealF(), lstTokens[i + 3].RealF() );

				//if (pTextItem != nullptr )
				//	pTextItem.m_runs[pTextItem.m_runs.Count - 1].GState.FillColor = new pdfColor(1.0F, lstTokens[i].RealF(), lstTokens[i + 1].RealF(), lstTokens[i + 2].RealF(), lstTokens[i + 3].RealF());

				lstTokens.clear();
				break;
				#pragma endregion 
			////////////////////////////////////////////////////////////////////////////////////////////////////////////
			#pragma region stroke color - DeviceGray
			case GraphicsOps::op_G://"G":// stroke color - DeviceGray
				_ASSERT( lstTokens.size() >= 2 );
				i = lstTokens.size() - 2;

				// throw or assert if the val isn't a number
				//i = (int)(lstTokens[i].Real * 255.0);
				//GState.StrokeColor = System.Drawing.Color.FromArgb(i, i, i);
				pCurrState->GetGState().SetStrokeColor( pdfColor( 1.0F, lstTokens[i].RealF() ) );

				//if (pTextItem != nullptr )
				//	pTextItem.m_runs[pTextItem.m_runs.Count - 1].GState.StrokeColor = new pdfColor(1.0F, lstTokens[i].RealF());

				lstTokens.clear();
				break;
			#pragma endregion 
			////////////////////////////////////////////////////////////////////////////////////////////////////////////
			#pragma region stroke color - DeviceRGB
			case GraphicsOps::op_RG://"RG":// stroke color - DeviceRGB
				_ASSERT( lstTokens.size() >= 4 );
				i = lstTokens.size() - 4;

				// throw or assert if the val isn't a number
				//GState.StrokeColor = System.Drawing.Color.FromArgb((int)(lstTokens[i].Real * 255.0), (int)(lstTokens[i + 1].Real * 255.0), (int)(lstTokens[i + 2].Real * 255.0));
				pCurrState->GetGState().SetStrokeColor( pdfColor( 1.0F, lstTokens[i].RealF(), lstTokens[i + 1].RealF(), lstTokens[i + 2].RealF() ) );

				//if (pTextItem != nullptr )
				//	pTextItem.m_runs[pTextItem.m_runs.Count - 1].GState.StrokeColor = new pdfColor(1.0F, lstTokens[i].RealF(), lstTokens[i + 1].RealF(), lstTokens[i + 2].RealF());

				lstTokens.clear();
				break;

			#pragma endregion 
			////////////////////////////////////////////////////////////////////////////////////////////////////////////
			#pragma region stroke color - DeviceCMYK
			case GraphicsOps::op_K://"K":// stroke color - DeviceCMYK
				_ASSERT( lstTokens.size() >= 5 );
				i = lstTokens.size() - 5;

				// throw or assert if the val isn't a number
				//GState.StrokeColor = System.Drawing.Color.FromArgb((int)(lstTokens[i].Real * 255.0), (int)(lstTokens[i + 1].Real * 255.0), (int)(lstTokens[i + 2].Real * 255.0));
				pCurrState->GetGState().SetStrokeColor( pdfColor( 1.0F, lstTokens[i].RealF(), lstTokens[i + 1].RealF(), lstTokens[i + 2].RealF(), lstTokens[i + 3].RealF() ) );

				//if (pTextItem != nullptr )
				//	pTextItem.m_runs[pTextItem.m_runs.Count - 1].GState.StrokeColor = new pdfColor(1.0F, lstTokens[i].RealF(), lstTokens[i + 1].RealF(), lstTokens[i + 2].RealF(), lstTokens[i + 3].RealF());

				//Color c = GState.StrokeColor.RGB;
				//GState.StrokeColor.Type = pdfColorSpaceType.DeviceRGB;
				//GState.StrokeColor.Type = pdfColorSpaceType.DeviceCMYK;

				lstTokens.clear();
				break;

			#pragma endregion 
			////////////////////////////////////////////////////////////////////////////////////////////////////////////
			#pragma region Dash Array
			case GraphicsOps::op_d:// Dash Array
				{
					_ASSERT( lstTokens.size() >= 3 );
					i = lstTokens.size() - 3;

					// throw or assert if the val isn't a number
					pdfArray arr = lstTokens[i] as pdfArray;
					if (arr != nullptr )
					{
						pCurrState->GetGState().Dash = new pdfGState.DashPattern();
						if (arr.Count > 0)
						{
							pCurrState->GetGState().Dash.Array = new int[arr.Count];
							for (int idx = 0; idx < arr.Count; idx++)
							{
								pCurrState->GetGState().Dash.Array[idx] = arr[idx].Int;
							}
						}
						//
						pCurrState->GetGState().Dash.Phase = lstTokens[i + 1].Int;
					}

					lstTokens.clear();
					break;
				}
			#pragma endregion 
			////////////////////////////////////////////////////////////////////////////////////////////////////////////
			#pragma region Line Width
			case GraphicsOps::op_w:// Line Width
				_ASSERT( lstTokens.size() >= 2 );
				i = lstTokens.size() - 2;

				// throw or assert if the val isn't a number
				pCurrState->GetGState().SetLineWidth( lstTokens[i].RealF() );

				if (pCurrState->GetGState().GetLineWidth() < 0.1F) 
					pCurrState->GetGState().SetLineWidth( 0.1F );

				lstTokens.clear();
				break;
			#pragma endregion 
			////////////////////////////////////////////////////////////////////////////////////////////////////////////
			#pragma region Miter Limit
			case GraphicsOps::op_M:// Miter Limit
				_ASSERT( lstTokens.size() >= 2 );
				i = lstTokens.size() - 2;

				// throw or assert if the val isn't a number
				pCurrState->GetGState().MiterLimit = lstTokens[i].RealF();

				lstTokens.clear();
				break;
				#pragma endregion 
			////////////////////////////////////////////////////////////////////////////////////////////////////////////
			#pragma region FlatnessTolerance
			case GraphicsOps::op_i:// FlatnessTolerance
				_ASSERT( lstTokens.size() >= 2 );
				i = lstTokens.size() - 2;

				// throw or assert if the val isn't a number
				pCurrState->GetGState().FlatnessTolerance = (byte)lstTokens[i].Int;

				lstTokens.clear();
				break;
		#pragma endregion 
			////////////////////////////////////////////////////////////////////////////////////////////////////////////
			#pragma region LineJoin
			case GraphicsOps::op_j:// LineJoin
				_ASSERT( lstTokens.size() >= 2 );
				i = lstTokens.size() - 2;

				// throw or assert if the val isn't a number
				pCurrState->GetGState().LineJoin = (byte)lstTokens[i].Int;

				lstTokens.clear();
				break;
			#pragma endregion 
			////////////////////////////////////////////////////////////////////////////////////////////////////////////
			#pragma region LineCap
			case GraphicsOps::op_J:// LineCap
				_ASSERT( lstTokens.size() >= 2 );
				i = lstTokens.size() - 2;

				// throw or assert if the val isn't a number
				pCurrState->GetGState().LineCap = (byte)lstTokens[i].Int;

				lstTokens.clear();
				break;
			#pragma endregion 
			////////////////////////////////////////////////////////////////////////////////////////////////////////////
			#pragma region RenderingIntent
			case GraphicsOps::op_ri:// RenderingIntent
				_ASSERT( lstTokens.size() >= 2 );
				i = lstTokens.size() - 2;

				// throw or assert if the val isn't a number
				pCurrState->GetGState().SetRenderingIntent( lstTokens[i].Str().c_str() );

				lstTokens.clear();
				break;
			#pragma endregion 
			////////////////////////////////////////////////////////////////////////////////////////////////////////////
			#pragma region Concat matrix
			case GraphicsOps::op_cm:
				_ASSERT( lstTokens.size() >= 7 );
				i = lstTokens.size() - 7;

				/*if( sLastOp == sOp )
				{
				pdfMatrix m1 = new pdfMatrix( pCurrState->GetGState().Mat );
				pdfMatrix m2 = new pdfMatrix( pCurrState->GetGState().Mat );
				m1.Multiply( lstTokens[ i ].RealF(), lstTokens[ i + 1 ].RealF(), lstTokens[ i + 2 ].RealF(), lstTokens[ i + 3 ].RealF(), lstTokens[ i + 4 ].RealF(), lstTokens[ i + 5 ].RealF(),
				MatrixOrder.Prepend );
				m2.Multiply( lstTokens[ i ].RealF(), lstTokens[ i + 1 ].RealF(), lstTokens[ i + 2 ].RealF(), lstTokens[ i + 3 ].RealF(), lstTokens[ i + 4 ].RealF(), lstTokens[ i + 5 ].RealF(),
				MatrixOrder.Append );
				OutputDebugStringA( "" );
				}*/
				pCurrState->GetGState().Mat.Multiply( lstTokens[i].RealF(), lstTokens[i + 1].RealF(), lstTokens[i + 2].RealF(), lstTokens[i + 3].RealF(), lstTokens[i + 4].RealF(), lstTokens[i + 5].RealF(),
					MatrixOrder.Prepend );

				lstTokens.clear();
				//OutputDebugStringA("Found a conversion matrix!"), m_Doc );
				break;
			#pragma endregion 
			////////////////////////////////////////////////////////////////////////////////////////////////////////////
			#pragma endregion 

			////////////////////////////////////////////////////////////////////////////////////////////////////////////
			#pragma region Path Operators

			#pragma region moveto  starts new path
			case GraphicsOps::op_m: // moveto
				_ASSERT( lstTokens.size() >= 3 );
				i = lstTokens.size() - 3;

				if (pPathItem != nullptr)
				{
					//lstObjects.push_back(pPathItem);
					//pPathItem = nullptr;
				}
				else
					pPathItem = new pdfPathItem( pCurrState->GetGState() );
				if (pPathItem != nullptr)
					pPathItem->MoveTo( lstTokens[i].RealF(), lstTokens[i + 1].RealF() );

				lstTokens.clear();
				break;// return new pdfOp(opType.opDo, m_Doc);
				#pragma endregion 
            #pragma region lineto
			case GraphicsOps::op_l: // lineto
				_ASSERT( lstTokens.size() >= 3 );
				i = lstTokens.size() - 3;

				if (pPathItem == nullptr )
					pPathItem = new pdfPathItem( pCurrState->GetGState() );
				if (pPathItem != nullptr)
					pPathItem->LineTo( lstTokens[i].RealF(), lstTokens[i + 1].RealF() );

				lstTokens.clear();
				break;
			#pragma endregion 
            #pragma region CurveTo
			case GraphicsOps::op_c: // CurveTo
				_ASSERT( lstTokens.size() >= 7 );
				i = lstTokens.size() - 7;

				if (pPathItem == nullptr )
					pPathItem = new pdfPathItem( pCurrState->GetGState() );
				if (pPathItem != nullptr)
					pPathItem->CurveTo(
						new PointF( lstTokens[i].RealF(), lstTokens[i + 1].RealF() ),
						new PointF( lstTokens[i + 2].RealF(), lstTokens[i + 3].RealF() ),
						new PointF( lstTokens[i + 4].RealF(), lstTokens[i + 5].RealF() )
					);

				lstTokens.clear();
				break;
			#pragma endregion 
            #pragma region CurveToV
			case GraphicsOps::op_v: // CurveToV
				_ASSERT( lstTokens.size() >= 5 );
				i = lstTokens.size() - 5;

				if (pPathItem == nullptr )
					pPathItem = new pdfPathItem( pCurrState->GetGState() );
				if (pPathItem != nullptr)
					pPathItem->CurveToV(
						new PointF( lstTokens[i].RealF(), lstTokens[i + 1].RealF() ),
						new PointF( lstTokens[i + 2].RealF(), lstTokens[i + 3].RealF() )
					);

				lstTokens.clear();
				break;
				#pragma endregion 
            #pragma region CurveToY
			case GraphicsOps::op_y: // CurveToY
				_ASSERT( lstTokens.size() >= 5 );
				i = lstTokens.size() - 5;

				if (pPathItem == nullptr )
					pPathItem = new pdfPathItem( pCurrState->GetGState() );
				if (pPathItem != nullptr)
					pPathItem->CurveToY(
						new PointF( lstTokens[i].RealF(), lstTokens[i + 1].RealF() ),
						new PointF( lstTokens[i + 2].RealF(), lstTokens[i + 3].RealF() )
					);

				lstTokens.clear();
				break;
				#pragma endregion 
            #pragma region W = Clipping
			case GraphicsOps::op_W:
				if (pPathItem != nullptr)
					pPathItem->SetClip( false );
				lstTokens.clear();
				break;
				#pragma endregion 

			#pragma region W* = Clipping( even - odd rule )
			case GraphicsOps::op_Wstar:
				if (pPathItem != nullptr)
					pPathItem->SetClip( true );
				lstTokens.clear();
				break;
				#pragma endregion 
            #pragma region Closes - h
			case GraphicsOps::op_h:
				if (pPathItem != nullptr)
					pPathItem->Close( false, false, false );

				lstTokens.clear();
				break;
				#pragma endregion 

            #pragma region Rect Ends Current path and creates a new one of just the rect - re
			case GraphicsOps::op_re: // Rect
				_ASSERT( lstTokens.size() >= 5 );
				i = lstTokens.size() - 5;

				/*if (pPathItem != nullptr)
				{
				lstObjects.push_back(pPathItem);
				pPathItem = nullptr;
				}*/
				//else
				if (pPathItem == nullptr )
				{
					pPathItem = new pdfPathItem( pCurrState->GetGState() );
					//lstObjects.push_back( pPathItem );
				}
				if (pPathItem != nullptr)
				{
					pPathItem->AddRect( lstTokens[i].RealF(), lstTokens[i + 1].RealF(), lstTokens[i + 2].RealF(), lstTokens[i + 3].RealF() );
					//lstObjects.push_back(pPathItem);
					//pPathItem = nullptr;
				}

				lstTokens.clear();
				break;
			#pragma endregion 

					//these all end the path
            #pragma region Neither fill nor stroke; used for clipping; ends path - n
			case GraphicsOps::op_n:
				if (pPathItem != nullptr)
				{
					pPathItem->CloseWithoutPainting();
					if (!pPathItem->IsClipping)
						lstObjects.push_back( pPathItem );
					else
					{
						pdfClipItem clip = new pdfClipItem( m_dictRes );
						clip.Contents.InternalAdd( pPathItem );
						pCurrState->GetGState().Clip = clip;
					}
					pPathItem = nullptr;
				}

				lstTokens.clear();
				break;
				#pragma endregion 
             #pragma region Fill the path - f( or F ) - fills imply closing
			case GraphicsOps::op_f:
			case GraphicsOps::op_F:
				if (pPathItem != nullptr)
				{
					pPathItem->Close( false, true, false );
					if (!pPathItem->IsClipping)
						lstObjects.push_back( pPathItem );
					else
					{
						pdfClipItem clip = new pdfClipItem( m_dictRes );
						clip.Contents.InternalAdd( pPathItem );
						pCurrState->GetGState().Clip = clip;
					}
					pPathItem = nullptr;
				}

				lstTokens.clear();
				break;
				#pragma endregion 
             #pragma region Fill w / EvenOdd Rule - f*
			case GraphicsOps::op_fstar: // "f*":
				if (pPathItem != nullptr)
				{
					pPathItem->Close( false, true, true );
					if (!pPathItem->IsClipping)
						lstObjects.push_back( pPathItem );
					else
					{
						pdfClipItem clip = new pdfClipItem( m_dictRes );
						clip.Contents.InternalAdd( pPathItem );
						pCurrState->GetGState().Clip = clip;
					}
					pPathItem = nullptr;
				}
				lstTokens.clear();
				break;
				#pragma endregion 
             #pragma region Stroke the path - S
			case GraphicsOps::op_S:
				if (pPathItem != nullptr)
				{
					pPathItem->Str()oke();
					if (!pPathItem->IsClipping)
						lstObjects.push_back( pPathItem );
					else
					{
						pdfClipItem clip = new pdfClipItem( m_dictRes );
						clip.Contents.InternalAdd( pPathItem );
						pCurrState->GetGState().Clip = clip;
					}
					pPathItem = nullptr;
				}
				lstTokens.clear();
				break;
				#pragma endregion 
             #pragma region Fill And Stroke the path - B
			case GraphicsOps::op_B:
				if (pPathItem != nullptr)
				{
					pPathItem->Fill( true, false );
					if (!pPathItem->IsClipping)
						lstObjects.push_back( pPathItem );
					else
					{
						pdfClipItem clip = new pdfClipItem( m_dictRes );
						clip.Contents.InternalAdd( pPathItem );
						pCurrState->GetGState().Clip = clip;
					}
					pPathItem = nullptr;
				}

				lstTokens.clear();
				break;
				#pragma endregion 
             #pragma region Fill w / EvenOdd Rule And Stroke the path - B*
			case "B*":
				if (pPathItem != nullptr)
				{
					pPathItem->Fill( true, true );
					if (!pPathItem->IsClipping)
						lstObjects.push_back( pPathItem );
					else
					{
						pdfClipItem clip = new pdfClipItem( m_dictRes );
						clip.Contents.InternalAdd( pPathItem );
						pCurrState->GetGState().Clip = clip;
					}
					pPathItem = nullptr;
				}
				lstTokens.clear();
				break;
				#pragma endregion 
             #pragma region Close and Fill And Stroke the path - b
			case GraphicsOps::op_b:
				if (pPathItem != nullptr)
				{
					pPathItem->Close( true, true, false );
					if (!pPathItem->IsClipping)
						lstObjects.push_back( pPathItem );
					else
					{
						pdfClipItem clip = new pdfClipItem( m_dictRes );
						clip.Contents.InternalAdd( pPathItem );
						pCurrState->GetGState().Clip = clip;
					}
					pPathItem = nullptr;
				}

				lstTokens.clear();
				break;
				#pragma endregion 
			#pragma region Close and Fill w / EvenOdd Rule And Stroke the path - b*
			case GraphicsOps::op_bstar:// "b*"
				if (pPathItem != nullptr)
				{
					pPathItem->Close( true, true, true );
					if (!pPathItem->IsClipping)
						lstObjects.push_back( pPathItem );
					else
					{
						pdfClipItem clip = new pdfClipItem( m_dictRes );
						clip.Contents.InternalAdd( pPathItem );
						pCurrState->GetGState().Clip = clip;
					}
					pPathItem = nullptr;
				}

				lstTokens.clear();
				break;
				#pragma endregion 
             #pragma region Close and stroke the path - s
			case GraphicsOps::op_s:
				if (pPathItem != nullptr)
				{
					pPathItem->Close( true, false, false );
					if (!pPathItem->IsClipping)
						lstObjects.push_back( pPathItem );
					else
					{
						pdfClipItem clip = new pdfClipItem( m_dictRes );
						clip.Contents.InternalAdd( pPathItem );
						pCurrState->GetGState().Clip = clip;
					}
					pPathItem = nullptr;
				}
				lstTokens.clear();
				break;
				#pragma endregion 
					#pragma endregion 

			////////////////////////////////////////////////////////////////////////////////////////////////////////////
			#pragma region Invoke an XObject
			case GraphicsOps::op_Do: // Invoke an XObject (images, forms, etc)
				_ASSERT( lstTokens.size() >= 2 );
				i = lstTokens.size() - 2;

				atm = FindXObject( lstTokens[i].Str(), m_dictRes );
				if (atm != nullptr )
				{
					if (IsImageElement( atm ))
					{
						lstObjects.push_back( new pdfImageItem( atm as pdfBlob, pCurrState->GetGState() ) );
					}
					else if (IsFormElement( atm ))
					{
						pdfFormItem pNew = new pdfFormItem( atm as pdfBlob, pCurrState->GetGState() );
						lstObjects.push_back( pNew );
					}
					else
						OutputDebugStringA( "\nUNHANDLED XOBJECT: {0}", lstTokens[i].Str() );
				}
				else
					OutputDebugStringA( "\nCannot find XOBJECT: {0}", lstTokens[i].Str() );

				lstTokens.clear();
				break;// return new pdfOp(opType.opDo, m_Doc);
				#pragma endregion 

			////////////////////////////////////////////////////////////////////////////////////////////////////////////
            #pragma region Text Objects
            #pragma region OP_BT
			case GraphicsOps::op_BT: // start a new Tetxt Object

							  // Set a marker so we know how to transition graphics states when/if inserting new objects 
							  //lstObjects.push_back( new pdfMarkerItem( pCurrState->GetGState(), pdfMarker.BeginText ) );

				_ASSERT( lstTokens.size() >= 1 );
				if (pTextItem == nullptr )
				{//technically, thisis an error, but try to recover... by ignoring it

					pTextItem = new pdfTextItem( pCurrState->GetGState() );
					//pTextItem.m_runs.Add(new pdfTextItem.TextRun(pCurrState->GetTState()));

					//////////////////////////////////
					//pTextItem.m_runs.Last.GState.Mat = new Matrix(1, 0, 0, 1, 0, 0);
					//pTextItem.m_runs.Last.Rotation = (float)Math.Round(Utils.GetMatrixRotation(GState.Mat));
					//pTextItem.m_runs.Last.Points *= (float)Utils.GetMatrixVScale(GState.Mat);
					//pTextItem.m_runs.Last.X += GState.Mat.Elements[4];
					//pTextItem.m_runs.Last.Y -= GState.Mat.Elements[5];

				}
				else
				{
					_ASSERT( false );
				}
				// and at least one new run to start - which will be the one we're currently building.

				lstTokens.clear();
				break;// return new pdfOp(opType.opDo, m_Doc);
			#pragma endregion 
				////////////////////////////////////////////////////////////////////////////////////////////////////////////
            #pragma region OP_ET
			case GraphicsOps::op_ET: // end the text object currently getting built.
				_ASSERT( lstTokens.size() >= 1 );
				//_ASSERT(pTextItem != nullptr );

				// if the last one is blank, remove
				if (pTextItem != nullptr ) // outside of a BT is illegal - maybe later we could be forgiving and create a temp one just to use for "global" state settings
				{
					//if (pTextItem.m_runs.Count > 0 && (pTextItem.m_runs.Last.Text == null || pTextItem.m_runs.Last.Text.Length == 0))
					//	pTextItem.m_runs.Remove(pTextItem.m_runs.Last);

					if (pTextItem.m_runs.Count > 0)
						lstObjects.push_back( pTextItem );

					pTextItem = null;
				}

				lstTokens.clear();

				// Set a marker so we know how to transition graphics states when/if inserting new objects 
				//lstObjects.push_back( new pdfMarkerItem( pCurrState->GetGState(), pdfMarker.EndText ) );

				break;// return new pdfOp(opType.opDo, m_Doc);
			#pragma endregion 
				////////////////////////////////////////////////////////////////////////////////////////////////////////////
            #pragma region OP_Tf
			case GraphicsOps::op_Tf:// Font   "/FontName Size Tf"
				_ASSERT( lstTokens.size() >= 3 );
				//_ASSERT(pTextItem != nullptr );
				i = lstTokens.size() - 3;

				//
				atm = FindFont( lstTokens[i].Str(), m_dictRes );
				if (IsFontElement( atm ))
				{
					try
					{
						pdfFont next = pdfFont.FromAtom( atm );
						if (next != nullptr )
							pCurrState->GetTState().Font = next;
					}
					catch (const std::exception& e)
					{
						OutputDebugStringA( e.what() );
						OutputDebugStringA( "\n" );
					}
				}

				//
				pCurrState->GetTState().Points = lstTokens[i + 1].RealF();

				if (pCurrState->GetTState().Font == null || pCurrState->GetTState().Font.Type == FontType.Unknown)
					OutputDebugStringA( "NO FONT!!!!!!!!!!!!!!!!!!!" );

				if (pCurrState->GetTState().Font != null && pCurrState->GetTState().Font.Encoding != null && pCurrState->GetTState().Font.Encoding.Name.Length > 0)
				{
					//OutputDebugStringA("\t***{0} becoming {1}", str.Str()ingType, pdfString.NameToStringType(ts.Font.Encoding.Name));
					StringEncoding t = pdfString.NameToStringType( pCurrState->GetTState().Font.Encoding.Name );
					if (t == StringEncoding::WinAnsiEncoding ||
						t == StringEncoding::PDFDocEncoding ||
						t == StringEncoding::StdEncoding ||
						t == StringEncoding::MacRomanEncoding)
						this.m_strDefStrEncoding = t;
				}

				lstTokens.clear();
				break;
			#pragma endregion 
				////////////////////////////////////////////////////////////////////////////////////////////////////////////
            #pragma region OP_Tm
			case GraphicsOps::op_Tm:
				_ASSERT( lstTokens.size() >= 7 );
				//_ASSERT(pTextItem != nullptr );
				i = lstTokens.size() - 7;

				pCurrState->GetTState().Mat = new pdfMatrix( lstTokens[i].RealF(), lstTokens[i + 1].RealF(), lstTokens[i + 2].RealF(), lstTokens[i + 3].RealF(), lstTokens[i + 4].RealF(), lstTokens[i + 5].RealF() );

				// these are always relative to the matrix
				pCurrState->GetTState().X = 0;
				pCurrState->GetTState().Y = 0;

				lstTokens.clear();
				//OutputDebugStringA("Found a conversion matrix!"), m_Doc );
				break;// return new pdfOp(opType.op_cm, m_Doc);
				#pragma endregion 
					////////////////////////////////////////////////////////////////////////////////////////////////////////////
            #pragma region OP_Td
			case GraphicsOps::op_Td:
				_ASSERT( lstTokens.size() >= 3 );
				i = lstTokens.size() - 3;

				pCurrState->GetTState().X += lstTokens[i].RealF();
				pCurrState->GetTState().Y += lstTokens[i + 1].RealF();

				lstTokens.clear();
				//OutputDebugStringA("Found a conversion matrix!"), m_Doc );
				break;
			#pragma endregion 
					////////////////////////////////////////////////////////////////////////////////////////////////////////////
            #pragma region OP_TD
			case GraphicsOps::op_TD:
				_ASSERT( lstTokens.size() >= 3 );
				i = lstTokens.size() - 3;

				pCurrState->GetTState().X += lstTokens[i].RealF();
				pCurrState->GetTState().Y += lstTokens[i + 1].RealF();

				pCurrState->GetTState().Leading = -lstTokens[i + 1].RealF();

				lstTokens.clear();
				break;
			#pragma endregion 
					////////////////////////////////////////////////////////////////////////////////////////////////////////////
            #pragma region OP_Tc
			case GraphicsOps::op_Tc:
				_ASSERT( lstTokens.size() >= 2 );
				//_ASSERT(pTextItem != nullptr );
				i = lstTokens.size() - 2;

				//if (pTextItem != nullptr ) // outside of a BT is illegal - maybe later we could be forgiving and create a temp one just to use for "global" state settings
				//{
				pCurrState->GetTState().CharSpacing = lstTokens[i].RealF();// / 1000.0f * pCurrState->GetTState().Mat.ScaleX;
															  //}

				lstTokens.clear();
				//OutputDebugStringA("Found a conversion matrix!"), m_Doc );
				break;// return new pdfOp(opType.op_cm, m_Doc);
			#pragma endregion 
					////////////////////////////////////////////////////////////////////////////////////////////////////////////
            #pragma region OP_Tw
			case GraphicsOps::op_Tw:
				_ASSERT( lstTokens.size() >= 2 );
				//_ASSERT(pTextItem != nullptr );
				i = lstTokens.size() - 2;

				//if (pTextItem != nullptr ) // outside of a BT is illegal - maybe later we could be forgiving and create a temp one just to use for "global" state settings
				pCurrState->GetTState().WordSpacing = lstTokens[i].RealF();

				lstTokens.clear();
				//OutputDebugStringA("Found a conversion matrix!"), m_Doc );
				break;// return new pdfOp(opType.op_cm, m_Doc);
			#pragma endregion 
					////////////////////////////////////////////////////////////////////////////////////////////////////////////
            #pragma region OP_Ts
			case GraphicsOps::op_Ts:
				_ASSERT( lstTokens.size() >= 2 );
				//_ASSERT(pTextItem != nullptr );
				i = lstTokens.size() - 2;

				//if (pTextItem != nullptr ) // outside of a BT is illegal - maybe later we could be forgiving and create a temp one just to use for "global" state settings
				pCurrState->GetTState().TextRise = lstTokens[i].RealF();

				lstTokens.clear();
				//OutputDebugStringA("Found a conversion matrix!"), m_Doc );
				break;// return new pdfOp(opType.op_cm, m_Doc);
				#pragma endregion 
					////////////////////////////////////////////////////////////////////////////////////////////////////////////
            #pragma region OP_Tr
			case GraphicsOps::op_Tr:
				_ASSERT( lstTokens.size() >= 2 );
				//_ASSERT(pTextItem != nullptr );
				i = lstTokens.size() - 2;

				//if (pTextItem != nullptr ) // outside of a BT is illegal - maybe later we could be forgiving and create a temp one just to use for "global" state settings
				pCurrState->GetTState().RenderMode = (pdfTextRenderMode)lstTokens[i].Int;

				lstTokens.clear();
				//OutputDebugStringA("Found a conversion matrix!"), m_Doc );
				break;// return new pdfOp(opType.op_cm, m_Doc);
			#pragma endregion 
					////////////////////////////////////////////////////////////////////////////////////////////////////////////
            #pragma region OP_Tz
			case GraphicsOps::op_Tz:
				_ASSERT( lstTokens.size() >= 2 );
				//_ASSERT(pTextItem != nullptr );
				i = lstTokens.size() - 2;

				//if (pTextItem != nullptr ) // outside of a BT is illegal - maybe later we could be forgiving and create a temp one just to use for "global" state settings
				pCurrState->GetTState().HScale = lstTokens[i].RealF() / 100.0F;
				pCurrState->GetTState().HScale = (pCurrState->GetTState().HScale != 1.0f && pCurrState->GetTState().HScale != 0.0f ? pCurrState->GetTState().HScale : 1);
				lstTokens.clear();
				//OutputDebugStringA("Found a conversion matrix!"), m_Doc );
				break;// return new pdfOp(opType.op_cm, m_Doc);
			#pragma endregion 
					////////////////////////////////////////////////////////////////////////////////////////////////////////////
            #pragma region OP_TL
			case GraphicsOps::op_TL:
				_ASSERT( lstTokens.size() >= 2 );
				//_ASSERT(pTextItem != nullptr );
				i = lstTokens.size() - 2;

				//if (pTextItem != nullptr ) // outside of a BT is illegal - maybe later we could be forgiving and create a temp one just to use for "global" state settings
				pCurrState->GetTState().Leading = lstTokens[i].RealF();

				lstTokens.clear();
				//OutputDebugStringA("Found a conversion matrix!"), m_Doc );
				break;// return new pdfOp(opType.op_cm, m_Doc);
				#pragma endregion 
					////////////////////////////////////////////////////////////////////////////////////////////////////////////
            #pragma region OP_T*
			case GraphicsOps::op_Tstar:
				//if (pTextItem != nullptr ) // outside of a BT is illegal - maybe later we could be forgiving and create a temp one just to use for "global" state settings
				pCurrState->GetTState().X = 0;
				pCurrState->GetTState().Y -= pCurrState->GetTState().Leading;

				lstTokens.clear();
				break;// return new pdfOp(opType.op_cm, m_Doc);
				#pragma endregion 
					////////////////////////////////////////////////////////////////////////////////////////////////////////////
            #pragma region OP_Tj
			case GraphicsOps::op_Tj:
				{
					_ASSERT( lstTokens.size() >= 2 );
					//_ASSERT(pTextItem != nullptr );
					i = lstTokens.size() - 2;

					// MAYBE IT WOULD BE BETTER TO SAVE UNTIL PARSING IS DONE TO MINIMIZE THE NUMBER OF TIMES WE GET Tjs where the matrix, etc. doesn't change.

					// here is where we'd best resolve thingsl matrix/points scaling.
					// so create a temp TState to do the job;
					pdfTState ts = pCurrState->GetTState();
					/*pdfTState ts = new pdfTState( pCurrState->GetTState() );
					float dx = (float)Math.Round(Math.Sqrt((ts.Mat.Elements[0] * ts.Mat.Elements[0]) + (ts.Mat.Elements[1] * ts.Mat.Elements[1])), 3);
					float dy = (float)Math.Round(Math.Sqrt((ts.Mat.Elements[2] * ts.Mat.Elements[2]) + (ts.Mat.Elements[3] * ts.Mat.Elements[3])), 3);
					ts.Points *= dy;// (float)Utils.GetMatrixVScale(ts.Mat);
					ts.X = (ts.X * ts.Points) + ts.Mat.Elements[4];
					ts.Y = (ts.Y * ts.Points) + ts.Mat.Elements[5];
					ts.Mat.Scale(1.0f / dx, 1.0f / dy);
					ts.Mat.Translate(-ts.Mat.Elements[4], -ts.Mat.Elements[5]);
					ts.Mat = new Matrix((float)Math.Round(ts.Mat.Elements[0], 3), (float)Math.Round(ts.Mat.Elements[1], 3), (float)Math.Round(ts.Mat.Elements[2], 3), (float)Math.Round(ts.Mat.Elements[3], 3), (float)Math.Round(ts.Mat.Elements[4], 3), (float)Math.Round(ts.Mat.Elements[5], 3));
					//ts.Mat.Multiply(new Matrix(1.0f / dx, 0, 0, 1.0f / dy, 0, 0));
					ts.Leading *= ts.Points;
					ts.WordSpacing *= ts.Points;
					ts.CharSpacing *= ts.Points;
					ts.TextRise *= ts.Points;*/

#if DEBUG
					if (ts.Font == null || ts.Font.Type == FontType.Unknown)
						OutputDebugStringA( "NO FONT!!!!!!!!!!!!!!!!!!!" );
#endif
					if (pTextItem != nullptr ) // outside of a BT is illegal - maybe later we could be forgiving and create a temp one just to use for "global" state settings
					{
						_ASSERT( lstTokens[i] is pdfString );

						pdfString str = lstTokens[i] as pdfString;
						if (ts.Font != null && str != nullptr )
						{
							if (ts.Font.Encoding != null &&
								ts.Font.Encoding.Name.Length > 0 &&
								(str.Str()ingType != StringEncoding::Byte && str.Str()ingType != StringEncoding::UTF16BE && str.Str()ingType != StringEncoding::UTF16LE))
							{
								//OutputDebugStringA("\t***{0} becoming {1}", str.Str()ingType, pdfString.NameToStringType(ts.Font.Encoding.Name));
								str.Str()ingType = pdfString.NameToStringType( ts.Font.Encoding.Name );
							}

							pdfTextRun lastRun = pTextItem.Count > 0 ? pTextItem[pTextItem.Count - 1] : null;
							pdfTextRun newRun = null;

							// can we concatentate with the last run? (same some space/performance)
							if (lastRun != null && sOp == sLastOp && lastRun.RawText != null && lastRun.RawText.Str()ingType == str.Str()ingType)
							{
								newRun = lastRun;

								byte[] bNew = new byte[newRun.RawText.m_HexBytes.Length + str.m_HexBytes.Length];
								newRun.RawText.m_HexBytes.CopyTo( bNew, 0 );
								str.m_HexBytes.CopyTo( bNew, newRun.RawText.m_HexBytes.Length );
								newRun.RawText.m_HexBytes = bNew;
							}
							else
							{
								newRun = new pdfTextRun( pCurrState->GetGState(), ts );
								pTextItem.m_runs.Add( newRun );
								newRun.RawText = str;
							}
						}
					}

					lstTokens.clear();
				}
				break;
				#pragma endregion 
					////////////////////////////////////////////////////////////////////////////////////////////////////////////
            #pragma region OP_TJ
			case GraphicsOps::op_TJ:
				{
					// here is where we'd best resolve thingsl matrix/points scaling.
					// so create a temp TState to do the job;
					//pdfTState ts = pCurrState->GetTState();
					pdfTState ts = new pdfTState( pCurrState->GetTState() );
					/*float dx = (float)Math.Round( Math.Sqrt( ( ts.Mat.Elements[ 0 ] * ts.Mat.Elements[ 0 ] ) + ( ts.Mat.Elements[ 1 ] * ts.Mat.Elements[ 1 ] ) ), 3 );
					float dy = (float)Math.Round(Math.Sqrt((ts.Mat.Elements[2] * ts.Mat.Elements[2]) + (ts.Mat.Elements[3] * ts.Mat.Elements[3])), 3);
					ts.Points *= dy;// (float)Utils.GetMatrixVScale(ts.Mat);
					ts.X = (ts.X * ts.Points) + ts.Mat.Elements[4];
					ts.Y = (ts.Y * ts.Points) + ts.Mat.Elements[5];
					ts.Mat.Scale(1.0f / dx, 1.0f / dy);
					ts.Mat.Translate(-ts.Mat.Elements[4], -ts.Mat.Elements[5]);
					ts.Mat = new Matrix((float)Math.Round(ts.Mat.Elements[0], 3), (float)Math.Round(ts.Mat.Elements[1], 3), (float)Math.Round(ts.Mat.Elements[2], 3), (float)Math.Round(ts.Mat.Elements[3], 3), (float)Math.Round(ts.Mat.Elements[4], 3), (float)Math.Round(ts.Mat.Elements[5], 3));
					//ts.Mat.Multiply(new Matrix(1.0f / dx, 0, 0, 1.0f / dy, 0, 0));
					ts.Leading *= ts.Points;
					ts.WordSpacing *= ts.Points;
					ts.CharSpacing *= ts.Points;
					ts.TextRise *= ts.Points;*/



					// CONVERT INTO MULTIPLE RUNS, ONLY CREATE ARRAYS WHEN CREATING OUTPUT STREAM


					_ASSERT( lstTokens.size() >= 2 );
					//_ASSERT(pTextItem != nullptr );
					i = lstTokens.size() - 2;
					//_ASSERT(false);
					if (pTextItem != nullptr ) // outside of a BT is illegal - maybe later we could be forgiving and create a temp one just to use for "global" state settings
					{
						//pdfTextItem.TextRun newRun = new pdfTextItem.TextRun(pCurrState->GetGState(), ts);
						////
						//newRun.TextArray = new List<object>();// lstTokens[i].Str();
						//pdfArray atmArr = lstTokens[i] as pdfArray;
						//if (atmArr != null && atmArr.Count > 0)
						//{
						//    for (int x = 0; x < atmArr.Count; x++)
						//        if (atmArr[x].IsNumber)
						//            newRun.TextArray.Add(atmArr[x].RealF() / 1000.0f * dx);
						//        else
						//            if (atmArr[x] is pdfString)
						//            {
						//                pdfString s = atmArr[x] as pdfString;
						//                if (s.IsHex)
						//                {
						//                    if (s.HexBytes != nullptr )
						//                        newRun.TextArray.Add(s.HexBytes.Clone());
						//                }
						//                else
						//                    newRun.TextArray.Add(s.Value);
						//            }
						//}

						//pTextItem.m_runs.Add(newRun);


						//
						pdfArray atmArr = lstTokens[i] as pdfArray;
						if (atmArr != null && atmArr.Count > 0)
						{
							for (int x = 0; x < atmArr.Count; x++)
							{
								if (atmArr[x].IsNumber)
								{
									ts.X -= (atmArr[x].RealF() / 1000.0f / ts.Mat.ScaleX);//ts.Points);
								}
								else if (atmArr[x] is pdfString)
								{

									pdfTextRun newRun = new pdfTextRun( pCurrState->GetGState(), ts );

									newRun.RawText = atmArr[x] as pdfString;//.Text = ExtractText(atmArr[x] as pdfString, ts);

																			//if (ts.Font.Encoding.Name.Length > 0 &&
																			//    (newRun.RawText.Str()ingType != StringEncoding::Byte && newRun.RawText.Str()ingType != StringEncoding::UTF16BE && newRun.RawText.Str()ingType != StringEncoding::UTF16LE))
																			//{
																			//    //OutputDebugStringA("\t***{0} becoming {1}", newRun.RawText.Str()ingType, pdfString.NameToStringType(ts.Font.Encoding.Name));
																			//    newRun.RawText.Str()ingType = pdfString.NameToStringType(ts.Font.Encoding.Name);
																			//}

									pTextItem.m_runs.Add( newRun );

									// for the next section.
									ts.X += (float)ts.Font.MeasureString( newRun.RawText, ts ).Width; ;
								}
							}
						}

					}

					lstTokens.clear();
				}
				break;
			#pragma endregion 

            #pragma region OP '
			case GraphicsOps::op_quote:
				_ASSERT( lstTokens.size() >= 3 );
				i = lstTokens.size() - 3;

				//if (pTextItem != nullptr ) // outside of a BT is illegal - maybe later we could be forgiving and create a temp one just to use for "global" state settings
				pCurrState->GetTState().Y -= pCurrState->GetTState().Leading;
				lstTokens.RemoveAt( 0 );
				sOp = Key_Ops.Tj;
				goto tryagain;

				//lstTokens.clear();
				break;// return new pdfOp(opType.op_cm, m_Doc);
			#pragma endregion 

            #pragma region OP "
			case GraphicsOps::op_dblquote:
				_ASSERT( lstTokens.size() >= 5 );
				i = lstTokens.size() - 5;

				//if (pTextItem != nullptr ) // outside of a BT is illegal - maybe later we could be forgiving and create a temp one just to use for "global" state settings
				pCurrState->GetTState().WordSpacing = lstTokens[i].RealF();
				pCurrState->GetTState().CharSpacing = lstTokens[i + 1].RealF();// / 1000.0f * pCurrState->GetTState().Mat.ScaleX;
				lstTokens.RemoveAt( 0 );
				lstTokens.RemoveAt( 0 );
				sOp = "'";
				goto tryagain;

				//lstTokens.clear();
				break;// return new pdfOp(opType.op_cm, m_Doc);
				#pragma endregion 
					////////////////////////////////////////////////////////////////////////////////////////////////////////////
					#pragma endregion 

			////////////////////////////////////////////////////////////////////////////////////////////////////////////
            #pragma region Marked Content
					////////////////////////////////////////////////////////////////////////////////////////////////////////////
			#pragma region BMC
			case GraphicsOps::op_BMC: // start a new marked content - should NOT have properties associates with it.
				_ASSERT( lstTokens.size() >= 2 );
				i = lstTokens.size() - 2;
				{
					pdfContainerItem pNew = new pdfContainerItem( pCurrState->GetGState() );
					lstObjects.push_back( pNew );
					pNew.Tag = lstTokens[i].Str();
					stm.Position -= 2; // move back two spaces so the reader will be sync'ed properly.
					pNew.m_Contents = new pdfContents( m_Doc );
					pNew.m_Contents.GraphicsStack = stack;
					pNew.m_Contents.InitContainer( stm, m_dictRes );

					// now resync us
					Cursor = (int)stm.Position - 2;
				}

				// Set a marker so we know how to transition graphics states when/if inserting new objects 
				//lstObjects.push_back( new pdfMarkerItem( pCurrState->GetGState(), pdfMarker.BeginContainer ) );

				lstTokens.clear();
				break;// return new pdfOp(opType.opDo, m_Doc);
			#pragma endregion 
					////////////////////////////////////////////////////////////////////////////////////////////////////////////
			#pragma region BDC
			case GraphicsOps::op_BDC: // start a new marked content - should have properties associates with it.
			_ASSERT( lstTokens.size() >= 3 );
			i = lstTokens.size() - 3;

			{
				pdfContainerItem pNew = new pdfContainerItem( pCurrState->GetGState() );
				lstObjects.push_back( pNew );

				//
				if (lstTokens[i + 1] is pdfName)
					pNew.Properties = FindBDCProp( lstTokens[i + 1].Str(), m_dictRes );
				else if (lstTokens[i + 1] is pdfDictionary)
					pNew.Properties = lstTokens[i + 1] as pdfDictionary;
				else
					_ASSERT( false );

				//
				pNew.Tag = lstTokens[i].Str();
				stm.Position -= 2; // move back two spaces so the reader will be sync'ed properly.
				pNew.m_Contents = new pdfContents( m_Doc );
				pNew.m_Contents.GraphicsStack = stack;
				pNew.m_Contents.InitContainer( stm, m_dictRes );
				//pNew.m_Contents.Atom = pNew.Properties;

				// now resync us
				Cursor = (int)stm.Position - 2;

			}

			lstTokens.clear();

			// Set a marker so we know how to transition graphics states when/if inserting new objects 
			//lstObjects.push_back( new pdfMarkerItem( pCurrState->GetGState(), pdfMarker.BeginContainer ) );
			break;// return new pdfOp(opType.opDo, m_Doc);
			#pragma endregion 
				////////////////////////////////////////////////////////////////////////////////////////////////////////////
			#pragma region EMC
			case GraphicsOps::op_EMC:
				//case GraphicsOps::op_EDC:
				// end the marked content - for us to see this means this
				// instance of pdfCOntentReader is being used via some outer pdfContainerItem.m_Cont object.
				// which means we should bail out since we're done building it.!
				lstTokens.clear();

				// Set a marker so we know how to transition graphics states when/if inserting new objects 
				//lstObjects.push_back( new pdfMarkerItem( pCurrState->GetGState(), pdfMarker.EndContainer ) );
				return lstObjects;
			#pragma endregion 
					////////////////////////////////////////////////////////////////////////////////////////////////////////////
			#pragma region Marked Content Point
				case GraphicsOps::op_MP: // start a new marked content - should NOT have properties associates with it.
				_ASSERT( lstTokens.size() >= 2 );
				i = lstTokens.size() - 2;
				{
					pdfMarkedContentPointItem pNew = new pdfMarkedContentPointItem( pCurrState->GetGState() );
					pNew.Tag = lstTokens[i].Str();

					lstObjects.push_back( pNew );
				}

				lstTokens.clear();

				// Set a marker so we know how to transition graphics states when/if inserting new objects 
				//lstObjects.push_back( new pdfMarkerItem( pCurrState->GetGState(), pdfMarker.ContainerPoint ) );
				break;// return new pdfOp(opType.opDo, m_Doc);
			#pragma endregion 

			////////////////////////////////////////////////////////////////////////////////////////////////////////////
			#pragma region Marked Content Point With Properties
			case GraphicsOps::op_DP: // start a new marked content - should have properties associates with it.
				_ASSERT( lstTokens.size() >= 3 );
				i = lstTokens.size() - 3;
				{
					pdfMarkedContentPointItem pNew = new pdfMarkedContentPointItem( pCurrState->GetGState() );
					pNew.Tag = lstTokens[i].Str();

					//
					if (lstTokens[i + 1] is pdfName)
						pNew.Properties = FindBDCProp( lstTokens[i + 1].Str(), m_dictRes );
					else if (lstTokens[i + 1] is pdfDictionary)
						pNew.Properties = lstTokens[i + 1] as pdfDictionary;
					else
						_ASSERT( false );

					lstObjects.push_back( pNew );
				}

				lstTokens.clear();

				// Set a marker so we know how to transition graphics states when/if inserting new objects 
				//lstObjects.push_back( new pdfMarkerItem( pCurrState->GetGState(), pdfMarker.ContainerPoint ) );
				break;// return new pdfOp(opType.opDo, m_Doc);
			#pragma endregion 

			#pragma endregion 

			default:
				OutputDebugStringA( "\nUNHANDLED OPERATOR: " );
				OutputDebugStringA( sOp.c_str() );
				OutputDebugStringA( "\n" );

				//Debug.IndentLevel++;
				/*foreach( pdfAtom a in lstTokens )
				{
					OutputDebugStringA( a.Obj.ToString() );
				}*/
				//Debug.IndentLevel--;
				lstTokens.clear();
				break;
			}

			//
			sLastOp = sOp;
		}
	}

	// make sure no objects were half-baked;
	_ASSERT( pTextItem == nullptr );
	_ASSERT( pPathItem == nullptr );

	//OutputDebugStringA("{0} hanging ops\t{1:G} Items added", lstTokens.size(), lstObjects.Count );
	return lstObjects;
}


};
