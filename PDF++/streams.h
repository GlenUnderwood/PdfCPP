#pragma once
//#include <io.h>
#include <iosfwd> 
#include <sstream>
#include "pdfTypes.h"
using namespace std;

#ifdef __GNUC__
	#include <limits>

	#define __CLR_OR_THIS_CALL
	#define _STD ::std::
	//#define _BADOFF std::numeric_limits<std::streamoff>::max()
#else // _MSC_VER
#endif

#ifndef INT_MAX
#define INT_MAX       __INT32_MAX__
#endif

namespace Pdf
{

/* ======================================================================================================================


*/

/* ======================================================================================================================
	basic byte buffer class

*/
template<class _Elem, class _Traits, class _Alloc>
class basic_bytebuf
	: public basic_streambuf<_Elem, _Traits>
{	// stream buffer maintaining an allocated character array
public:
	typedef basic_bytebuf<_Elem, _Traits, _Alloc> _Myt;
	typedef _Alloc allocator_type;
	typedef basic_streambuf<_Elem, _Traits> _Mysb;
	//typedef basic_string<_Elem, _Traits, _Alloc> _Mystr;
#ifdef __GNUC__
	const std::streamoff _BADOFF = std::numeric_limits<std::streamoff>::max() ;
#else // _MSC_VER
#endif

	explicit basic_bytebuf( const _Elem* _pSrcToCopy, size_t nCount, ios_base::openmode _Mode = ios_base::in | ios_base::out)
		{	// construct empty character buffer from mode
		_Init(_pSrcToCopy, nCount, _Getstate(_Mode));
		}

	explicit basic_bytebuf(size_t nCount = 0, ios_base::openmode _Mode = ios_base::in | ios_base::out)
		{	// construct empty character buffer with a starting amount
		_Init(0, nCount, _Getstate(_Mode));
		}

	/*explicit basic_bytebuf(const _Mystr& _Str,
		ios_base::openmode _Mode = ios_base::in | ios_base::out)
		{	// construct character buffer from string, mode
		_Init(_Str.c_str(), _Str.size(), _Getstate(_Mode));
		}*/

	basic_bytebuf(_Myt&& _Right)
		{	// construct by moving _Right
		_Assign_rv(_STD forward<_Myt>(_Right));
		}

	_Myt& operator=(_Myt&& _Right)
		{	// assign from _Right
		_Assign_rv(_STD forward<_Myt>(_Right));
		return (*this);
		}

	void _Assign_rv(_Myt&& _Right)
		{	// assign by moving _Right
		if (this != &_Right)
			{	// different, worth moving
			_Tidy();
			this->swap(_Right);
			}
		}

	void swap(_Myt& _Right)
		{	// swap with _Right
		if (this != &_Right)
			{	// different, worth swapping
			_Mysb::swap(_Right);
			_STD swap(_Seekhigh, _Right._Seekhigh);
			_STD swap(_Mystate, _Right._Mystate);
			_Swap_adl(_Al, _Right._Al);
			}
		}

	void swap(_Myt&& _Right)
		{	// swap with _Right
		_Assign_rv(_STD forward<_Myt>(_Right));
		}

	virtual ~basic_bytebuf()
		{	// destroy the object
		_Tidy();
		}


	enum
		{	// constants for bits in stream state
		_Allocated = 1,	// set if character array storage has been allocated
		_Constant = 2,	// set if character array nonmutable
		_Noread = 4,	// set if character array cannot be read
		_Append = 8,	// set if all writes are appends
		_Atend = 16};	// set if initial writes are appends
	typedef int _Strstate;

	typedef typename _Traits::int_type int_type;
	typedef typename _Traits::pos_type pos_type;
	typedef typename _Traits::off_type off_type;


////////////////////////////////
protected:
	/*virtual*/ _Myt *__CLR_OR_THIS_CALL setbuf(_Elem *, std::streamsize)
		{	// offer buffer to external agent (do nothing)
		return (this);
		}
public:
	size_t GetLength() const
	{
		if (!(_Mystate & _Constant) && _Mysb::pptr() != 0)
		{	// writable, make string from write buffer
			//_Mystr _Str(_Mysb::pbase(), (_Seekhigh < _Mysb::pptr()
			//	? _Mysb::pptr() : _Seekhigh) - _Mysb::pbase());
			//return (_Str);
			return ( (_Seekhigh < _Mysb::pptr() ? _Mysb::pptr() : _Seekhigh) - _Mysb::pbase() );
		}
		else if (!(_Mystate & _Noread) && _Mysb::gptr() != 0)
		{	// readable, make string from read buffer
			//_Mystr _Str(_Mysb::eback(), _Mysb::egptr() - _Mysb::eback());
			//return (_Str);
			return (_Mysb::egptr() - _Mysb::eback());
		}
		//else inaccessible, return empty
		return 0;//_Mysb::egptr() - _Mysb::eback();//_Mysb::
	}
	const _Elem* GetBuffer() const
	{	// return string copy of character array
		if (!(_Mystate & _Constant) && _Mysb::pptr() != 0)
		{	// writable, make string from write buffer
			_Elem* _Str(_Mysb::pbase()/*, (_Seekhigh < _Mysb::pptr()
				? _Mysb::pptr() : _Seekhigh) - _Mysb::pbase()*/);
			return (_Str);
		}
		else if (!(_Mystate & _Noread) && _Mysb::gptr() != 0)
		{	// readable, make string from read buffer
			_Elem* _Str(_Mysb::eback());//, _Mysb::egptr() - _Mysb::eback());
			return (_Str);
		}
		//else
		//{	// inaccessible, return empty string
			_Elem* _Nul = nullptr;
			return (_Nul);
		//}
	}

////////////////////////////////
	/*void str(const _Mystr& _Newstr)
		{	// replace character array from string
		_Tidy();
		_Init(_Newstr.c_str(), _Newstr.size(), _Mystate);
		}*/

protected:
	virtual int_type overflow(int_type _Meta = _Traits::eof())
		{	// put an element to stream
		if ((_Mystate & _Append)
			&& _Mysb::pptr() != 0 && _Mysb::pptr() < _Seekhigh)
		{
#ifdef __GNUC__
			_Elem * pb = _Mysb::pbase();
			_Elem * pc = _Seekhigh;
			_Elem * pe = _Mysb::epptr();

			_Mysb::setp(pb, pe);	// change write position to match
			_Mysb::pbump( pc - pb );// to simulate what's going on in the MSC-originated side.  MSC Has a version of setp that also takes a current pos.
#else // _MSC_VER
			_Mysb::setp(_Mysb::pbase(), _Seekhigh, _Mysb::epptr());
#endif
		}

		if (_Traits::eq_int_type(_Traits::eof(), _Meta))
			return (_Traits::not_eof(_Meta));	// EOF, return success code
		else if (_Mysb::pptr() != 0
			&& _Mysb::pptr() < _Mysb::epptr())
			{	// room in buffer, store it
#ifdef __GNUC__
			*_Mysb::pptr() = _Traits::to_char_type(_Meta);
			_Mysb::pbump(1);
#else // _MSC_VER
			*_Mysb::_Pninc() = _Traits::to_char_type(_Meta);
#endif
			return (_Meta);
			}
		else if (_Mystate & _Constant)
			return (_Traits::eof());	// array nonmutable, fail
		else
			{	// grow buffer and store element
			size_t _Oldsize = _Mysb::pptr() == 0
				? 0 : _Mysb::epptr() - _Mysb::eback();
			size_t _Newsize = _Oldsize;
			size_t _Inc = _Newsize / 2 < _MINSIZE
				? _MINSIZE : _Newsize / 2;	// grow by 50 per cent

			while (0 < _Inc && INT_MAX - _Inc < _Newsize)
				_Inc /= 2;	// increment causes overflow, halve it
			if (_Inc == 0)
				return (_Traits::eof());	// buffer can't grow, fail

			_Newsize += _Inc;

			_Elem *_Newptr = _Al.allocate(_Newsize);
			_Elem *_Oldptr = _Mysb::eback();

			if (0 < _Oldsize)
				_Traits::copy(_Newptr, _Oldptr, _Oldsize);

			if (_Oldsize == 0)
				{	// first growth, set up pointers
				_Seekhigh = _Newptr;
				_Mysb::setp(_Newptr, _Newptr + _Newsize);
				if (_Mystate & _Noread)
					_Mysb::setg(_Newptr, 0, _Newptr);
				else
					_Mysb::setg(_Newptr, _Newptr, _Newptr + 1);
				}
			else
				{	// not first growth, adjust pointers
				_Seekhigh = _Newptr + (_Seekhigh - _Oldptr);
#ifdef __GNUC__
				_Elem * pb = _Newptr + (_Mysb::pbase() - _Oldptr);
				_Elem * pc = _Newptr + (_Mysb::pptr() - _Oldptr);
				_Elem * pe = _Newptr + _Newsize;

				_Mysb::setp(pb, pe);	// change write position to match
				_Mysb::pbump( pc - pb );// to simulate what's going on in the MSC-originated side.  MSC Has a version of setp that also takes a current pos.
#else // _MSC_VER
				_Mysb::setp(_Newptr + (_Mysb::pbase() - _Oldptr),
					_Newptr + (_Mysb::pptr() - _Oldptr),
					_Newptr + _Newsize);
#endif
				if (_Mystate & _Noread)
					_Mysb::setg(_Newptr, 0, _Newptr);
				else
					_Mysb::setg(_Newptr,
						_Newptr + (_Mysb::gptr() - _Oldptr),
						_Mysb::pptr() + 1);
				}

			if (_Mystate & _Allocated)
				_Al.deallocate(_Oldptr, _Oldsize);
			_Mystate |= _Allocated;

#ifdef __GNUC__
			*_Mysb::pptr() = _Traits::to_char_type(_Meta);
			_Mysb::pbump(1);
#else // _MSC_VER
			*_Mysb::_Pninc() = _Traits::to_char_type(_Meta);
#endif
			return (_Meta);
			}
		}

	virtual int_type pbackfail(int_type _Meta = _Traits::eof())
		{	// put an element back to stream
		if (_Mysb::gptr() == 0
			|| _Mysb::gptr() <= _Mysb::eback()
			|| !_Traits::eq_int_type(_Traits::eof(), _Meta)
			&& !_Traits::eq(_Traits::to_char_type(_Meta), _Mysb::gptr()[-1])
			&& (_Mystate & _Constant))
			return (_Traits::eof());	// can't put back, fail
		else
			{	// back up one position and store put-back character
			_Mysb::gbump(-1);
			if (!_Traits::eq_int_type(_Traits::eof(), _Meta))
				*_Mysb::gptr() = _Traits::to_char_type(_Meta);
			return (_Traits::not_eof(_Meta));
			}
		}

	virtual int_type underflow()
		{	// get an element from stream, but don't point past it
		if (_Mysb::gptr() == 0)
			return (_Traits::eof());	// no character buffer, fail
		else if (_Mysb::gptr() < _Mysb::egptr())
			return (_Traits::to_int_type(*_Mysb::gptr()));	// return buffered
		else if (_Mystate & _Noread || _Mysb::pptr() == 0
			|| (_Mysb::pptr() <= _Mysb::gptr() && _Seekhigh <= _Mysb::gptr()))
			return (_Traits::eof());	// can't read, fail
		else
			{	// extend read buffer into written area, then return buffered
			if (_Seekhigh < _Mysb::pptr())
				_Seekhigh = _Mysb::pptr();
			_Mysb::setg(_Mysb::eback(), _Mysb::gptr(), _Seekhigh);
			return (_Traits::to_int_type(*_Mysb::gptr()));
			}
		}

	virtual pos_type seekoff(off_type _Off,
		ios_base::seekdir _Way,
		ios_base::openmode _Which = ios_base::in | ios_base::out)
		{	// change position by _Off, according to _Way, _Mode
		if (_Mysb::pptr() != 0 && _Seekhigh < _Mysb::pptr())
			_Seekhigh = _Mysb::pptr();	// update high-water pointer

		if ((_Which & ios_base::in) && _Mysb::gptr() != 0)
			{	// position within read buffer
			if (_Way == ios_base::end)
				_Off += (off_type)(_Seekhigh - _Mysb::eback());
			else if (_Way == ios_base::cur
				&& (_Which & ios_base::out) == 0)
				_Off += (off_type)(_Mysb::gptr() - _Mysb::eback());
			else if (_Way != ios_base::beg)
				_Off = _BADOFF;

			if (0 <= _Off && _Off <= _Seekhigh - _Mysb::eback())
				{	// change read position
					_Mysb::gbump((int)(_Mysb::eback() - _Mysb::gptr() + _Off));
					if (_Which & ios_base::out && _Mysb::pptr() != 0)
					{
	#ifdef __GNUC__
						_Elem * pb = _Mysb::pbase();
						_Elem * pc = _Mysb::gptr();
						_Elem * pe = _Mysb::epptr();

						_Mysb::setp(pb, pe);	// change write position to match
						_Mysb::pbump( pc - pb );// to simulate what's going on in the MSC-originated side.  MSC Has a version of setp that also takes a current pos.
	#else // _MSC_VER
						_Mysb::setp(_Mysb::pbase(), _Mysb::gptr(),
							_Mysb::epptr());	// change write position to match
	#endif
					}
				}
			else
				_Off = _BADOFF;
			}
		else if (_Which & ios_base::out && _Mysb::pptr() != 0)
			{	// position within write buffer
			if (_Way == ios_base::end)
				_Off += (off_type)(_Seekhigh - _Mysb::eback());
			else if (_Way == ios_base::cur)
				_Off += (off_type)(_Mysb::pptr() - _Mysb::eback());
			else if (_Way != ios_base::beg)
				_Off = _BADOFF;

			if (0 <= _Off && _Off <= _Seekhigh - _Mysb::eback())
				_Mysb::pbump((int)(_Mysb::eback()
					- _Mysb::pptr() + _Off));	// change write position
			else
				_Off = _BADOFF;
			}

 #if _HAS_CPP0X || __cplusplus >= 201103L
		else if (_Off != 0)

 #else /* _HAS_CPP0X */
		else
 #endif /* _HAS_CPP0X */

			_Off = _BADOFF;	// neither read nor write buffer selected, fail
		return (pos_type(_Off));
		}

	virtual pos_type seekpos(pos_type _Ptr,
		ios_base::openmode _Mode = ios_base::in | ios_base::out)
		{	// change position to _Pos, according to _Mode
		streamoff _Off = (streamoff)_Ptr;
		if (_Mysb::pptr() != 0 && _Seekhigh < _Mysb::pptr())
			_Seekhigh = _Mysb::pptr();	// update high-water pointer

		if (_Off == _BADOFF)
			;
		else if ((_Mode & ios_base::in) && _Mysb::gptr() != 0)
			{	// position within read buffer
			if (0 <= _Off && _Off <= _Seekhigh - _Mysb::eback())
				{	// change read position
					_Mysb::gbump((int)(_Mysb::eback() - _Mysb::gptr() + _Off));
					if ((_Mode & ios_base::out) && _Mysb::pptr() != 0)
					{
	#ifdef __GNUC__
						_Elem * pb = _Mysb::pbase();
						_Elem * pc = _Mysb::gptr();
						_Elem * pe = _Mysb::epptr();

						_Mysb::setp(pb, pe);	// change write position to match
						_Mysb::pbump( pc - pb );// to simulate what's going on in the MSC-originated side.  MSC Has a version of setp that also takes a current pos.
	#else // _MSC_VER
						_Mysb::setp(_Mysb::pbase(), _Mysb::gptr(),
							_Mysb::epptr());	// change write position to match
	#endif
					}
				}
			else
				_Off = _BADOFF;
			}
		else if ((_Mode & ios_base::out) && _Mysb::pptr() != 0)
			{	// position within write buffer
			if (0 <= _Off && _Off <= _Seekhigh - _Mysb::eback())
				_Mysb::pbump((int)(_Mysb::eback()
					- _Mysb::pptr() + _Off));	// change write position
			else
				_Off = _BADOFF;
			}
		else
			_Off = _BADOFF;	// neither read nor write buffer selected, fail
		return (streampos(_Off));
		}

	void _Init(const _Elem *_Ptr,
		size_t _Count, _Strstate _State)
		{	// initialize buffer to [_Ptr, _Ptr + _Count), set state
		_Seekhigh = 0;
		_Mystate = _State;

		if (_Count != 0
			&& (_Mystate & (_Noread | _Constant)) != (_Noread | _Constant))
			{	// finite buffer that can be read or written, set it up
			_Elem *_Pnew = _Al.allocate(_Count);

			if (_Ptr != nullptr )
				_Traits::copy(_Pnew, _Ptr, _Count);
			else
				memset(_Pnew, 0, _Count);

			_Seekhigh = _Pnew + _Count;

			if (!(_Mystate & _Noread))
				_Mysb::setg(_Pnew, _Pnew,
					_Pnew + _Count);	// setup read buffer
			if (!(_Mystate & _Constant))
				{	// setup write buffer, and maybe read buffer
#ifdef __GNUC__
					_Elem * pb = _Pnew;
					_Elem * pc = (_Mystate & _Atend) ? _Pnew + _Count : _Pnew;
					_Elem * pe = _Pnew + _Count;

					_Mysb::setp(pb, pe);	// change write position to match
					_Mysb::pbump( pc - pb );// to simulate what's going on in the MSC-originated side.  MSC Has a version of setp that also takes a current pos.
#else // _MSC_VER
					_Mysb::setp(_Pnew,
						(_Mystate & _Atend) ? _Pnew + _Count : _Pnew,
						_Pnew + _Count);
#endif

				if (_Mysb::gptr() == 0)
					_Mysb::setg(_Pnew, 0, _Pnew);
				}
			_Mystate |= _Allocated;
			}
		}

	void _Tidy()
		{	// discard any allocated buffer and clear pointers
		if (_Mystate & _Allocated)
			_Al.deallocate(_Mysb::eback(),
				(_Mysb::pptr() != 0 ? _Mysb::epptr()
					: _Mysb::egptr()) - _Mysb::eback());
		_Mysb::setg(0, 0, 0);
		_Mysb::setp(0, 0);
		_Seekhigh = 0;
		_Mystate &= ~_Allocated;
		}

private:
	enum
		{	// constant for minimum buffer size
		_MINSIZE = 32};

	_Strstate _Getstate(ios_base::openmode _Mode)
	{	// convert open mode to stream state bits
		_Strstate _State = (_Strstate)0;
		if (!(_Mode & ios_base::in))
			_State |= _Noread;
		if (!(_Mode & ios_base::out))
			_State |= _Constant;
		if (_Mode & ios_base::app)
			_State |= _Append;
		if (_Mode & ios_base::ate)
			_State |= _Atend;
		return (_State);
	}

	_Elem *_Seekhigh;	// the high-water pointer in character array
	_Strstate _Mystate;	// the stream state
	allocator_type _Al;	// the allocator object
};

// ================================================
// basic_bytebuf TEMPLATE OPERATORS
template<class _Elem, class _Traits, class _Alloc>
inline void swap(basic_bytebuf<_Elem, _Traits, _Alloc>& _Left,
	basic_bytebuf<_Elem, _Traits, _Alloc>& _Right)
{	// swap _Left and _Right basic_bytebufs
	_Left.swap(_Right);
}

template<class _Elem, class _Traits, class _Alloc>
inline void swap(basic_bytebuf<_Elem, _Traits, _Alloc>& _Left,
	basic_bytebuf<_Elem, _Traits, _Alloc>&& _Right)
{	// swap _Left and _Right basic_bytebufs
	typedef basic_bytebuf<_Elem, _Traits, _Alloc> _Myt;
	_Left.swap(_STD forward<_Myt>(_Right));
}

template<class _Elem, class _Traits, class _Alloc>
inline void swap(basic_bytebuf<_Elem, _Traits, _Alloc>&& _Left,
	basic_bytebuf<_Elem, _Traits, _Alloc>& _Right)
{	// swap _Left and _Right basic_bytebufs
	typedef basic_bytebuf<_Elem, _Traits, _Alloc> _Myt;
	_Right.swap(_STD forward<_Myt>(_Left));
}


/* ======================================================================================================================


*/

/* ======================================================================================================================
	shared byte buffer class.

	Uses a mem block allocated elsewhere.  Does not take ownership

*/
template<class _Elem, class _Traits, class _Alloc>
class shared_bytebuf
	: public basic_streambuf<_Elem, _Traits>
{	// stream buffer maintaining an allocated character array
public:
	typedef shared_bytebuf<_Elem, _Traits, _Alloc> _Myt;
	typedef _Alloc allocator_type;
	typedef basic_streambuf<_Elem, _Traits> _Mysb;
	//typedef basic_string<_Elem, _Traits, _Alloc> _Mystr;
#ifdef __GNUC__
	const std::streamoff _BADOFF = std::numeric_limits<std::streamoff>::max() ;
#else // _MSC_VER
#endif

	explicit shared_bytebuf( const _Elem* _pSrcToCopy, size_t nCount)//, ios_base::openmode _Mode = ios_base::in )
		{	// construct empty character buffer from mode
		_Init(_pSrcToCopy, nCount, _Getstate(ios_base::in));
		}

	shared_bytebuf(_Myt&& _Right)
		{	// construct by moving _Right
		_Assign_rv(_STD forward<_Myt>(_Right));
		}

	_Myt& operator=(_Myt&& _Right)
		{	// assign from _Right
		_Assign_rv(_STD forward<_Myt>(_Right));
		return (*this);
		}

	void _Assign_rv(_Myt&& _Right)
		{	// assign by moving _Right
		if (this != &_Right)
			{	// different, worth moving
			_Tidy();
			this->swap(_Right);
			}
		}

	void swap(_Myt& _Right)
		{	// swap with _Right
		if (this != &_Right)
			{	// different, worth swapping
			_Mysb::swap(_Right);
			_STD swap(_Seekhigh, _Right._Seekhigh);
			_STD swap(_Mystate, _Right._Mystate);
			_Swap_adl(_Al, _Right._Al);
			}
		}

	void swap(_Myt&& _Right)
		{	// swap with _Right
		_Assign_rv(_STD forward<_Myt>(_Right));
		}

	virtual ~shared_bytebuf()
		{	// destroy the object
		_Tidy();
		}


	enum
		{	// constants for bits in stream state
		_Allocated = 1,	// set if character array storage has been allocated
		_Constant = 2,	// set if character array nonmutable
		_Noread = 4,	// set if character array cannot be read
		_Append = 8,	// set if all writes are appends
		_Atend = 16};	// set if initial writes are appends
	typedef int _Strstate;

	typedef typename _Traits::int_type int_type;
	typedef typename _Traits::pos_type pos_type;
	typedef typename _Traits::off_type off_type;


////////////////////////////////
protected:
	/*virtual*/ _Myt *__CLR_OR_THIS_CALL setbuf(_Elem *, streamsize)
		{	// offer buffer to external agent (do nothing)
		return (this);
		}
public:
	size_t GetLength() const
	{
		if (!(_Mystate & _Constant) && _Mysb::pptr() != 0)
		{	// writable, make string from write buffer
			return ( (_Seekhigh < _Mysb::pptr() ? _Mysb::pptr() : _Seekhigh) - _Mysb::pbase() );
		}
		else if (!(_Mystate & _Noread) && _Mysb::gptr() != 0)
		{	// readable, make string from read buffer
			return (_Mysb::egptr() - _Mysb::eback());
		}
		//else inaccessible, return empty
		return 0;//_Mysb::egptr() - _Mysb::eback();//_Mysb::
	}
	const _Elem* GetBuffer() const
	{	// return string copy of character array
		if (!(_Mystate & _Constant) && _Mysb::pptr() != 0)
		{	// writable, make string from write buffer
			_Elem* _Str(_Mysb::pbase()/*, (_Seekhigh < _Mysb::pptr()
				? _Mysb::pptr() : _Seekhigh) - _Mysb::pbase()*/);
			return (_Str);
		}
		else if (!(_Mystate & _Noread) && _Mysb::gptr() != 0)
		{	// readable, make string from read buffer
			_Elem* _Str(_Mysb::eback());//, _Mysb::egptr() - _Mysb::eback());
			return (_Str);
		}
		//else
		//{	// inaccessible, return empty string
			_Elem* _Nul = nullptr;
			return (_Nul);
		//}
	}

////////////////////////////////
	/*void str(const _Mystr& _Newstr)
		{	// replace character array from string
		_Tidy();
		_Init(_Newstr.c_str(), _Newstr.size(), _Mystate);
		}*/

protected:
	virtual int_type overflow(int_type _Meta = _Traits::eof())
		{	// put an element to stream
		if (_Mystate & _Append
			&& _Mysb::pptr() != 0 && _Mysb::pptr() < _Seekhigh)
		{
#ifdef __GNUC__
			_Elem * pb = _Mysb::pbase();
			_Elem * pc = _Seekhigh;
			_Elem * pe = _Mysb::epptr();

			_Mysb::setp(pb, pe);	// change write position to match
			_Mysb::pbump( pc - pb );// to simulate what's going on in the MSC-originated side.  MSC Has a version of setp that also takes a current pos.
#else // _MSC_VER
			_Mysb::setp(_Mysb::pbase(), _Seekhigh, _Mysb::epptr());
#endif
		}
		if (_Traits::eq_int_type(_Traits::eof(), _Meta))
			return (_Traits::not_eof(_Meta));	// EOF, return success code
		else if (_Mysb::pptr() != 0
			&& _Mysb::pptr() < _Mysb::epptr())
			{	// room in buffer, store it
#ifdef __GNUC__
			*_Mysb::pptr() = _Traits::to_char_type(_Meta);
			_Mysb::pbump(1);
#else // _MSC_VER
			*_Mysb::_Pninc() = _Traits::to_char_type(_Meta);
#endif
			return (_Meta);
			}
		else if (_Mystate & _Constant)
			return (_Traits::eof());	// array nonmutable, fail
		else
			{	// grow buffer and store element
			size_t _Oldsize = _Mysb::pptr() == 0
				? 0 : _Mysb::epptr() - _Mysb::eback();
			size_t _Newsize = _Oldsize;
			size_t _Inc = _Newsize / 2 < _MINSIZE
				? _MINSIZE : _Newsize / 2;	// grow by 50 per cent

			while (0 < _Inc && INT_MAX - _Inc < _Newsize)
				_Inc /= 2;	// increment causes overflow, halve it
			if (_Inc == 0)
				return (_Traits::eof());	// buffer can't grow, fail

			_Newsize += _Inc;

			_Elem *_Newptr = _Al.allocate(_Newsize);
			_Elem *_Oldptr = _Mysb::eback();

			if (0 < _Oldsize)
				_Traits::copy(_Newptr, _Oldptr, _Oldsize);

			if (_Oldsize == 0)
				{	// first growth, set up pointers
				_Seekhigh = _Newptr;
				_Mysb::setp(_Newptr, _Newptr + _Newsize);
				if (_Mystate & _Noread)
					_Mysb::setg(_Newptr, 0, _Newptr);
				else
					_Mysb::setg(_Newptr, _Newptr, _Newptr + 1);
				}
			else
				{	// not first growth, adjust pointers
				_Seekhigh = _Newptr + (_Seekhigh - _Oldptr);
#ifdef __GNUC__
				_Elem * pb = _Newptr + (_Mysb::pbase() - _Oldptr);
				_Elem * pc = _Newptr + (_Mysb::pptr() - _Oldptr);
				_Elem * pe = _Newptr + _Newsize;

				_Mysb::setp(pb, pe);	// change write position to match
				_Mysb::pbump( pc - pb );// to simulate what's going on in the MSC-originated side.  MSC Has a version of setp that also takes a current pos.
#else // _MSC_VER
				_Mysb::setp(_Newptr + (_Mysb::pbase() - _Oldptr),
					_Newptr + (_Mysb::pptr() - _Oldptr),
					_Newptr + _Newsize);
#endif
				if (_Mystate & _Noread)
					_Mysb::setg(_Newptr, 0, _Newptr);
				else
					_Mysb::setg(_Newptr,
						_Newptr + (_Mysb::gptr() - _Oldptr),
						_Mysb::pptr() + 1);
				}

			if (_Mystate & _Allocated)
				_Al.deallocate(_Oldptr, _Oldsize);
			_Mystate |= _Allocated;

#ifdef __GNUC__
			*_Mysb::pptr() = _Traits::to_char_type(_Meta);
			_Mysb::pbump(1);
#else // _MSC_VER
			*_Mysb::_Pninc() = _Traits::to_char_type(_Meta);
#endif
			return (_Meta);
			}
		}

	virtual int_type pbackfail(int_type _Meta = _Traits::eof())
		{	// put an element back to stream
		if (_Mysb::gptr() == 0
			|| _Mysb::gptr() <= _Mysb::eback()
			|| !_Traits::eq_int_type(_Traits::eof(), _Meta)
			&& !_Traits::eq(_Traits::to_char_type(_Meta), _Mysb::gptr()[-1])
			&& _Mystate & _Constant)
			return (_Traits::eof());	// can't put back, fail
		else
			{	// back up one position and store put-back character
			_Mysb::gbump(-1);
			if (!_Traits::eq_int_type(_Traits::eof(), _Meta))
				*_Mysb::gptr() = _Traits::to_char_type(_Meta);
			return (_Traits::not_eof(_Meta));
			}
		}

	virtual int_type underflow()
		{	// get an element from stream, but don't point past it
		if (_Mysb::gptr() == 0)
			return (_Traits::eof());	// no character buffer, fail
		else if (_Mysb::gptr() < _Mysb::egptr())
			return (_Traits::to_int_type(*_Mysb::gptr()));	// return buffered
		else if (_Mystate & _Noread || _Mysb::pptr() == 0
			|| (_Mysb::pptr() <= _Mysb::gptr() && _Seekhigh <= _Mysb::gptr()))
			return (_Traits::eof());	// can't read, fail
		else
			{	// extend read buffer into written area, then return buffered
			if (_Seekhigh < _Mysb::pptr())
				_Seekhigh = _Mysb::pptr();
			_Mysb::setg(_Mysb::eback(), _Mysb::gptr(), _Seekhigh);
			return (_Traits::to_int_type(*_Mysb::gptr()));
			}
		}

	virtual pos_type seekoff(off_type _Off,
		ios_base::seekdir _Way,
		ios_base::openmode _Which = ios_base::in | ios_base::out)
		{	// change position by _Off, according to _Way, _Mode
		if (_Mysb::pptr() != 0 && _Seekhigh < _Mysb::pptr())
			_Seekhigh = _Mysb::pptr();	// update high-water pointer

		if ((_Which & ios_base::in) && _Mysb::gptr() != 0)
			{	// position within read buffer
			if (_Way == ios_base::end)
				_Off += (off_type)(_Seekhigh - _Mysb::eback());
			else if (_Way == ios_base::cur
				&& (_Which & ios_base::out) == 0)
				_Off += (off_type)(_Mysb::gptr() - _Mysb::eback());
			else if (_Way != ios_base::beg)
				_Off = _BADOFF;

			if (0 <= _Off && _Off <= _Seekhigh - _Mysb::eback())
				{	// change read position
					_Mysb::gbump((int)(_Mysb::eback() - _Mysb::gptr() + _Off));
					if ((_Which & ios_base::out) && _Mysb::pptr() != 0)
					{
#ifdef __GNUC__
						_Elem * pb = _Mysb::pbase();
						_Elem * pc = _Seekhigh;
						_Elem * pe = _Mysb::epptr();

						_Mysb::setp(pb, pe);	// change write position to match
						_Mysb::pbump( pc - pb );// to simulate what's going on in the MSC-originated side.  MSC Has a version of setp that also takes a current pos.
#else // _MSC_VER
						_Mysb::setp(_Mysb::pbase(), _Mysb::gptr(),
							_Mysb::epptr());	// change write position to match
#endif
					}
				}
			else
				_Off = _BADOFF;
			}
		else if (_Which & ios_base::out && _Mysb::pptr() != 0)
			{	// position within write buffer
			if (_Way == ios_base::end)
				_Off += (off_type)(_Seekhigh - _Mysb::eback());
			else if (_Way == ios_base::cur)
				_Off += (off_type)(_Mysb::pptr() - _Mysb::eback());
			else if (_Way != ios_base::beg)
				_Off = _BADOFF;

			if (0 <= _Off && _Off <= _Seekhigh - _Mysb::eback())
				_Mysb::pbump((int)(_Mysb::eback()
					- _Mysb::pptr() + _Off));	// change write position
			else
				_Off = _BADOFF;
			}

 #if _HAS_CPP0X || __cplusplus >= 201103L
		else if (_Off != 0)

 #else /* _HAS_CPP0X */
		else
 #endif /* _HAS_CPP0X */

			_Off = _BADOFF;	// neither read nor write buffer selected, fail
		return (pos_type(_Off));
		}

	virtual pos_type seekpos(pos_type _Ptr,
		ios_base::openmode _Mode = ios_base::in | ios_base::out)
		{	// change position to _Pos, according to _Mode
		streamoff _Off = (streamoff)_Ptr;
		if (_Mysb::pptr() != 0 && _Seekhigh < _Mysb::pptr())
			_Seekhigh = _Mysb::pptr();	// update high-water pointer

		if (_Off == _BADOFF)
			;
		else if ((_Mode & ios_base::in) && _Mysb::gptr() != 0)
			{	// position within read buffer
			if (0 <= _Off && _Off <= _Seekhigh - _Mysb::eback())
				{	// change read position
					_Mysb::gbump((int)(_Mysb::eback() - _Mysb::gptr() + _Off));
					if ((_Mode & ios_base::out) && _Mysb::pptr() != 0)
					{
#ifdef __GNUC__
						_Elem * pb = _Mysb::pbase();
						_Elem * pc = _Mysb::gptr();
						_Elem * pe = _Mysb::epptr();

						_Mysb::setp(pb, pe);	// change write position to match
						_Mysb::pbump( pc - pb );// to simulate what's going on in the MSC-originated side.  MSC Has a version of setp that also takes a current pos.
#else // _MSC_VER
						_Mysb::setp(_Mysb::pbase(), _Mysb::gptr(),
							_Mysb::epptr());	// change write position to match
#endif
					}
				}
			else
				_Off = _BADOFF;
			}
		else if ((_Mode & ios_base::out) && _Mysb::pptr() != 0)
			{	// position within write buffer
			if (0 <= _Off && _Off <= _Seekhigh - _Mysb::eback())
				_Mysb::pbump((int)(_Mysb::eback()
					- _Mysb::pptr() + _Off));	// change write position
			else
				_Off = _BADOFF;
			}
		else
			_Off = _BADOFF;	// neither read nor write buffer selected, fail
		return (streampos(_Off));
		}

	void _Init(const _Elem *_Ptr,
		size_t _Count, _Strstate _State)
		{	// initialize buffer to [_Ptr, _Ptr + _Count), set state
		_Seekhigh = 0;
		_Mystate = _State;

		if (_Count != 0
			&& (_Mystate & (_Noread | _Constant)) != (_Noread | _Constant))
			{	// finite buffer that can be read or written, set it up
			_Elem *_Pnew = const_cast<_Elem*>(_Ptr);

			/*if (_Ptr != nullptr )
				_Traits::copy(_Pnew, _Ptr, _Count);
			else
				memset(_Pnew, 0, _Count);*/

			_Seekhigh = _Pnew + _Count;

			if (!(_Mystate & _Noread))
				_Mysb::setg(_Pnew, _Pnew,
					_Pnew + _Count);	// setup read buffer
			if (!(_Mystate & _Constant))
				{	// setup write buffer, and maybe read buffer
#ifdef __GNUC__
				_Elem * pb = _Pnew;
				_Elem * pc = (_Mystate & _Atend) ? _Pnew + _Count : _Pnew;
				_Elem * pe = _Pnew + _Count;

				_Mysb::setp(pb, pe);	// change write position to match
				_Mysb::pbump( pc - pb );// to simulate what's going on in the MSC-originated side.  MSC Has a version of setp that also takes a current pos.
#else // _MSC_VER
				_Mysb::setp(_Pnew,
					(_Mystate & _Atend) ? _Pnew + _Count : _Pnew,
					_Pnew + _Count);
#endif
				if (_Mysb::gptr() == 0)
					_Mysb::setg(_Pnew, 0, _Pnew);
				}
			_Mystate |= _Allocated;
			}
		}

	void _Tidy()
		{	// discard any allocated buffer and clear pointers
		/*if (_Mystate & _Allocated)
			_Al.deallocate(_Mysb::eback(),
				(_Mysb::pptr() != 0 ? _Mysb::epptr()
					: _Mysb::egptr()) - _Mysb::eback());*/
		_Mysb::setg(0, 0, 0);
		_Mysb::setp(0, 0);
		_Seekhigh = 0;
		_Mystate &= ~_Allocated;
		}

private:
	enum
		{	// constant for minimum buffer size
		_MINSIZE = 32};

	_Strstate _Getstate(ios_base::openmode _Mode)
	{	// convert open mode to stream state bits
		_Strstate _State = (_Strstate)0;
		if (!(_Mode & ios_base::in))
			_State |= _Noread;
		if (!(_Mode & ios_base::out))
			_State |= _Constant;
		if (_Mode & ios_base::app)
			_State |= _Append;
		if (_Mode & ios_base::ate)
			_State |= _Atend;
		return (_State);
	}

	_Elem *_Seekhigh;	// the high-water pointer in character array
	_Strstate _Mystate;	// the stream state
	allocator_type _Al;	// the allocator object
};

// ================================================
// shared_bytebuf TEMPLATE OPERATORS
template<class _Elem, class _Traits, class _Alloc>
inline void swap(shared_bytebuf<_Elem, _Traits, _Alloc>& _Left,
	shared_bytebuf<_Elem, _Traits, _Alloc>& _Right)
{	// swap _Left and _Right shared_bytebufs
	_Left.swap(_Right);
}

template<class _Elem, class _Traits, class _Alloc>
inline void swap(shared_bytebuf<_Elem, _Traits, _Alloc>& _Left,
	shared_bytebuf<_Elem, _Traits, _Alloc>&& _Right)
{	// swap _Left and _Right shared_bytebufs
	typedef shared_bytebuf<_Elem, _Traits, _Alloc> _Myt;
	_Left.swap(_STD forward<_Myt>(_Right));
}

template<class _Elem, class _Traits, class _Alloc>
inline void swap(shared_bytebuf<_Elem, _Traits, _Alloc>&& _Left,
	shared_bytebuf<_Elem, _Traits, _Alloc>& _Right)
{	// swap _Left and _Right shared_bytebufs
	typedef shared_bytebuf<_Elem, _Traits, _Alloc> _Myt;
	_Right.swap(_STD forward<_Myt>(_Left));
}



/* ======================================================================================================================
	basic input byte stream 

*/
template<class _Elem,
	class _Traits,
	class _Alloc>
	class basic_ibytestream
		: public basic_istream<_Elem, _Traits>
	{	// input stream associated with a character array
public:
	typedef basic_ibytestream<_Elem, _Traits, _Alloc> _Myt;
	typedef basic_istream<_Elem, _Traits> _Mybase;
	typedef _Alloc allocator_type;
	typedef basic_bytebuf<_Elem, _Traits, _Alloc> _Mysb;
	//typedef basic_string<_Elem, _Traits, _Alloc> _Mystr;

	//explicit basic_ibytestream(ios_base::openmode _Mode = ios_base::in)
	//	: _Mybase(&_ByteBuffer),
	//		_ByteBuffer(_Mode | ios_base::in)
	//	{	// construct empty readable character buffer
	//	}

	explicit basic_ibytestream(const _Elem* _pSrcToCopy, size_t nCount, ios_base::openmode _Mode = ios_base::in)
		: _Mybase(&_ByteBuffer),
			_ByteBuffer(_pSrcToCopy, nCount, _Mode)
		{	// construct empty character buffer
		}

	explicit basic_ibytestream(size_t nCount=0, ios_base::openmode _Mode = ios_base::in)
		: _Mybase(&_ByteBuffer),
			_ByteBuffer(nCount,_Mode)
		{	// construct empty character buffer
		}
/*	explicit basic_ibytestream(const _Mystr& _Str,
		ios_base::openmode _Mode = ios_base::in)
		: _Mybase(&_ByteBuffer),
			_ByteBuffer(_Str, _Mode | ios_base::in)
		{	// construct readable character buffer from NTCS
		}*/

	basic_ibytestream(_Myt&& _Right)
		: _Mybase(&_ByteBuffer)
		{	// construct by moving _Right
		_Assign_rv(_STD forward<_Myt>(_Right));
		}

	_Myt& operator=(_Myt&& _Right)
		{	// move from _Right
		_Assign_rv(_STD forward<_Myt>(_Right));
		return (*this);
		}

	void _Assign_rv(_Myt&& _Right)
		{	// assign by moving _Right
		if (this != &_Right)
			{	// different, worth moving
			_ByteBuffer.str(_Mysb());
			this->swap(_Right);
			}
		}

	void swap(_Myt& _Right)
		{	// swap with _Right
		if (this != &_Right)
			{	// different, swap base and buffer
			_Mybase::swap(_Right);
			_ByteBuffer.swap(_Right._ByteBuffer);
			}
		}

	void swap(_Myt&& _Right)
		{	// swap with _Right
		_Assign_rv(_STD forward<_Myt>(_Right));
		}

	virtual ~basic_ibytestream()
		{	// destroy the object
		}

	/*_Mysb *rdbuf() const
		{	// return pointer to file buffer
		return ((_Mysb *)&_ByteBuffer);
		}*/

	//////////////////////////
	size_t GetLength() const
	{	// return string copy of character array
		return (_ByteBuffer.GetLength());
	}
	const _Elem* GetBuffer() const
	{	// return string copy of character array
		return (_ByteBuffer.GetBuffer());
	}

private:
	_Mysb _ByteBuffer;	// the string buffer
	};

