#include "stdafx.h"
#include "GraphicsStack.h"

namespace Pdf {
namespace Content {

pdfGraphicsStack::pdfGraphicsStack() : m_Prev(nullptr)
{
}

pdfGraphicsStack::pdfGraphicsStack(const pdfGraphicsStack& src): m_Prev(nullptr)
{
	operator=( src );
}

pdfGraphicsStack::~pdfGraphicsStack()
{
}
pdfGraphicsStack& pdfGraphicsStack::operator=( const pdfGraphicsStack& src )
{
	if (&src != this)
	{
		m_GState.reset();
		m_TState.reset();
		if (src.m_GState.get() != nullptr)
			m_GState = std::make_unique<pdfGState>( src.m_GState );
		if (src.m_TState.get() != nullptr)
			m_TState = std::make_unique<pdfTState>( src.m_TState );
	}
}

pdfGState& pdfGraphicsStack::GetGState()
{
	if (m_GState.get() != nullptr)
		m_GState = std::make_unique<pdfGState>();
	return *m_GState.get();
}
void pdfGraphicsStack::SetGState( const pdfGState & rhs )
{
	m_GState.reset();
	m_GState = std::make_unique<pdfGState>();
}

pdfTState& pdfGraphicsStack::GetTState()
{
	if (m_TState.get() != nullptr)
		m_TState = std::make_unique<pdfTState>();
	return *m_TState.get();
}

void pdfGraphicsStack::SetTState( const pdfTState & rhs )
{
	m_TState.reset();
	m_TState = std::make_unique<pdfTState>();
}


};

};
