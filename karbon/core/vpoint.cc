/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include "vpoint.h"

const double
VPoint::s_fractInvScale = 1.0/s_fractScale;


VPoint::VPoint( const double x, const double y )
	: m_x( x ), m_y( y )
{
}

VPoint::VPoint( const VPoint& point )
	: m_x( point.m_x ), m_y( point.m_y )
{
}

const QPoint&
VPoint::getQPoint( const double zoomFactor ) const
{
// TODO: round instead of casting
	m_QPoint.setX( static_cast<int> ( m_x*zoomFactor ) );
	m_QPoint.setY( static_cast<int> ( m_y*zoomFactor ) );

	return m_QPoint;
}

// make these functions inline someday :)
void
VPoint::moveTo( const double x, const double y )
{
	m_x = x; m_y = y;
}

void
VPoint::rmoveTo( const double x, const double y )
{
	m_x += x; m_y += y;
}

void
VPoint::setX( const double x )
{
	m_x = x;
}

void
VPoint::setY( const double y )
{
	m_y = y;
}