	// basic_ibytestream TEMPLATE OPERATORS
template<class _Elem,
	class _Traits,
	class _Alloc> inline
	void swap(basic_ibytestream<_Elem, _Traits, _Alloc>& _Left,
		basic_ibytestream<_Elem, _Traits, _Alloc>& _Right)
	{	// swap _Left and _Right basic_ibytestreams
	_Left.swap(_Right);
	}

template<class _Elem,
	class _Traits,
	class _Alloc> inline
	void swap(basic_ibytestream<_Elem, _Traits, _Alloc>& _Left,
		basic_ibytestream<_Elem, _Traits, _Alloc>&& _Right)
	{	// swap _Left and _Right basic_ibytestreams
	typedef basic_ibytestream<_Elem, _Traits, _Alloc> _Myt;
	_Left.swap(_STD forward<_Myt>(_Right));
	}

template<class _Elem,
	class _Traits,
	class _Alloc> inline
	void swap(basic_ibytestream<_Elem, _Traits, _Alloc>&& _Left,
		basic_ibytestream<_Elem, _Traits, _Alloc>& _Right)
	{	// swap _Left and _Right basic_ibytestreams
	typedef basic_ibytestream<_Elem, _Traits, _Alloc> _Myt;
	_Right.swap(_STD forward<_Myt>(_Left));
	}


/* ======================================================================================================================
	basic output byte stream 

*/
template<class _Elem, class _Traits, class _Alloc>
class basic_obytestream : public basic_ostream<_Elem, _Traits>
{	// output stream associated with a character array
public:
	typedef basic_obytestream<_Elem, _Traits, _Alloc> _Myt;
	typedef basic_ostream<_Elem, _Traits> _Mybase;
	typedef _Alloc allocator_type;
	typedef basic_bytebuf<_Elem, _Traits, _Alloc> _Mysb;
	//typedef basic_string<_Elem, _Traits, _Alloc> _Mystr;

