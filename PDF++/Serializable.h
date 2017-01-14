#pragma once
#include "ExportDef.h"

#include <iosfwd>

namespace Pdf{

class PDF_API CSerializable
{
	//friend std::ostream& operator<<( std::ostream& stm, const CSerializable& atm );
public:
	virtual void Serialize( std::ostream& stm )
	{
	};
	virtual void Deserialize( std::ostream& stm )
	{
	};
};

}; // namespace Pdf

PDF_API std::ostream& operator<<( std::ostream& stm, const Pdf::CSerializable& c );

PDF_API std::ostream& operator>>( std::ostream& stm, Pdf::CSerializable& c );
