/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VMTOOL_H__
#define __VMTOOL_H__

#include <math.h>
#include <qpoint.h>

#include "vglobal.h"
#include "vtool.h"

class QEvent;

class KarbonPart;
class KarbonView;
class VCommand;

class VMTool : public VTool
{
public:
	VMTool( KarbonPart* part = 0L, bool polar = false );
	virtual ~VMTool() {}

	// derived tools implement specialised commands. d1, d2 are either
	// width, height or radius, angle.
	virtual VCommand* createCmd( double x, double y, double d1, double d2 ) = 0;

	// draw the object while it is edited:
	virtual void drawTemporaryObject(
		KarbonView* view, const QPoint& p, double d1, double d2 ) = 0;

	// allows for things like copying the object(s) to manipulate
	virtual void startDragging() = 0;

	virtual bool eventFilter( KarbonView* view, QEvent* event );

protected:	// for selection-tool. it has to reimplement eventFilter.
	inline void recalcCoords();
};

inline void
VMTool::recalcCoords()
{
	// calculate radius and angle:
	if( m_calcPolar )
	{
		// radius:
		m_d1 = sqrt(
			( m_lp.x() - m_fp.x() ) * ( m_lp.x() - m_fp.x() ) +
			( m_lp.y() - m_fp.y() ) * ( m_lp.y() - m_fp.y() ) );

		// angle:
		m_d2 = atan2( ( m_lp.y() - m_fp.y() ) , ( m_lp.x() - m_fp.x() ) );

		// define pi/2 as "0.0":
		m_d2 -= VGlobal::pi_2;

		m_p = m_fp;
	}
	else
	// calculate width and height:
	{
		m_d1 = m_lp.x() - m_fp.x();
		m_d2 = m_lp.y() - m_fp.y();

		const int m_sign1 = m_d1 < 0.0 ? -1 : +1;
		const int m_sign2 = m_d2 < 0.0 ? -1 : +1;

		//m_d1 = QABS( m_d1 );
		//m_d2 = QABS( m_d2 );

		if ( m_isSquare )
		{
			if ( m_d1 > m_d2 )
				m_d2 = m_d1;
			else
				m_d1 = m_d2;
		}

		m_p.setX( int( m_fp.x() ) /* - ( m_sign1 < 0.0 ? m_d1 : 0.0 */ );
		m_p.setY( int( m_fp.y() ) /* - ( m_sign2 < 0.0 ? m_d2 : 0.0 */ );

		if ( m_isCentered )
		{
			m_p.setX( m_p.x() - m_sign1 * qRound( m_d1 * 0.5 ) );
			m_p.setY( m_p.y() - m_sign2 * qRound( m_d2 * 0.5 ) );
		}
	}
}

#endif
