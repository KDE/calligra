/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include <math.h>

#include "vaffinemap.h"
#include "vglobal.h"
#include "vpoint.h"

// default to the identity:
VAffineMap::VAffineMap()
	: m_a11( 1.0 ), m_a12( 0.0 ), m_v1( 0.0 ),
	  m_a21( 0.0 ), m_a22( 1.0 ), m_v2( 0.0 )
{
}

VAffineMap::VAffineMap(
		const double& a11, const double& a12,
		const double& a21, const double& a22,
		const double& v1,  const double& v2 )
	: m_a11( a11 ), m_a12( a12 ), m_v1( v1 ),
	  m_a21( a21 ), m_a22( a22 ), m_v2( v2 )
{
}

void
VAffineMap::translate( const double& dx, const double& dy )
{
	m_v1 += dx;
	m_v2 += dy;
}

void
VAffineMap::rotate( const double& ang )
{
	const double sina = sin( ang * VGlobal::pi_180 );
	const double cosa = cos( ang * VGlobal::pi_180 );

	VAffineMap map( cosa, sina, -sina, cosa, 0.0, 0.0 );
	mul( map );
}

void
VAffineMap::scale( const double& sx, const double& sy )
{
	VAffineMap map( sx, 0.0, 0.0, sy, 0.0, 0.0 );
	mul( map );
}

void
VAffineMap::shear( const double& sh, const double& sv )
{
	VAffineMap map( 1.0, sv, sh, 1.0, 0.0, 0.0 );
	mul( map );
}

void
VAffineMap::mul( const VAffineMap& map )
{
	const double a11 = map.m_a11 * m_a11 + map.m_a12 * m_a21;
	const double a12 = map.m_a11 * m_a12 + map.m_a12 * m_a22;
	const double a21 = map.m_a21 * m_a11 + map.m_a22 * m_a21;
	const double a22 = map.m_a21 * m_a12 + map.m_a22 * m_a22;
	const double v1  = map.m_v1  * m_a11 + map.m_v2  * m_a21 + m_v1;
	const double v2  = map.m_v1  * m_a12 + map.m_v2  * m_a22 + m_v2;

	m_a11 = a11; m_a12 = a12; m_v1 = v1;
	m_a21 = a21; m_a22 = a22; m_v2 = v2;
}

VPoint
VAffineMap::applyTo( const VPoint& point )
{
	return VPoint(
		m_a11 * point.x() + m_a21 * point.y() + m_v1,
		m_a12 * point.x() + m_a22 * point.y() + m_v2 );
}
