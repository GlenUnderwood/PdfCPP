#include "stdafx.h"
#include "Model.h"
#include "Document.h"
#include "Filters.h"
#include "FlateFilter.h"
#include "CCITTFilter.h"

namespace Pdf {
namespace Filter {

pdfFilters::pdfFilters(void)
{
	// TODO:  gotta release these where appropriate  or..... change pdfFilter to just store a enum and make the righ class to encode/encode on the fly
	m_lst.reset( new std::list<pdfFilter*>() ); 
}

pdfFilters::pdfFilters(const pdfDictionary& dictFilters)
{
	m_lst.reset( new std::list<pdfFilter*>() );

	pdfDictionary& dictSrc( const_cast<pdfDictionary&>(dictFilters) );

	if( dictSrc.Has("Filter") )
	{
		pdfAtom atmFilters( dictSrc[ "Filter" ] );

		AtomType t = atmFilters.GetType();
		if (t != Null)
		{
			try
			{
				if (t == AtomType::Array )
				{
					pdfArray arr( atmFilters );
					if (arr.GetCount() > 0)
					{
						//arr.ForEach( [] (const pdfAtom& atmRef) { std::cout << atmRef << std::endl; }  );

						arr.ForEach( [this] (const pdfAtom& atmRef) { 
								//std::cout << atmRef << std::endl;
								pdfAtom atm( atmRef.Dereference() );
								if (atm.GetType() == Name)
									m_lst->push_front( FilterFromName(atm.Str().c_str()) );
									//Add( atm.Str().c_str() );
							});
						//m_lst->reverse();
					}
				//	newFilters.m_lst.Re
				}
				else if (t == AtomType::Name)
				{
					if (atmFilters.Str().size() > 0)
					{
						m_lst->push_back( FilterFromName(atmFilters.Str().c_str()) );
					}
				}
				else
				{
					//newFilters.m_lst.Add(new pdfFilterNone());
				}
			}
			catch( const std::exception& e)
			{
#ifdef _DEBUG
				std::clog << e.what();
#endif
				//Debug.Print("pdfFilters.GetFilters: ERROR: " + e.Message);
			}

			// Some filters require a little extra information (which we can provide if dictSrc is an image, which are the only ones that have DecodeParms
			pdfDictionary dictDecodeParms(dictSrc["DecodeParms"]);
			if (m_lst->size()>0 && !dictDecodeParms.IsNull() )
			{
#ifdef _DEBUG
				//dictDecodeParms.ForEach( [&] ( const char* szKey, pdfAtom& a ) {std::cout << "\"" << szKey << "\" : " << a << std::endl; });
#endif
				for( auto it = m_lst->begin(); it != m_lst->end(); ++it )
				{
					///////////////
					if ( (*it)->GetType() == CCITTFaxDecode )
					{
						pdfCCITT* fax = reinterpret_cast<pdfCCITT*>( (*it) );
						if (dictDecodeParms.Has("K"))
							fax->SetK( dictDecodeParms["K"].Int() );
						else
							fax->SetK( -1 );
						fax->SetBPP( 1 );
						if (dictDecodeParms.Has("Columns"))
							fax->SetWidth( dictDecodeParms["Columns"].Int() );
						else
							fax->SetWidth( dictSrc["Width"].Int() );
						if (dictDecodeParms.Has("Rows"))
							fax->SetHeight( dictDecodeParms["Rows"].Int() );
						else
							fax->SetHeight( dictSrc["Height"].Int() );
						break;
					}
				}				
			}
		}
	}
}


pdfFilters::~pdfFilters(void)
{
}


/*
	case Pdf::FilterType::NoFilter:
		break;
	case Pdf::FilterType::FlateDecode:
		stm << "FlateDecode";
		break;
	case Pdf::FilterType::CCITTFaxDecode:
		stm << "CCITTFaxDecode";
		break;
	case Pdf::FilterType::DCTDecode:
		stm << "DCTDecode";
		break;
	case Pdf::FilterType::LZWDecode:
		stm << "LZWDecode";
		break;
	case Pdf::FilterType::ASCII85Decode:
		stm << "ASCII85Decode";
		break;
	case Pdf::FilterType::ASCIIHexDecode:
		stm << "ASCIIHexDecode";
		break;
	}
}
*/

/*char toLower(char a)
{
    return std::tolower(a);
}*/

pdfFilter* pdfFilters::FilterFromName( const char* szFilter )
{
	if( szFilter == nullptr || szFilter[0] == 0 )
		throw pdfException("No filter type specified." );

	char szBuff[64]="";
#ifdef __GNUC__

	std::string sTmp( szFilter );
	std::string sFilter;
	std::transform(	sTmp.begin(),sTmp.end(),
					std::back_inserter(sFilter),
					[] (char a) { return tolower(a); });

	if( sFilter == std::string("flatedecode") || sFilter == std::string("fl")  )
		return new pdfFlate();
	else if( sFilter == std::string("ccittfaxdecode") || sFilter == std::string("ccf") )
		return new pdfCCITT();
	/*ielse if( strcasecmp(szFilter, "DCTDecode") ==0 || strcasecmp(szFilter, "DCT") ==0 )
		return pdfFilterDCT();
	else if( strcasecmp(szFilter, "LZWDecode") ==0 || strcasecmp(szFilter, "LZW") ==0 )
		return pdfFilterLZW();
	else if( _stricmp(szFilter, "ASCII85Decode") ==0 || strcasecmp(szFilter, "A85") ==0 )
		return strcasecmp();
	else if( strcasecmp(szFilter, "ASCIIHexDecode") ==0 || strcasecmp(szFilter, "A85") ==0 )
		return pdfFilterASCIIHex();*/
	sprintf( szBuff,  "Unhandled Filter Type: '%s'", szFilter );
#else
	if( ::_stricmp(szFilter, "FlateDecode") ==0 || ::_stricmp(szFilter, "Fl") ==0 )
		return new pdfFlate();
	else if( _stricmp(szFilter, "CCITTFaxDecode") ==0 || _stricmp(szFilter, "CCF") ==0 )
		return new pdfCCITT();
	/*else if( _stricmp(szFilter, "DCTDecode") ==0 || stricmp(szFilter, "DCT") ==0 )
		return pdfFilterDCT();
	else if( _stricmp(szFilter, "LZWDecode") ==0 || stricmp(szFilter, "LZW") ==0 )
		return pdfFilterLZW();
	else if( _stricmp(szFilter, "ASCII85Decode") ==0 || stricmp(szFilter, "A85") ==0 )
		return pdfFilterASCII85();
	else if( _stricmp(szFilter, "ASCIIHexDecode") ==0 || stricmp(szFilter, "A85") ==0 )
		return pdfFilterASCIIHex();*/
	sprintf_s( szBuff, 64*sizeof(szBuff[0]), "Unhandled Filter Type: '%s'", szFilter );
#endif
	throw pdfException(szBuff);
}
/*
pdfFilters pdfFilters::GetFilters(const pdfBlob& dictSrc)// can be an image or any stream obejct
{
	pdfFilters newFilters;
	if( !dictSrc.Has("Filter") )
		return newFilters;

	pdfAtom atmFilters( dictSrc["Filter"] );

	AtomType t = atmFilters.GetType();
	if (t != Null)
	{
		if (t == AtomType::Array )
		{
			pdfArray arr( atmFilters );
			if (arr.GetCount() > 0)
			{
				foreach ( pdfAtom atmRef in arr.m_lst )
				{
					pdfAtom atm = atmRef.Dereference;
					if (atm is pdfName)
						newFilters.Add(FilterFromName(atm.Str));
				}
				newFilters.m_lst.Reverse();
			}
		//	newFilters.m_lst.Re
		}
		else if (t == AtomType::Name)
		{
			if (atmFilters.Str().size() > 0)
			{
				pdfFilter f = null;
				try
				{
					f = FilterFromName(atmFilters.Str().c_str());
				}
				catch( const std::exception& e)
				{
					//Debug.Print("pdfFilters.GetFilters: ERROR: " + e.Message);
				}
				if (f != null)
				{
					newFilters.Add(f);
				}
			}
		}
		else
		{
			//newFilters.m_lst.Add(new pdfFilterNone());
		}

		// Some filters require a little extra information (which we can provide if dictSrc is an image, which are the only ones that have DecodeParms
		pdfDictionary dictDecodeParms = dictSrc[Keys.DecodeParms] as pdfDictionary;
		if (newFilters!= null && dictDecodeParms != null)
		{
			foreach (pdfFilter filter in newFilters.m_lst)
			{
				///////////////
				if (filter is pdfCCITT)
				{
					pdfCCITT fax = filter as pdfCCITT;
					if (dictDecodeParms.Has("K"))
						fax.K = dictDecodeParms["K"].Int;
					else
						fax.K = -1;
					fax.BPP = 1;
					if (dictDecodeParms.Has("Columns"))
						fax.Width = dictDecodeParms["Columns"].Int;
					else
						fax.Width = dictSrc["Width"].Int;
					if (dictDecodeParms.Has("Rows"))
						fax.Height = dictDecodeParms["Rows"].Int;
					else
						fax.Height = dictSrc["Height"].Int;
					break;
				}
			}
		}
	}

	return newFilters;
}
*/

// pdfFilter
void pdfFilters::Transform( const CSharedBuff& buffSrc, CSharedBuff& buffDest, bool bDecode )
{
	buffDest.Clear();
	if (m_lst.get() != nullptr && m_lst->size() > 0)
	{
		CSharedBuff tmpSrc(buffSrc);
		CSharedBuff tmpDest;
		if( !bDecode )
		{
			for( auto it = m_lst->begin(); it != m_lst->end(); ++it )
			{
				pdfFilter* pFilter = *it; 
				pFilter->Transform( tmpSrc, tmpDest,bDecode );
				//bytesOut = fil.Encode(bytesOut);
				tmpSrc = tmpDest;
				tmpDest.Clear();
			}
		}
		else
		{
			//notice we go backward
			for( auto it = m_lst->rbegin(); it != m_lst->rend(); ++it )
			{
				pdfFilter* pFilter = *it; 
				pFilter->Transform( tmpSrc, tmpDest, bDecode );
				//bytesOut = fil.Decode(bytesOut);
				tmpSrc = tmpDest;
				tmpDest.Clear();
			}
		}
		buffDest = tmpSrc;
	}
}

//  TODO make buffsrc a istream
void pdfFilters::Transform( const CSharedBuff& buffSrc, std::ostream& buffDest, bool bDecode )
{
	// TODO Implement
	if( !bDecode )
	{
	}
	else
	{
	}
}
//
//pdfAtom pdfFilters::CreateObj( pdfDocument* doc )
//{
//	if (doc != nullptr && m_lst != nullptr && m_lst->size() > 0)
//	{
//		if (m_lst->size() == 1)
//		{
//			return doc->CreateName(m_lst->front()->ToString(),false);
//		}
//		else
//		{
//			pdfArray arr( doc->CreateArray( m_lst->size(), false ));
//
//			//notice we go backward
//			for( auto it = m_lst->rbegin(); it != m_lst->rend(); ++it )
//				arr.Add( doc->CreateName( (*it)->ToString(), false) );
//
//			return arr;
//		}
//	}
//
//	return nullptr;
//}

void pdfFilters::Add(const char* sFilter)
{
	m_lst->push_back( FilterFromName(sFilter) );
}
/*inline void pdfFilters::Add(const pdfFilter& pFilter)
{
	m_lst.push_back(pFilter);
}*/
size_t pdfFilters::GetCount() const
{
	return m_lst->size();
}
/*inline pdfFilter& pdfFilters::GetFilter(int idx)
{
	return m_lst[idx];
}*/

}; //namespace Filter 
}; //namespace Pdf 