	explicit basic_obytestream(const _Elem* _pSrcToCopy, size_t nCount = 0, ios_base::openmode _Mode = ios_base::out)
		: _Mybase(&_ByteBuffer),
			_ByteBuffer(_pSrcToCopy, nCount, _Mode)
		{	// construct empty character buffer
		}

	explicit basic_obytestream(size_t nCount = 0, ios_base::openmode _Mode = ios_base::out)
		: _Mybase(&_ByteBuffer),
			_ByteBuffer(nCount,_Mode)
		{	// construct empty character buffer
		}

	/*explicit basic_obytestream(const _Mystr& _Str,
		ios_base::openmode _Mode = ios_base::out)
		: _Mybase(&_ByteBuffer),
			_ByteBuffer(_Str, _Mode | ios_base::out)
		{	// construct writable character buffer from NTCS
		}*/

	basic_obytestream(_Myt&& _Right)
		: _Mybase(&_ByteBuffer)
		{	// construct by moving _Right
		_Assign_rv(_STD forward<_Myt>(_Right));
		}

	_Myt& operator=(_Myt&& _Right)
		{	// move from _Right
		_Assign_rv(_STD forward<_Myt>(_Right));
		return (*this);
		}

	void _Assign_rv(_Myt&& _Right)
		{	// assign by moving _Right
		if (this != &_Right)
			{	// different, worth moving
			_ByteBuffer.str(_Mysb());
			this->swap(_Right);
			}
		}

