#pragma once
#include <memory>
#include <string>
#include <vector>
#include <map>
#include "Enum.h"
#include "ClipItem.h"

namespace Pdf{

class CDashPattern
{
public:
	CDashPattern() :m_Phase( 0 ) { }
	CDashPattern( const CDashPattern& rhs );

	CDashPattern& operator=( const CDashPattern& rhs );

	std::vector<int>& GetArray();
	void SetArray( const std::vector<int>& src );

	int GetPhase() const;
	void SetPhase( const int& src );

	bool operator==( const CDashPattern& rhs );
	bool operator!=( const CDashPattern& rhs ) {
		return !operator==( rhs );
	};
private:
	std::vector<int> m_Array;
	int m_Phase;
};

class CGStateData //: public IAtomData
{
public:
	CGStateData(void);
	CGStateData(const CGStateData& src);
	~CGStateData(void);

	typedef std::shared_ptr<CGStateData> Ptr;

	CGStateData& operator=( const CGStateData& src );

	GStateFlags m_Flags;
	pdfColor m_StrokeColor;
	pdfColor m_FillColor;
	float m_LineWidth;
	float m_MiterLimit;
	byte m_FlatnessTolerance;
	byte m_LineCap;

	byte m_LineJoin;
	std::string m_RenderingIntent;

	std::shared_ptr<CDashPattern> m_Dash;

	Content::pdfClipItem m_pathClip;
	pdfExtGState m_ExtGState;

	Geometry::pdfMatrix m_Mat;
};


};//namespace Pdf
