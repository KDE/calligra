/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSHAPETOOL_H__
#define __VSHAPETOOL_H__

#include <koPoint.h>

#include "vglobal.h"
#include "vtool.h"


class VShapeTool : public VTool
{
public:
	VShapeTool( KarbonView* view, bool polar = false );

	virtual bool eventFilter( QEvent* event );

	virtual void showDialog() const {}

protected:
	// make it "abstract":
	virtual ~VShapeTool() {}

	// derived tools implement specialised commands. d1, d2 are either
	// width, height or radius, angle.
	virtual VCommand* createCmd( double x, double y, double d1, double d2 ) = 0;

	// draw the object while it is edited:
	virtual void drawTemporaryObject( const KoPoint& p, double d1, double d2 ) = 0;

private:
	inline void recalcCoords();

	// calculate width/height or radius/angle?
	bool m_calcPolar;

	// input (mouse coordinates):
	KoPoint m_fp;
	KoPoint m_lp;

	// output:
	KoPoint m_p;
	double m_d1;
	double m_d2;

	// states:
	bool m_isDragging;
	bool m_isSquare;
	bool m_isCentered;
};


void
VShapeTool::recalcCoords()
{
	// calculate radius and angle:
	if( m_calcPolar )
	{
		// radius:
		m_d1 = sqrt(
			(double)(( m_lp.x() - m_fp.x() ) * ( m_lp.x() - m_fp.x() ) +
			( m_lp.y() - m_fp.y() ) * ( m_lp.y() - m_fp.y() ) ));

		// angle:
		m_d2 = atan2( (double)( m_lp.y() - m_fp.y() ), (double)( m_lp.x() - m_fp.x() ) );

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

		m_d1 = QABS( m_d1 );
		m_d2 = QABS( m_d2 );

		if ( m_isSquare )
		{
			if ( m_d1 > m_d2 )
				m_d2 = m_d1;
			else
				m_d1 = m_d2;
		}

		m_p.setX(
			m_fp.x() - static_cast<int>( m_sign1 < 0.0 ? m_d1 : 0.0 ) );
		m_p.setY(
			m_fp.y() - static_cast<int>( m_sign2 < 0.0 ? m_d2 : 0.0 ) );

		if ( m_isCentered )
		{
			m_p.setX( m_p.x() - m_sign1 * qRound( m_d1 * 0.5 ) );
			m_p.setY( m_p.y() - m_sign2 * qRound( m_d2 * 0.5 ) );
		}
	}
}

#endif

