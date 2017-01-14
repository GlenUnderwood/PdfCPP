// TestLib.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <sstream> 

using namespace std;

#include "..\PDF++\PDF++.h"
#include "..\PDF++\FlateFilter.h"
#include "..\PDF++\Writer.h"
#include "..\PDF++\StdSecurity.h"

using namespace Pdf;
using namespace Pdf::Exception;
using namespace Pdf::Geometry;






//
pdfDocument CreateDoc()
{
	pdfDocument doc = pdfDocument::NewDoc();

	pdfDictionary d = doc.Catalog();
pdfInt in;
in=nullptr;

	if( !d )
	{
		d=doc.Catalog();
	}
	if( d )
	{
		d=nullptr;
	}

	pdfPages pgs(doc.Pages());
	pdfPage pg = pgs.Add();
	//pg.Contents().

	pgs.Add();
	pgs.Add();
	pgs.Add();
	pgs.Add();
	pgs.Add();

	// TODO Why is this possible - CDocData shouldn't be getting exported.
	string s = doc.GetData()->m_filename;


	Pdf::pdfColor clr( 1.0f, 1.0f, .0, .0 );
	unsigned long c = clr.GetARGB();
	unsigned long cc = clr.GetRGB();
	c = Pdf::pdfColor::ToGrayScale(0.5f, .5, 1.0 );
	clr.SetColorSpaceType( ColorSpaceType::DeviceCMYK );

	clr = pdfColor(RGB(255,127,127), false);
	

	Pdf::Security::pdfStdSecurity sec = Pdf::Security::pdfStdSecurity::NewSecurity( doc, "user", nullptr, Pdf::All, true );

	doc.SetSecurity( &sec );

	
	////////////////////
	pdfWriter w;
	//w.SetOutputStyle( Pdf::Stingy );
	w.Write( doc, clog );
	doc.Close();

	//delete sec;
	return doc;
}



#include <set>
std::set<Pdf::pdfAtom> g_setDumped;

void Dump( pdfAtom& atom, std::ostream& stm, int indent ) //throw(Pdf::Exception::pdfException)
{
	// this prevents these three types from emitting from anywhere other than the root.  (Just for items in the xref table.)
	pdfAtom atmRef = atom.GetRef();
	/*if( atmRef != nullptr && indent > 1 &&  (atom.GetType()==AtomType::Dictionary || atom.GetType()==AtomType::Stream || atom.GetType()==AtomType::Array) )
	{
		stm << atom.GetType() << " <see xref " << atmRef.GetId()<< " R >" << std::endl;
		return;
	}*/
	
	if( g_setDumped.find (atom) != g_setDumped.end() )
	{
		stm << atom.GetType() << " <emitted>" << std::endl;
		//stm << atom.GetType() << " <emitted, " << (atmRef !=nullptr ? atmRef.GetId() : atom.GetId()) << " R >" << std::endl;
		return;
	}
	g_setDumped.insert( atom );

	stm << atom << std::endl;

	// indentation
	char* sz = new char[indent+2];
	memset( sz, '\t', indent+1);
	sz[indent+1]=0;

	if( atom.GetType()==AtomType::Dictionary || atom.GetType()==AtomType::Stream )
	{
		pdfDictionary d( atom );

		d.ForEach( [&] ( const char* szKey, Pdf::pdfAtom& a )
			{
				stm << sz << "'" << szKey << "': ";// << a << std::endl;
				Dump( a, stm, indent + 1 );
			});

		if( atom.GetType()==AtomType::Stream )
		{
			///CSharedBuff buff ;
			//pdfBlob blob ( atom );
			//blob.GetBytes( buff, false );
//#ifdef _DEBUG
//			std::ofstream fout;
//			try
//			{
//				fout.open( "C:\\users\\glen\\desktop\\Contents.tif", std::ios::out | std::ios::trunc);
//				fout.write( (const char*)buff.get(), buff.GetLength() );
//				fout.flush();
//			}
//			catch(...)
//			{
//			}
//			if( fout.is_open() )
//				fout.close();
//#endif
		}
	}
	else if( atom.GetType()==AtomType::Array )
	{
		int idx = 0;
		pdfArray d( atom );
		d.ForEach( [&] ( Pdf::pdfAtom& a )
			{
				stm << sz << "#" << ++idx << ": ";// << a << std::endl;
				Dump( a, stm, indent + 1 );
			});
	}
	delete [] sz;
}