	void swap(_Myt& _Right)
		{	// swap with _Right
		if (this != &_Right)
			{	// different, swap base and buffer
			_Mybase::swap(_Right);
			_ByteBuffer.swap(_Right._ByteBuffer);
			}
		}

	void swap(_Myt&& _Right)
		{	// swap with _Right
		_Assign_rv(_STD forward<_Myt>(_Right));
		}

	virtual ~basic_obytestream()
		{	// destroy the object
		}

	_Mysb *rdbuf() const
		{	// return pointer to buffer
		return ((_Mysb *)&_ByteBuffer);
		}

	//////////////////////////
	size_t GetLength() const
	{	// return string copy of character array
		return (_ByteBuffer.GetLength());
	}
	const _Elem* GetBuffer() const
	{	// return string copy of character array
		return (_ByteBuffer.GetBuffer());
	}

private:
	_Mysb _ByteBuffer;	// the string buffer
	};

	// basic_obytestream TEMPLATE OPERATORS
template<class _Elem,
	class _Traits,
	class _Alloc> inline
	void swap(basic_obytestream<_Elem, _Traits, _Alloc>& _Left,
		basic_obytestream<_Elem, _Traits, _Alloc>& _Right)
	{	// swap _Left and _Right basic_obytestreams
	_Left.swap(_Right);
	}

template<class _Elem,
	class _Traits,
	class _Alloc> inline
	void swap(basic_obytestream<_Elem, _Traits, _Alloc>& _Left,
		basic_obytestream<_Elem, _Traits, _Alloc>&& _Right)
	{	// swap _Left and _Right basic_obytestreams
	typedef basic_obytestream<_Elem, _Traits, _Alloc> _Myt;
	_Left.swap(_STD forward<_Myt>(_Right));
	}

template<class _Elem,
	class _Traits,
	class _Alloc> inline
	void swap(basic_obytestream<_Elem, _Traits, _Alloc>&& _Left,
		basic_obytestream<_Elem, _Traits, _Alloc>& _Right)
	{	// swap _Left and _Right basic_obytestreams
	typedef basic_obytestream<_Elem, _Traits, _Alloc> _Myt;
	_Right.swap(_STD forward<_Myt>(_Left));
	}

/* ======================================================================================================================
	basic input/output byte stream 

*/
template<class _Elem, class _Traits, class _Alloc>
class basic_bytestream : public basic_iostream<_Elem, _Traits>
{	// input/output stream associated with a character array
public:
	typedef basic_bytestream<_Elem, _Traits, _Alloc> _Myt;
	typedef basic_iostream<_Elem, _Traits> _Mybase;
	typedef _Elem char_type;
	typedef _Traits traits_type;
	typedef _Alloc allocator_type;
	typedef typename _Traits::int_type int_type;
	typedef typename _Traits::pos_type pos_type;
	typedef typename _Traits::off_type off_type;
	typedef basic_bytebuf<_Elem, _Traits, _Alloc> _Mysb;
	//typedef basic_string<_Elem, _Traits, _Alloc> _Mystr;

