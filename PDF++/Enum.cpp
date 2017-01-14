#include "stdafx.h"
#include "Enum.h"

std::ostream& operator<<( std::ostream& stm, const Pdf::AtomType& t )
{
	switch(t)
	{
	case Pdf::AtomType::Null:
		stm << "Null";
		break;
	case Pdf::AtomType::Integer:
		stm << "Integer";
		break;
	case Pdf::AtomType::Integer64:
		stm << "Integer64";
		break;
	case Pdf::AtomType::Real:
		stm << "Real";
		break;
	case Pdf::AtomType::Boolean:
		stm << "Boolean";
		break;
	case Pdf::AtomType::String:
		stm << "String";
		break;
	case Pdf::AtomType::Name:
		stm << "Name";
		break;
	case Pdf::AtomType::Dictionary:
		stm << "Dictionary";
		break;
	case Pdf::AtomType::Stream:
		stm << "Stream";
		break;
	case Pdf::AtomType::Array:
		stm << "Array";
		break;
	case Pdf::AtomType::Keyword:
		stm << "Keyword";
		break;
	case Pdf::AtomType::Reference:
		stm << "Reference";
		break;
	}
	return stm;
}

PDF_API std::ostream& operator<<( std::ostream& stm, const Pdf::FilterType& t )
{
	switch(t)
	{
	case Pdf::FilterType::NoFilter:
		stm << "NoFilter";
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
	return stm;
}

PDF_API std::ostream& operator<<( std::ostream& stm, const Pdf::OutputStyle& t )
{
	switch(t)
	{
	case Pdf::OutputStyle::NicelyFormatted:
		stm << "NicelyFormatted";
		break;
	case Pdf::OutputStyle::Neat:
		stm << "Neat";
		break;
	case Pdf::OutputStyle::Conservative:
		stm << "Conservative";
		break;
	case Pdf::OutputStyle::Stingy:
		stm << "Stingy";
		break;
	}
	return stm;
}