void Dump( pdfDocument& doc, std::ostream& stm )
{
	auto delDumpDictionary = [&] ( const char* szKey, Pdf::pdfAtom& a )
			{
				stm << "\t'" << szKey << "': " ;
				Dump( a, stm, 1 );
			};
	auto delDumpRefs = [&] ( const Pdf::pdfObjId& id, Pdf::pdfReference& a )
			{
				stm << '\t' << "'" << id << "' : ";
				if( id.IsFree() )
					stm << "Free reference"  << std::endl;
				else
				{
					pdfAtom atm = a.Dereference();
					Dump( atm, stm, 1 );
				}
			};

	try
	{
		stm << "\nCrossreference:" << std::endl;
		doc.CrossReference().ForEach( delDumpRefs );

		stm << "\nTrailer:" << std::endl;
		doc.Trailer().ForEach( delDumpDictionary );

	}
	catch( const pdfException& ex )
	{
		clog << "PDF error: " << ex << endl;
	}
	catch( const exception& ex )
	{
		clog << ex.what() << endl;
	}
	catch(...)
	{
		//clog << "UNHANDLED ERROR!!" << endl;
		throw;
	}


	g_setDumped.clear();
}

//#include <functional>

class CDummy
{
public:
	CDummy()
	{
		clog << "CDummy being created\n";
	};
	~CDummy()
	{
		clog << "CDummy being DESTROYED\n";
	}
};

void Foo()// throw(std::exception)
{
	CDummy dummy;

	throw std::exception();
}

