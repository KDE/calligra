#include "vpath.h"

VPath::VPath()
    : VObject(), m_currentPoint = 0L
{
}

VPath::~VPath()
{
    // remove primitives:
    VPrimitive* prim;
    for ( prim=m_primitives.first(); prim!=0L; prim=m_primitives.next() )
    {
	delete prim;
    }
}

void
VPath::draw( QPainter& /*p*/ )
{
}
