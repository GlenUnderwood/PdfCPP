#pragma once
#include "AtomData.h"

namespace Pdf {
namespace Filter {

class CTiffMgr
{
public:
	CTiffMgr(void);
	//virtual ~CTiffMgr(void);

	void EncodeG4Block( const CSharedBuff& buffSrc, CSharedBuff& buffDest, int iBPP, int iWidth, int iHeight, int iComp  );
	void DecodeG4Block(	const CSharedBuff& buffSrc, CSharedBuff& buffDest, int iWidth, int iHeight, int BPP, int K );
	void TiffFromG4Block( const CSharedBuff& buffSrc, CSharedBuff& buffDest, int iWidth, int iHeight, int BPP, int K );
};

};
};