int main(int argc, char* argv[])
{
	//try
	//{
	//	Foo();
	//}
	//catch(...)
	//{
	//	clog << "UNHANDLED ERROR!!" << endl;
	//	//throw;
	//}

	//return 0;
	pdfDocument doc;
	//pdfDocument doc2(doc);
	//pdfDictionary tr, tr2;

	// Log to a file instead of the console.
	// Basically, the delegate will get called when holdBuff goes out of scope, sending the streambuf* we gave to its constructor
	auto del = [&] (streambuf* p) { std::clog.rdbuf(p); };
	unique_ptr<streambuf,decltype(del)> holdBuff( std::clog.rdbuf(), del );// returns the original pointer of clog to what it was
	
	//std::stringbuf tmpBuf;
	std::stringbuf tmpBuf;
#ifdef _WINDOWS
	std::ofstream fLog("e:\\Projects\\PDFcpp\\PDF++\\Pdf++.Txt");
#else
	std::ofstream fLog("$/Desktop/Pdf++.Txt");
#endif
	//std::clog.rdbuf(&tmpBuf);
	std::clog.rdbuf(fLog.rdbuf());
	// Log to a file instead of the console.



	CreateDoc();
	
	
	return 0;

	// TODO won'y compile in GNU
	Pdf::byte b;
	Pdf::bytestream bs(10);

	/* TEST Flate
	CSharedBuff buff;
	buff.Resize( 60 );
	strcpy( (char*)buff.get(), "Hello there !!!!!!!");
	Pdf::Filter::pdfFlate filter;
	stringstream s ;
	filter.Transform( buff, (std::ostream&)s , false );
	if( s.tellp() > 0 )
	{
		int x = s.tellp() ;
		buff.Resize(x );
		s.seekg(0, std::ios::beg);
		s.read( (char*)buff.get(), buff.GetLength() );
		filter.Transform( buff, std::clog , true ); // ought to equal  "Hello there !!!!!!!"
	}

	*/
#ifdef unix
#endif


	//std::clog << "XYZ";
	//doc=doc2; // compiler shouldn't allow this
	try
	{//C:\Projects\PDF\TestApps\TestPDFMgr\\BrandNew.pdf
		//pdoc->Open( "C:\\Projects\\PDF\\TestApps\\TestPDFMgr\\BrandNew.pdf", "password" );
		//doc.Open( "C:\\Projects\\PDF\\TestApps\\TestPDFMgr\\G4.pdf", "password" );
		doc.Open( "C:\\Projects\\PDF\\TestApps\\TestPDFMgr\\ImageWithSoftMask.pdf", "password" );
		//doc.Open( "C:\\Projects\\TestFiles\\Security\\Enc-NeedsOwnerPwd-u o.pdf", "o" );
		//doc.Open( "C:\\Projects\\TestFiles\\Encrypted\\_SecurityTest_OwnerAndUserPwd.pdf", "user" );
		//doc.Open( "C:\\Projects\\Testfiles\\Blank.pdf" , "" );
		// C:\Projects\TestFiles\Security
		pdfPage pg( doc.Pages()[0]);

		pdfPage pg2;

		pg2 = pg;

		pdfRect rcMedia( pg.GetMediaBox() );
		pdfRect rcCrop( pg.GetCropBox() );
		//pdfContents dCont( pg.Contents() );
		//pdfResources dRes( pg.Resources() );


		pdfDocument doc2(doc);

		//doc.CrossReference() = doc2.CrossReference(); // compiler shouldn't allow this
		/*
		CSharedBuff buff,buff2;
		if( dCont.GetType() == AtomType::Array )
		{
			pdfArray arr(dCont);
			pdfBlob blob ( arr[0] );
			pdfBlob blob2 ( arr[1] );
			blob.GetBytes( buff, false );
			blob2.GetBytes( buff2, false );


		}
		else if( dCont.IsDictionary() )
		{
			pdfBlob blob(dCont);
			blob.GetBytes( buff, false );


		}
		byte* p = buff.get();*/
		//tr2 = tr


		pdfWriter w;
		//w.SetIndent( 4 );
		//w.SetMaxCharsPerLine( 80 );
		w.SetOutputStyle( Pdf::Stingy );
		//doc.Catalog().SetNumber( "TEST!!!", 1234.56 );
		//doc.Catalog().SetBool( "TEST2", true);
		w.Write( doc, clog );

		/*std::clog << "Dump contents of " << doc.GetFileName() << endl;

		Dump( doc, std::clog );*/

		doc.Close();
	}
	catch( const pdfException& ex )
	{
		clog << "PDF error: " << ex << endl;
	}
	catch( const exception& ex )
	{
		clog << ex.what() << endl;
	}
	catch(...)
	{
		clog << "UNHANDLED ERROR!!" << endl;
		throw;
	}

	std::clog << endl;
/*

typedef std::basic_stringbuf<byte> bytebuff;
typedef std::basic_ostream<byte> obytestream;
typedef std::basic_istream<byte> ibytestream;
typedef std::basic_iostream<byte> iobytestream;

	bytebuff mem;

	obytestream stm( &mem );
	ibytestream istm( &mem );

	stringstream z;

	byte bb=99;
	istm >> bb;
*/

	//zstream z;
	//z << "Hello Hello Hello Hello Hello Hello Hello Hello Hello Hello Hello Hello Hello Hello Hello Hello Hello Hello Hello Hello Hello Hello Hello Hello Hello Hello Hello ";
//	z.compress();

	////stm.clear();
	//stm << "Just testing " << stm.tellp() << std::endl;
	//stm << "Just testing " << stm.tellp() << std::endl;
	//stm << "Just testing " << stm.tellp() << std::endl;
	///*stm.seekp( 5 );
	//stm << "XX" << stm.tellp() << std::endl<< std::ends<< std::endl ;
	//stm.seekp( 0 );*/
	//cout << "Pos: " << stm.tellp() <<  '\n' << mem.str() << std::endl;


	return 0;
}

