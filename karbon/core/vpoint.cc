#include "vpoint.h"

const double
VPoint::s_fractInvScale = 1.0/s_fractScale;

VPoint::VPoint()
    : m_x(0.0), m_y(0.0), m_QPoint(0,0), m_isDirty(true)
{
}

VPoint::VPoint( const VPoint& p )
    : m_x(p.m_x), m_y(p.m_y), m_isDirty(true)
{
}

VPoint::VPoint( const double& x, const double& y )
    : m_x(x), m_y(y), m_isDirty(true)
{
}

const QPoint&
VPoint::getQPoint() const
{
    // recalculate QPoint-value:
    if ( m_isDirty )
    {
	m_QPoint.setX( static_cast<int> (m_x*s_fractScale) );
	m_QPoint.setY( static_cast<int> (m_y*s_fractScale) );
	m_isDirty = false;
    }
    return m_QPoint;
}

void
VPoint::moveTo( double& x, double& y )
{
    m_x = x; m_y = y;
    m_isDirty=true;
}

void
VPoint::rmoveTo( double& x, double& y )
{
    m_x += x; m_y += y;
    m_isDirty=true;
}

void
VPoint::setX( double& x )
{
    m_x = x;
    m_isDirty=true;
}

void
VPoint::setY( double& y )
{
    m_y = y;
    m_isDirty=true;
}
