/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include "vpoint.h"

//const double
//VPoint::s_fractInvScale = 1.0/s_fractScale;

double
VPoint::m_lastZoomFactor = 0.0;

VPoint::VPoint( const double x, const double y )
	: m_x( x ), m_y( y ), m_isDirty( true )
{
}

VPoint::VPoint( const VPoint& point )
	: m_x( point.m_x ), m_y( point.m_y ), m_isDirty( true )
{
}

const QPoint&
VPoint::getQPoint( const double zoomFactor ) const
{
	if ( m_isDirty || zoomFactor != m_lastZoomFactor )
	{
// TODO: - round instead of casting
		m_QPoint.setX( static_cast<int> ( m_x*zoomFactor ) );
		m_QPoint.setY( static_cast<int> ( m_y*zoomFactor ) );

		m_lastZoomFactor = zoomFactor;
		m_isDirty = false;
	}

	return m_QPoint;
}

// make these functions inline someday :)
void
VPoint::moveTo( const double x, const double y )
{
	m_x = x; m_y = y;
	m_isDirty = true;
}

void
VPoint::rmoveTo( const double x, const double y )
{
	m_x += x; m_y += y;
	m_isDirty = true;
}

void
VPoint::setX( const double x )
{
	m_x = x;
	m_isDirty = true;
}

void
VPoint::setY( const double y )
{
	m_y = y;
	m_isDirty = true;
}
