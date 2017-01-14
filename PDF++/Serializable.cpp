#include "stdafx.h"
#include "Serializable.h"

namespace Pdf{

};

std::ostream& operator<<( std::ostream& stm, const Pdf::CSerializable& c )
{
	const_cast<Pdf::CSerializable&>(c).Serialize( stm );
	return stm;
}

std::ostream& operator>>( std::ostream& stm, Pdf::CSerializable& c )
{
	c.Deserialize( stm );
	return stm;
}



//#include <sstream> 
//
//using namespace std;
//
//class zstream : public ostream
//{
//   public:
//      zstream()
//        : ostream(cout.rdbuf())
//        , displayedHeader(false)
//      { }
//      void displayHeader()
//      {
//         if(false == displayedHeader)
//         {
//            *this << "HEADER" << endl;
//            displayedHeader = true;
//         }
//      }
//   private:
//      bool displayedHeader;
//};
//
//template <typename T>
//zstream& operator<<(zstream& z, const T& v)
//{
//  z.displayHeader();
//  static_cast<std::ostream&>(z) << v;
//  return myo;
//}
