#pragma once
#include "ExportDef.h"

namespace Pdf {

    typedef enum _AtomType
    {
        Null = 0,
        Integer,
        Integer64,
        Real,
		Boolean,
        String,
        Name,
        Dictionary,
        Stream,
		Array,

		// only used internally.
		Keyword,
        Reference
	} PDF_API AtomType;

	typedef enum _PdfVersion
	{
		pdf10 = 0,
		pdf11,
		pdf12,
		pdf13,
		pdf14,
		pdf15,
		pdf16,
		pdf17
	} PDF_API PdfVersion;

	typedef enum _StringEncoding
	{
		ASCII = 0,
		WinAnsiEncoding,
		PDFDocEncoding,
		MacRomanEncoding,
		StdEncoding,
		UTF16BE,
		UTF16LE,
		Byte,
		ByteAsChars
	} PDF_API StringEncoding;

	typedef enum _PasswordType
	{
		Invalid = 0,
		User,
		Owner,
		None
	} PDF_API PasswordType;

	typedef enum _Permissions : unsigned int
	{
		All = 0xFFFFFFFC,

		Rev2Only = 0x000000FC,

		/// <summary>
		/// Bits 1–2 Reserved; must be 0.
		/// </summary>
		Reserved1_2 = 0x00000003, // 0x00000001 | 0x00000002,

		/// <summary>
		/// Bit 3 (Revision 2) Print the document.
		/// (Revision 3 or greater) Print the document (possibly not at the high-
		/// est quality level, depending on whether bit 12 is also set
		/// </summary>
		Print = 0x00000004,

		/// <summary>
		/// Bit 4 Modify the contents of the document by operations other than
		/// those controlled by bits 6, 9, and 11.
		/// </summary>
		EditDoc = 0x00000008,

		/// <summary>
		/// Bit 5 (Revision 2) Copy or otherwise extract text and graphics from the
		/// document, including extracting text and graphics (in support of ac-
		/// cessibility to users with disabilities or for other purposes).
		/// (Revision 3 or greater) Copy or otherwise extract text and graphics
		/// from the document by operations other than that controlled by bit 10.
		/// </summary>
		CopyContent = 0x00000010,

		/// <summary>
		/// Bit 6 Add or modify text annotations, fill in interactive form fields, and,
		/// if bit 4 is also set, create or modify interactive form fields (including
		/// signature fields).
		/// </summary>
		TextAnnotations = 0x00000020,

		/// <summary>
		/// Bits 7–8 Reserved; must be 1.
		/// </summary>
		Reserved7_8 = 0x000000C0, //0x00000040 | 0x00000080,

		/// <summary>
		/// Bit 9 (Revision 3 or greater) Fill in existing interactive form fields (includ-
		/// ing signature fields), even if bit 6 is clear.
		/// </summary>
		FillForms = 0x00000100,

		/// <summary>
		/// Bit 10 (Revision 3 or greater) Extract text and graphics (in support of ac-
		/// cessibility to users with disabilities or for other purposes).
		/// </summary>
		AccessibilityCopyContent = 0x00000200,

		/// <summary>
		/// Bit 11 (Revision 3 or greater) Assemble the document (insert, rotate, or de-
		/// lete pages and create bookmarks or thumbnail images), even if bit 4
		/// is clear.
		/// </summary>
		DocAssembly = 0x00000400,

		/// <summary>
		/// Bit 12 (Revision 3 or greater) Print the document to a representation from
		/// which a faithful digital copy of the PDF content could be generated.
		/// When this bit is clear (and bit 3 is set), printing is limited to a low-
		/// level representation of the appearance, possibly of degraded quality.
		/// (See implementation note 25 in Appendix H.)
		/// </summary>
		PrintHighQuality = 0x00000800,

		/// <summary>
		/// Bit 13–32 (Revision 3 or greater) Reserved; must be 1.
		/// </summary>
		Reserved13_32 =	0xFFFFF000,/*  0x00001000 | 0x00002000 | 0x00004000 | 0x00008000 | 0x00010000 | 0x00020000 | 0x00040000 | 0x00080000
						| 0x00100000 | 0x00200000 | 0x00400000 | 0x00800000 | 0x01000000 | 0x02000000 | 0x04000000 | 0x08000000
						| 0x10000000 | 0x20000000 | 0x40000000 | 0x80000000,*/
	
		TheRest = Reserved7_8 | Reserved13_32,
	} PDF_API Permissions;
	
	typedef enum _FilterType 
	{
		NoFilter = 0,
		FlateDecode,
		CCITTFaxDecode,
		DCTDecode,
		LZWDecode,
		ASCII85Decode,
		ASCIIHexDecode
	} PDF_API FilterType;

	typedef enum _OutputStyle
	{
		NicelyFormatted = 0,
		Neat,
		Conservative,
		Stingy,
#ifdef _DEBUG
		Default = NicelyFormatted
#else
		Default = Conservative
#endif
	} PDF_API OutputStyle;

	typedef enum _ContentType
	{
		Unknown = 0,
		Image,
		Text,
		FormXObject,
		Path,
		XObject,
		ExtGState,
		InlineImage,
		Clipping,
		Container
	} PDF_API ContentType;

	typedef enum _GStateFlags
	{
		NoGState	= 0,
		StrokeColor	= 0x000001,
		FillColor	= 0x00000002,
		ColorSpace	= 0x00000004,
		LineWidth	= 0x00000008,
		MiterLimit	= 0x00000010,
		Mat			= 0x00000020,
		Dash		= 0x00000040,
		Flatness	= 0x00000080,
		LineCap		= 0x00000100,
		LineJoin	= 0x00000200,
		RI			= 0x00000400,
		ExtGStateE	= 0x00000800,// E for embedded.  Just needed a unqique name
		Clip		= 0x00001000,

		ZoomX		= 0x00002000,
		ZoomY		= 0x00004000,
		Rotation	= 0x00008000,
		TransX		= 0x00010000,
		TransY		= 0x00020000
	} PDF_API GStateFlags;

	typedef enum _TStateFlags
	{
		NoTState		= 0,
		Font			= 0x00000001,
		CharSpacing		= 0x00000002,
		WordSpacing		= 0x00000004,
		TextRise		= 0x00000008,
		HScale			= 0x00000010,
		Leading			= 0x00000020,
		RenderMode		= 0x00000040,
		Justification	= 0x00000080,
		Position		= 0x00000100,
		Points			= 0x00000200/*,
		//Matrix		= 0x00000004,

		ZoomX			= 0x00000800,
		ZoomY			= 0x00001000,
		Rotation		= 0x00002000,
		TransX			= 0x00004000,
		TransY			= 0x00008000*/
	} PDF_API TStateFlags;

	typedef enum _TextRenderMode
	{
		Fill = 0,
		Stroke,
		FillAndStroke,
		Invisible
	} PDF_API TextRenderMode;
};// namespace Pdf

PDF_API std::ostream& operator<<( std::ostream& stm, const Pdf::AtomType& t );

PDF_API std::ostream& operator<<( std::ostream& stm, const Pdf::FilterType& t );

PDF_API std::ostream& operator<<( std::ostream& stm, const Pdf::OutputStyle& t );
