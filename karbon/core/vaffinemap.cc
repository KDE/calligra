#include "vaffinemap.h"

VAffineMap::VAffineMap()
	: m_a11( 1.0 ), m_a12( 0.0 ), m_v1( 0.0 ),
	  m_a21( 0.0 ), m_a22( 1.0 ), m_v2( 0.0 )
{
}

