#include "vrect.h"

VRect::VRect()
{
}

VRect::VRect( const double& l, const double& t, const double& r, const double& b )
    : m_tl( l, t ), m_br( r, b )
{
}

VRect::VRect( VPoint& tl, VPoint& br )
    : m_tl( tl ), m_br( br )
{
}
