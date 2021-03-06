#pragma once
#include <string>
#include <limits>

//#include "pdfTypes.h"

#include "ExportDef.h"

namespace Pdf{

#ifdef _MSC_VER
	const long MaxValueInt32 = INT_MAX;
	const long MinValueInt32 = INT_MIN;
	const int64 MaxValueInt64 = LONG_MAX;
	const int64 MinValueInt64 = LONG_MIN;
#else
	const long MaxValueInt32 = std::numeric_limits<int>::max();
	const long MinValueInt32 = std::numeric_limits<int>::min();
	const int64 MaxValueInt64 = std::numeric_limits<int64>::max();
	const int64 MinValueInt64 = std::numeric_limits<int64>::min();
#endif

//struct Keys{
	#define Key_xref "xref"
	#define Key_trailer "trailer"
	#define Key_startxref "startxref"
	//#define Key_Name "Name"
	#define Key_stream "stream"
	#define Key_endstream "endstream"
	#define Key_Names "Names"
	#define Key_Limits "Limits"
	#define Key_ID "ID"
	#define Key_Root "Root"
	#define Key_Catalog "Catalog"
	#define Key_Encrypt "Encrypt"
	#define Key_Contents "Contents"
	#define Key_Resources "Resources"
	#define Key_Type "Type"
	#define Key_Subtype "Subtype"
	#define Key_XObject "XObject"
	#define Key_Font "Font"
	#define Key_Image "Image"
	#define Key_ProcSet "ProcSet"
	#define Key_FlateDecode "FlateDecode"
	#define Key_CCITTFaxDecode "CCITTFaxDecode"
	#define Key_DCTDecode "DCTDecode"
	#define Key_LZWDecode "LZWDecode"
	#define Key_DeviceRGB "DeviceRGB"
	#define Key_DeviceGray "DeviceGray"
	#define Key_DeviceCMYK "DeviceCMYK"
	#define Key_Separation "Separation"
	#define Key_Indexed "Indexed"
	#define Key_BitsPerComponent "BitsPerComponent"
	#define Key_Interpolate "Interpolate"
	#define Key_ColorSpace "ColorSpace"
	#define Key_SMask "SMask"
	#define Key_IsMask "IsMask"
	#define Key_DecodeParms "DecodeParms"
	#define Key_Predictor "Predictor"
	#define Key_Widths "Widths"
	#define Key_Width "Width"
	#define Key_Height "Height"
	#define Key_BaseFont "BaseFont"
	#define Key_FontName "FontName"
	#define Key_FontFile "FontFile"
	#define Key_FontFile2 "FontFile2"
	#define Key_FontFile3 "FontFile3"
	#define Key_FontFamily "FontFamily"
	#define Key_FontWeight "FontWeight"
	#define Key_FontBBox "FontBBox"
	#define Key_ToUnicode "ToUnicode"
	#define Key_Encoding "Encoding"
	#define Key_FontDescriptor "FontDescriptor"
	#define Key_DescendantFonts "DescendantFonts"
	#define Key_Ascent "Ascent"
	#define Key_Descent "Descent"
	#define Key_StemV "StemV"
	#define Key_CapHeight "CapHeight"
	#define Key_XHeight "XHeight"
	#define Key_Weight "Weight"
	#define Key_Flags "Flags"
	#define Key_ItalicAngle "ItalicAngle"
	#define Key_Size "Size"
	#define Key_ExtGState "ExtGState"
	#define Key_Count "Count"
	#define Key_Page "Page"
	#define Key_Pages "Pages"
	#define Key_Kids "Kids"
	#define Key_FirstChar "FirstChar"
	#define Key_LastChar "LastChar"
	#define Key_Form "Form"
	#define Key_Outlines "Outlines"
	#define Key_Last "Last"
	#define Key_First "First"
	#define Key_Prev "Prev"
	#define Key_Next "Next"
	#define Key_Parent "Parent"
	#define Key_Title "Title"
	#define Key_Dests "Dests"
	#define Key_Dest "Dest"
	#define Key_A "A"
	#define Key_D "D"
	#define Key_Annot "Annot"
	#define Key_Annotation Annot
	#define Key_Annots "Annots"
	#define Key_Matrix "Matrix"
	#define Key_Rect "Rect"
	#define Key_Rotation "Rotation"
	#define Key_Decode "Decode"
	#define Key_Properties "Properties"

	#define Key_NotDef ".notdef"
	//#define Key_CRLF Environment.NewLine
	#define Key_SPACE " "

	//namespace Ops{
		#define Key_Q "Q"
		#define Key_q "q"
		#define Key_cm "cm"
		#define Key_Tm "Tm"
		#define Key_Do "Do"
		#define Key_BT "BT"
		#define Key_ET "ET"
		#define Key_BMC "BMC"
		#define Key_BDC "BDC"
		#define Key_EMC "EMC"
		#define Key_DP "DP"
		#define Key_MP "MP"
		#define Key_Tf "Tf"
		#define Key_Tj "Tj"
		#define Key_TJ "TJ"
		#define Key_Tstar "T*"
		#define Key_TD "TD"
		#define Key_Td "Td"
		#define Key_Tr "Tr"
		#define Key_Th "Th"
		#define Key_Tw "Tw"
		#define Key_Tc "Tc"
		#define Key_Ts "Ts"
		#define Key_Tz "Tz"
		#define Key_TL "TL"
		#define Key_g "g"
		#define Key_G "G"
		#define Key_rg "rg"
		#define Key_RG "RG"
		#define Key_K "K"
		#define Key_k "k"
		#define Key_m "m"
		#define Key_l "l"
		#define Key_re "re"
		#define Key_n "n"
		#define Key_f "f"
		#define Key_Wstar "W*"
		#define Key_w "w"
		#define Key_W "W"
		#define Key_gs "gs"
		#define Key_d "d" // Dash Array
		#define Key_c "c" // curveto
		#define Key_ri "ri" // RenderingIntent
		#define Key_bstar "b*"
		#define Key_quote "'"
		#define Key_dblquote "\""


	//};// Ops
	
//};// Keys
};// Pdf
