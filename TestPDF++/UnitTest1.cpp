#include "stdafx.h"
#include <sstream> 
#include "..\pdf++.h"
#include "..\FlateFilter.h"
#include "..\AtomData.h"

using namespace System;
using namespace System::Text;
using namespace System::Collections::Generic;
using namespace Microsoft::VisualStudio::TestTools::UnitTesting;

namespace TestPDF
{
	[TestClass]
	public ref class UnitTest1
	{
	private:
		TestContext^ testContextInstance;

	public: 
		/// <summary>
		///Gets or sets the test context which provides
		///information about and functionality for the current test run.
		///</summary>
		property Microsoft::VisualStudio::TestTools::UnitTesting::TestContext^ TestContext
		{
			Microsoft::VisualStudio::TestTools::UnitTesting::TestContext^ get()
			{
				return testContextInstance;
			}
			System::Void set(Microsoft::VisualStudio::TestTools::UnitTesting::TestContext^ value)
			{
				testContextInstance = value;
			}
		};

		#pragma region Additional test attributes
		//
		//You can use the following additional attributes as you write your tests:
		//
		//Use ClassInitialize to run code before running the first test in the class
		//[ClassInitialize()]
		//static void MyClassInitialize(TestContext^ testContext) {};
		//
		//Use ClassCleanup to run code after all tests in a class have run
		//[ClassCleanup()]
		//static void MyClassCleanup() {};
		//
		//Use TestInitialize to run code before running each test
		//[TestInitialize()]
		//void MyTestInitialize() {};
		//
		//Use TestCleanup to run code after each test has run
		//[TestCleanup()]
		//void MyTestCleanup() {};
		//
		#pragma endregion 

		[TestMethod]
		void TestMethod1()
		{
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

				stringstream sOut ;
				filter.Transform( buff, (std::ostream&)sOut , true );
				if( sOut.tellp() > 0 )
				{
					CSharedBuff buff2;
					int x = sOut.tellp() ;
					buff2.Resize(x );
					sOut.seekg(0, std::ios::beg);
					sOut.read( (char*)buff2.get(), buff2.GetLength() );
					System::Diagnostics::Debug::Assert("");
				}
			}
		};
	};
}
