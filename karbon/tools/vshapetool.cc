/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <qevent.h>

#include "karbon_part.h"
#include "karbon_view.h"
#include "vglobal.h"
#include "vpainter.h"
#include "vpainterfactory.h"
#include "vpath.h"
#include "vshapecmd.h"
#include "vshapetool.h"


VShapeTool::VShapeTool( KarbonView* view, const QString& name, bool polar )
	: VTool( view ), m_name( name )
{
	m_isPolar = polar;
	m_isSquare   = false;
	m_isCentered = false;
}

void
VShapeTool::draw()
{
	VPainter* painter = view()->painterFactory()->editpainter();
	
	VPath* path = shape();
	path->setState( VObject::edit );
	path->draw( painter, path->boundingBox() );
	delete( path );
}

void
VShapeTool::mouseDrag( const KoPoint& current )
{
	recalc();
}

void
VShapeTool::mouseDragRelease( const KoPoint& /*current*/ )
{
	recalc();

	VShapeCmd* cmd = new VShapeCmd(
		&view()->part()->document(),
		name(),
		shape( true ) );

	view()->part()->addCommand( cmd, true );
	view()->selectionChanged();

	m_isSquare = false;
	m_isCentered = false;
}

void
VShapeTool::mouseDragShiftPressed( const KoPoint& current )
{
	m_isSquare = true;
	recalc();
}

void
VShapeTool::mouseDragCtrlPressed( const KoPoint& current )
{
	m_isCentered = true;
	recalc();
}

void
VShapeTool::mouseDragShiftReleased( const KoPoint& current )
{
	m_isSquare = false;
	recalc();
}

void
VShapeTool::mouseDragCtrlReleased( const KoPoint& current )
{
	m_isCentered = false;
	recalc();
}

void
VShapeTool::cancel()
{
	m_isSquare = false;
	m_isCentered = false;
}

void
VShapeTool::recalc()
{
	// Calculate radius and angle:
	if( m_isPolar )
	{
		// Radius:
		m_d1 = sqrt(
			( last().x() - first().x() ) * ( last().x() - first().x() ) +
			( last().y() - first().y() ) * ( last().y() - first().y() ) );

		// Angle:
		m_d2 = atan2( last().y() - first().y(), last().x() - first().x() );

		// Define pi/2 as "0.0":
		m_d2 -= VGlobal::pi_2;

		m_p = first();
	}
	else
	// Calculate width and height:
	{
		m_d1 = last().x() - first().x();
		m_d2 = last().y() - first().y();

		const int m_sign1 = m_d1 < 0.0 ? -1 : +1;
// TODO: revert when we introduce y-mirroring:
		const int m_sign2 = m_d2 < 0.0 ? +1 : -1;

		// Make unsigned:
		if( m_d1 < 0.0 )
			m_d1 = -m_d1;

		if( m_d2 < 0.0 )
			m_d2 = -m_d2;

		if ( m_isSquare )
		{
			if ( m_d1 > m_d2 )
				m_d2 = m_d1;
			else
				m_d1 = m_d2;
		}

		m_p.setX(
			first().x() - ( m_sign1 == -1 ? m_d1 : 0.0 ) );
// TODO: revert when we introduce y-mirroring:
		m_p.setY(
			first().y() + ( m_sign2 == -1 ? m_d2 : 0.0 ) );

		if ( m_isCentered )
		{
			m_p.setX( m_p.x() - m_sign1 * qRound( m_d1 * 0.5 ) );
			m_p.setY( m_p.y() + m_sign2 * qRound( m_d2 * 0.5 ) );
		}
	}
}