	explicit basic_bytestream(const _Elem* _pSrcToCopy, size_t nCount = 0, ios_base::openmode _Mode =
		ios_base::in | ios_base::out)
		: _Mybase(&_ByteBuffer),
			_ByteBuffer(_pSrcToCopy,nCount,_Mode)
		{	// construct empty character buffer
		}

	explicit basic_bytestream(size_t nCount = 0, ios_base::openmode _Mode =
		ios_base::in | ios_base::out)
		: _Mybase(&_ByteBuffer),
			_ByteBuffer(nCount,_Mode)
		{	// construct empty character buffer
		}

	/*explicit basic_bytestream(const _Mystr& _Str,
		ios_base::openmode _Mode = ios_base::in | ios_base::out)
		: _Mybase(&_ByteBuffer),
			_ByteBuffer(_Str, _Mode)
		{	// construct character buffer from NTCS
		}*/

	basic_bytestream(_Myt&& _Right)
		: _Mybase(&_ByteBuffer)
		{	// construct by moving _Right
		_Assign_rv(_STD forward<_Myt>(_Right));
		}

	_Myt& operator=(_Myt&& _Right)
		{	// move from _Right
		_Assign_rv(_STD forward<_Myt>(_Right));
		return (*this);
		}

	void _Assign_rv(_Myt&& _Right)
		{	// assign by moving _Right
		if (this != &_Right)
			{	// different, worth moving
			//_ByteBuffer.str(_Mystr());
			this->swap(_Right);
			}
		}

