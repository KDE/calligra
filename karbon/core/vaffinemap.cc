/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include <math.h>

#include "vaffinemap.h"
#include "vglobal.h"
#include "vpoint.h"


VAffineMap::VAffineMap(
		const double a11, const double a12,
		const double a21, const double a22,
		const double v1,  const double v2 )
	: m_a11( a11 ), m_a12( a12 ), 
	  m_a21( a21 ), m_a22( a22 ),
	  m_v1( v1 ), m_v2( v2 )
{
}

void
VAffineMap::reset()
{
	m_a11 = 1.0; m_a12 = 0.0; m_v1 = 0.0;
	m_a21 = 0.0; m_a22 = 1.0; m_v2 = 0.0;
}

void
VAffineMap::translate( const double dx, const double dy )
{
	VAffineMap aff_map( 1.0, 0.0, 0.0, 1.0, dx, dy );
	mul( aff_map );
}

void
VAffineMap::rotate( const double ang )
{
	const double sina = sin( ang * VGlobal::pi_180 );
	const double cosa = cos( ang * VGlobal::pi_180 );

	VAffineMap aff_map( cosa, sina, -sina, cosa, 0.0, 0.0 );
	mul( aff_map );
}

void
VAffineMap::mirror( const bool horiz, const bool verti )
{
	VAffineMap aff_map(
		( horiz ? -1.0 : 1.0 ), 0.0,
		0.0, ( verti ? -1.0 : 1.0 ),
		0.0, 0.0 );
	mul( aff_map );
}

void
VAffineMap::scale( const double sx, const double sy )
{
	VAffineMap aff_map( sx, 0.0, 0.0, sy, 0.0, 0.0 );
	mul( aff_map );
}

void
VAffineMap::shear( const double sh, const double sv )
{
	VAffineMap aff_map( 1.0, sv, sh, 1.0, 0.0, 0.0 );
	mul( aff_map );
}

void
VAffineMap::skew( const double ang )
{
	const double tana = tan( ang * VGlobal::pi_180 );

	VAffineMap aff_map( 1.0, 0.0, tana, 1.0, 0.0, 0.0 );
	mul( aff_map );
}

void
VAffineMap::mul( const VAffineMap& affMap )
{
	const double a11 = affMap.m_a11 * m_a11 + affMap.m_a12 * m_a21;
	const double a12 = affMap.m_a11 * m_a12 + affMap.m_a12 * m_a22;
	const double a21 = affMap.m_a21 * m_a11 + affMap.m_a22 * m_a21;
	const double a22 = affMap.m_a21 * m_a12 + affMap.m_a22 * m_a22;

	m_v1 = affMap.m_a11 * m_v1 + affMap.m_a12 * m_v2 + affMap.m_v1;
	m_v2 = affMap.m_a21 * m_v1 + affMap.m_a22 * m_v2 + affMap.m_v2;

	m_a11 = a11; m_a12 = a12;
	m_a21 = a21; m_a22 = a22;
}

VPoint
VAffineMap::map( const VPoint& point ) const
{
	return VPoint(
		m_a11 * point.x() + m_a12 * point.y() + m_v1,
		m_a21 * point.x() + m_a22 * point.y() + m_v2 );
}
