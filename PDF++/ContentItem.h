#pragma once
#include "ExportDef.h"
//#include "Dictionary.h"
//#include "Resources.h"

namespace Pdf {
namespace Content {


class PDF_API pdfContentItem
{
public:
	pdfContentItem(void);
	virtual ~pdfContentItem(void);
};
/*
	public abstract class pdfContentItem
	{
		public pdfContentItem()
		{
			m_GState = new pdfGState();
		}
		public pdfContentItem( pdfGState inGState )
		{
			m_GState = new pdfGState( inGState );
		}
		
		public virtual pdfContentType Type { get { return pdfContentType.Unknown;} }

		internal pdfGState m_GState = null;
		public pdfGState GState
		{
			get
			{
				if ( m_GState == null )
					m_GState = new pdfGState();
				return m_GState;
			}
			set
			{
				//always make a copy
				m_GState = new pdfGState( value );
			}
		}

		//public pdfContents Parent { get; internal set; }
	}

*/
};// namespace Content
};// namespace Pdf