	void swap(_Myt& _Right)
		{	// swap with _Right
		if (this != &_Right)
			{	// different, swap base and buffer
			_Mybase::swap(_Right);
			_ByteBuffer.swap(_Right._ByteBuffer);
			}
		}

	void swap(_Myt&& _Right)
		{	// swap with _Right
		_Assign_rv(_STD forward<_Myt>(_Right));
		}

	virtual ~basic_bytestream()
		{	// destroy the object
		}

	_Mysb *rdbuf() const
		{	// return pointer to buffer
		return ((_Mysb *)&_ByteBuffer);
		}

	//////////////////////////
	size_t GetLength() const
	{	// return string copy of character array
		return (_ByteBuffer.GetLength());
	}
	const _Elem* GetBuffer() const
	{	// return string copy of character array
		return (_ByteBuffer.GetBuffer());
	}
	/*_Mystr str() const
		{	// return string copy of character array
		return (_ByteBuffer.str());
		}

	void str(const _Mystr& _Newstr)
		{	// replace character array from string
		_ByteBuffer.str(_Newstr);
		}*/
	//////////////////////////

private:
	_Mysb _ByteBuffer;	// the string buffer
};

//////////////////////////////////////////////////////////////
///
/// the objects we'd actually instantiate and use
///
///
typedef basic_bytebuf<byte, char_traits<byte>, allocator<byte> > PDF_API bytebuf; // buffer - allocates its own
typedef shared_bytebuf<byte, char_traits<byte>, allocator<byte> > PDF_API sharedbytebuf; // buffer - uses a block from elsewhere

typedef basic_ibytestream<byte, char_traits<byte>, allocator<byte> > PDF_API ibytestream; // input
typedef basic_obytestream<byte, char_traits<byte>, allocator<byte> > PDF_API obytestream; // output
typedef basic_bytestream<byte, char_traits<byte>, allocator<byte> >  PDF_API bytestream;  // input/output

}; // namespace Pdf
