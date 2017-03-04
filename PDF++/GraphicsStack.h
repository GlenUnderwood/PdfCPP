#pragma once

#include "GState.h"
#include "TState.h"

namespace Pdf {
namespace Content {

class PDF_API pdfGraphicsStack
{
public:
	pdfGraphicsStack();
	pdfGraphicsStack( const pdfGraphicsStack& src );
	virtual ~pdfGraphicsStack();

	pdfGraphicsStack& operator=( const pdfGraphicsStack& src );

	pdfGState& GetGState();
	void SetGState( const pdfGState & rhs );

	pdfTState& GetTState();
	void SetTState( const pdfTState & rhs );

	inline pdfGraphicsStack* GetPrev() {
		return m_Prev;
	}
	inline void SetPrev(pdfGraphicsStack* pPrev) {
		m_Prev = pPrev;
	}
protected:
	std::unique_ptr<pdfGState> m_GState;
	std::unique_ptr<pdfTState> m_TState;
	//
	pdfGraphicsStack* m_Prev;
};


};
};