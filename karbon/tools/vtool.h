/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VTOOL_H__
#define __VTOOL_H__

#include <math.h>
#include <qpoint.h>

#include "vglobal.h"

#include <kdebug.h>

class QEvent;

class KarbonPart;
class KarbonView;
class VCommand;

class VTool
{
public:
	VTool( KarbonPart* part = 0L, bool polar = false );
	virtual ~VTool() {}

	// derived tools implement specialised commands. d1, d2 are either
	// width, height or radius, angle.
	virtual VCommand* createCmd( const QPoint& p, double d1, double d2 ) = 0;

	// draw the object while it is edited:
	virtual void drawTemporaryObject(
		KarbonView* view, const QPoint& p, double d1, double d2 ) = 0;

	virtual bool eventFilter( KarbonView* view, QEvent* event );

	KarbonPart* part() const { return m_part; }

private:
	// that's our part:
	KarbonPart* m_part;

protected:	// for selection-tool. it has to reimplement eventFilter.
	inline void recalcCoords();

	// states:
	bool m_isDragging;
	bool m_isSquare;
	bool m_isCentered;

	// calculate width, height or radius, angle?
	bool m_calcPolar;

	// input (mouse coordinates):
	QPoint m_fp;
	QPoint m_lp;

	// output:
	QPoint m_p;
	double m_d1;
	double m_d2;
};

inline void
VTool::recalcCoords()
{
	// calculate angle and radius:
	if( m_calcPolar )
	{
		// radius:
		m_d1 = sqrt(
			( m_lp.x() - m_fp.x() ) * ( m_lp.x() - m_fp.x() ) +
			( m_lp.y() - m_fp.y() ) * ( m_lp.y() - m_fp.y() ) );

		// angle:
		if( m_lp.x() - m_fp.x() == 0.0 )	// catch division by zero.
		{
kdDebug() << "*" << endl;
			m_d2 = m_lp.y() - m_fp.y() >= 0.0
				? +VGlobal::pi_2
				: -VGlobal::pi_2;
		}
		else
		{
			m_d2 = atan( ( m_lp.y() - m_fp.y() ) / ( m_lp.x() - m_fp.x() ) );
		}

		// define pi/2 as "0.0":
		m_d2 -= VGlobal::pi_2;

kdDebug() << m_d2 << endl;
		m_p = m_fp;
	}
	else
	// calculate width and height:
	{
		m_d1 = m_lp.x() - m_fp.x();
		m_d2 = m_lp.y() - m_fp.y();

		const int m_sign1 = m_d1 < 0.0 ? -1 : +1;
		const int m_sign2 = m_d2 < 0.0 ? -1 : +1;

		m_d1 = QABS( m_d1 );
		m_d2 = QABS( m_d2 );

		if ( m_isSquare )
		{
			if ( m_d1 > m_d2 )
				m_d2 = m_d1;
			else
				m_d1 = m_d2;
		}

		m_p.setX( m_fp.x() - ( m_sign1 < 0.0 ? m_d1 : 0.0 ) );
		m_p.setY( m_fp.y() - ( m_sign2 < 0.0 ? m_d2 : 0.0 ) );

		if ( m_isCentered )
		{
			m_p.setX( m_p.x() - m_sign1 * qRound( m_d1 * 0.5 ) );
			m_p.setY( m_p.y() - m_sign2 * qRound( m_d2 * 0.5 ) );
		}
	}
}

#endif
