#include "vpoint.h"

const double
VPoint::s_fractInvScale = 1.0/s_fractScale;

VPoint::VPoint()
	: m_x( 0.0 ), m_y( 0.0 ), m_refCount( 1 )
{
}

VPoint::VPoint( const VPoint& p )
	: m_x( p.m_x ), m_y( p.m_y ), m_refCount( 1 )
{
}

VPoint::VPoint( const double& x, const double& y )
	: m_x( x ), m_y( y ), m_refCount( 1 )
{
}

const QPoint&
VPoint::getQPoint( const double& zoomFactor ) const
{
	m_QPoint.setX( static_cast<int> ( m_x*zoomFactor ) );
	m_QPoint.setY( static_cast<int> ( m_y*zoomFactor ) );

	return m_QPoint;
}

// make these functions inline someday :)
void
VPoint::moveTo( const double& x, const double& y )
{
	m_x = x; m_y = y;
}

void
VPoint::rmoveTo( const double& x, const double& y )
{
	m_x += x; m_y += y;
}

void
VPoint::setX( const double& x )
{
	m_x = x;
}

void
VPoint::setY( const double& y )
{
	m_y = y;
}
