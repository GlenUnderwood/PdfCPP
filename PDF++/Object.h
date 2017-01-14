#pragma once
#include <memory>
#include "ExportDef.h"
#include "Int.h"
#include "Int64.h"
#include "Real.h"
#include "Bool.h"
#include "String.h"
#include "Name.h"
#include "Blob.h"
#include "Array.h"
//#include "Document.h"

namespace Pdf {

class pdfDocument;

class PDF_API pdfObject
{
protected:
	pdfObject(void);
	pdfObject(const pdfAtom& src);
	//pdfObject(pdfDocument& doc);
public:
	virtual ~pdfObject(void);

protected:
	virtual pdfObject& operator=(const pdfAtom& src);
public:

	//pdfObject& operator=(const pdfDictionary& src);
	//operator pdfDictionary();
	inline pdfObject& operator=(const std::nullptr_t i) { m_atm.SetValue( i ); return *this;}

	pdfAtom operator[]( const size_t  i );
	pdfAtom operator[]( const char* i );
	pdfAtom operator[]( const std::string& i );

	pdfDocument GetDocument() const;

	pdfDictionary	GetDict() const;
	pdfBlob			GetStream() const;
	pdfArray		GetArray() const;
	pdfAtom			GetAtom() const throw();
	
	virtual void Reset();
	/*
	public static implicit operator pdfDictionary(pdfObject o)
		{
			if (o.Atom == null)
				throw new pdfObjectUninitializedEx();
			if (!(o.Atom is pdfDictionary))
				throw new pdfMgrException( o.GetType() + " is not a dictionary." );
			return o.Atom as pdfDictionary;
		}
		public static implicit operator pdfArray( pdfObject o )
		{
			if ( o.Atom == null )
				throw new pdfObjectUninitializedEx();
			if ( !( o.Atom is pdfArray ) )
				throw new pdfMgrException( o.Atom.GetType() + " is not an array." );
			return o.Atom as pdfArray;
		}
		public static implicit operator pdfStream( pdfObject o )
		{
			if ( o.Atom == null )
				throw new pdfObjectUninitializedEx();
			if ( !( o.Atom is pdfStream ) )
				throw new pdfMgrException( o.Atom.GetType() + " is not a stream." );
			return o.Atom as pdfStream;
		}
		public static implicit operator pdfAtom( pdfObject o )
		{
			if ( o.Atom == null )
				throw new pdfObjectUninitializedEx();
			return o.Atom;
		}
		/// <summary>
		/// Gets the associated pdfAtom in the form of a pdfDictionary if it is one.
		/// </summary>
		public pdfDictionary Dict
		{
			get
			{
				if (Atom == null)
					throw new pdfObjectUninitializedEx();
				if (!(Atom is pdfDictionary))
					throw new pdfMgrException(Atom.GetType() + " is not a dictionary.");
				return (Atom as pdfDictionary);
			}
		}
		/// <summary>
		/// Gets the associated pdfAtom in the form of a pdfStream if it is one.
		/// since pdfStream inherits pdfDictionary, you could still use it as one.
		/// </summary>
		public pdfStream Stream
		{
			get
			{
				if (Atom == null)
					throw new pdfObjectUninitializedEx();
				if (!(Atom is pdfStream))
					throw new pdfMgrException( Atom.GetType() + " is not a Stream.");
				return (Atom as pdfStream);
			}
		}
		public pdfArray Array
		{
			get
			{
				if (Atom == null)
					throw new pdfObjectUninitializedEx();
				if (!(Atom is pdfArray))
					throw new pdfMgrException( Atom.GetType() + " is not an array." );
				return (Atom as pdfArray);
			}
		}
		public bool Has(string sKey)
		{
			return Atom != null ? Dict.Has(sKey) : false;
		}
		public pdfAtom this[string sKey]
		{
			get
			{
				return Atom != null ? Dict[sKey] : null;
			}
			set
			{
				if( Atom != null )
					Dict[sKey] = value;
			}
		}
		public pdfAtom this[int iIndex]
		{
			get
			{
				return Array[iIndex];
			}
			set
			{
				Array[iIndex] = value;
			}
		}

		internal pdfAtom Atom { get; set; }

		protected Guid m_InternalID = Guid.NewGuid();
		public Guid InternalID { get { return m_InternalID; } }

		protected pdfRect GetRect( string sName )
		{
			pdfAtom atmCrop = this[ sName ];
			if (atmCrop != null && atmCrop is pdfArray)
			{
				pdfArray arr = atmCrop as pdfArray;
				return GetRectFromArray( arr );
			}
			return new pdfRect( 0, 0, 0, 0 );
		}
		protected pdfRect GetRectFromArray( pdfArray arr )
		{
			if ( arr != null && arr.Count >= 4)
			{
				pdfRect rc = new pdfRect( 0, 0, 0, 0 );
				rc.Left =  arr.m_lst[ 0 ].RealF;
				rc.Top = arr.m_lst[ 3 ].RealF;
				rc.Right = arr.m_lst[ 2 ].RealF;
				rc.Bottom = arr.m_lst[ 1 ].RealF;
//						return new pdfRect( arr.m_lst[ 0 ].RealF, arr.m_lst[ 1 ].RealF,
//												arr.m_lst[ 2 ].RealF - arr.m_lst[ 0 ].RealF, arr.m_lst[ 3 ].RealF - arr.m_lst[ 1 ].RealF );
				return rc;
			}
			return new pdfRect( 0, 0, 0, 0 );
		}
		//public pdfObjType Type
		//{
		//    get;
		//    protected set;
		//}


	//void Clear();
		*/
protected:
	//pdfDocument* m_doc;
	pdfAtom m_atm;

};

};// namespace Exception
