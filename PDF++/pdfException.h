#pragma once
#include <string>
#include <exception>
#include "Serializable.h"
#include "ExportDef.h"
#ifdef _DEBUG
#include <iostream>
#endif

namespace Pdf {
namespace Exception {
//
//typedef std::string PDF_API CMyString;
//
//class PDF_API CMyStrClass : public std::string
//{
//public:
//};

///////////////////////////////////////////////////////////////////////
class PDF_API pdfException : public std::exception, public CSerializable
{
public:
	pdfException(void);
#ifdef _MSC_VER
    explicit pdfException(const char * const & psz )  : std::exception(psz) { m_err = -1;} ;
	pdfException(const char * const & psz, int i)  : std::exception(psz, i) {m_err = i;} ;
    pdfException(const pdfException& ex)  : std::exception(ex.what()) {m_err = ex.m_err ;} ;
    pdfException(const std::exception& ex)  : std::exception(ex.what()) {m_err = -1;} ;
#else
    explicit pdfException(const char * const & psz )  { m_err = -1; m_what = psz; } ;
	pdfException(const char * const & psz, int i)  {m_err = i; m_what = psz; } ;
    pdfException(const pdfException& ex)  {m_err = ex.m_err ; m_what = ex.m_what; } ;
    pdfException(const std::exception& ex)  {m_err = -1; m_what = ex.what();} ;
#endif
	virtual ~pdfException(void);

	// CSerializable
	void Serialize( std::ostream& stm );

	inline int err() const throw()
	{
		return m_err;
	};
	inline const char* explanation() const throw()
	{
		return m_explanation.c_str();
	};
#ifdef __GNUC__
    virtual const char* what() const _GLIBCXX_USE_NOEXCEPT
	{
		return m_what.c_str();
	};
#endif

protected:
#ifdef __GNUC__
	std::string m_what;
#endif
	std::string m_explanation;
	int m_err;
};

///////////////////////////////////////////////////////////////////////
class PDF_API pdfNotImplException : public pdfException
{
public:
	pdfNotImplException() : pdfException("Not implemented", -2)
	{
	};
};

///////////////////////////////////////////////////////////////////////
class PDF_API pdfTypeMismatchException : public pdfException
{
public:
	pdfTypeMismatchException() : pdfException("Type mismatch.", -3)
	{
#ifdef _DEBUG
		std::clog << *this << std::endl;
#endif
	};
};

class PDF_API pdfBadIndexException : public pdfException
{
public:
	pdfBadIndexException() : pdfException("Key or Index out of range.", -4)
	{
	};
};
///////////////////////////////////////////////////////////////////////
class PDF_API pdfDocUndefinedException : public pdfException
{
public:
	pdfDocUndefinedException() : pdfException("Atoms require a valid document.", -5)
	{
	};
};
///////////////////////////////////////////////////////////////////////
class PDF_API pdfAlreadyChildException : public pdfException
{
public:
	pdfAlreadyChildException() : pdfException("Atom is already a child of another collection.", -6)
	{
	};
};

///////////////////////////////////////////////////////////////////////
class PDF_API pdfBadRelationshipException : public pdfException
{
public:
	pdfBadRelationshipException() : pdfException("Cannot set an Atom object to refer to itself.", -7)
	{
	};
};
///////////////////////////////////////////////////////////////////////
class PDF_API pdfBlankKeyException : public pdfException
{
public:
	pdfBlankKeyException() : pdfException("Cannot have a blank key for a dictionary member.", -8)
	{
	};
};

///////////////////////////////////////////////////////////////////////
class PDF_API pdfBlankNameException : public pdfException
{
public:
	pdfBlankNameException() : pdfException("A name object cannot be blank.", -9 )
	{
	};
};

///////////////////////////////////////////////////////////////////////
class PDF_API pdfObjectUninitializedEx : public pdfException
{
public:
	pdfObjectUninitializedEx() : pdfException("Object requires a valid document and/or atom.", -10)
	{
	};
};

///////////////////////////////////////////////////////////////////////
class PDF_API pdfObjectInvalidEx : public pdfException
{
public:
	pdfObjectInvalidEx() : pdfException("Object is not of the right type. ", -11)
	{
//		m_explanation = "Either a pdfObject has not been assigned a dictionary or array pdf object OR the pdf object's type attribute doesn't match the functionality of the pdfObject being used on it.  pdfPage, for example, looks for '/Type(Page)' in the dictionary.";
		m_explanation = "A pdfObject has not been assigned a dictionary or array pdf object whose type attribute doesn't match the functionality of the kind pdfObject being used on it.  pdfPage, for example, looks for '/Type /Page' in its dictionary.";
	};
};

///////////////////////////////////////////////////////////////////////
class PDF_API pdfUnhandledSecurityEx : public pdfException
{
public:
	pdfUnhandledSecurityEx(const std::string& szMsg) : pdfException(szMsg.c_str(), -12)
	{
	};
	pdfUnhandledSecurityEx(const char* szMsg) : pdfException(szMsg, -12)
	{
	};
	//pdfUnhandledSecurityEx() : pdfException("Object is not of the right type. ", -12)
	//{
	//};
};

///////////////////////////////////////////////////////////////////////
class PDF_API pdfUnauthorizedEx : public pdfException
{
public:
	pdfUnauthorizedEx(const std::string& szMsg) : pdfException(szMsg.c_str(), -13)
	{
	};
	pdfUnauthorizedEx(const char* szMsg) : pdfException(szMsg, -13)
	{
	};
	pdfUnauthorizedEx() : pdfException("Unauthorized.", -13)
	{
	};
};

///////////////////////////////////////////////////////////////////////
class PDF_API pdfNeedUserPwdEx : public pdfException
{
public:
	pdfNeedUserPwdEx(const std::string& szMsg) : pdfException(szMsg.c_str(), -14)
	{
	};
	pdfNeedUserPwdEx(const char* szMsg) : pdfException(szMsg, -14)
	{
	};
	pdfNeedUserPwdEx() : pdfException("A user password is required to open.", -14)
	{
	};
};

///////////////////////////////////////////////////////////////////////
class PDF_API pdfDocClosedException : public pdfException
{
public:
	pdfDocClosedException() : pdfException("Document has not been opened/initialized.", -15)
	{
	};
};

///////////////////////////////////////////////////////////////////////
class PDF_API pdfUnhandledColorSpaceEx : public pdfException
{
public:
	pdfUnhandledColorSpaceEx() : pdfException("Unhandled ColorSpace.", -16)
	{
	};
};

///////////////////////////////////////////////////////////////////////
class PDF_API pdfUnrecognizedItemTypeEx : public pdfException
{
public:
	pdfUnrecognizedItemTypeEx() : pdfException( "Unrecognized item type.", -17 )
	{
	};
};

};// namespace Exception
};// namespace Pdf


//PDF_API std::ostream& operator<<( std::ostream& stm, const Pdf::Exception::pdfException& atm );
