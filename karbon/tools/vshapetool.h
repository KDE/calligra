/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSHAPETOOL_H__
#define __VSHAPETOOL_H__

#include <qstring.h>

#include <koPoint.h>

#include "vglobal.h"
#include "vtool.h"


class VPath;


class VShapeTool : public VTool
{
public:
	VShapeTool( KarbonView* view, const QString& name, bool polar = false );

	virtual void showDialog() const {}

	const QString& name() const { return m_name; }

protected:
	virtual void mousePressed( QMouseEvent * );
	virtual void mouseMoved( QMouseEvent * );
	virtual void dragShiftPressed();
	virtual void dragCtrlPressed();
	virtual void dragShiftReleased();
	virtual void dragCtrlReleased();
	virtual void dragAltPressed();
	virtual void mouseReleased( QMouseEvent * );
	virtual void cancel(); // ESC pressed

	// Make it "abstract":
	virtual ~VShapeTool() {}

	virtual VPath* shape( bool decide = false ) const = 0;
	
	/// Draws the object while it is edited.
	virtual void drawTemporaryObject();

	/// Output coordinates.
	KoPoint m_p;
	double m_d1;
	double m_d2;

private:
	inline void recalcCoords();

	QString m_name;
	
	/// Calculate wiidth/height or radius/angle?
	bool m_isPolar;

	/// States:
	bool m_isSquare;
	bool m_isCentered;
};


void
VShapeTool::recalcCoords()
{
	// Calculate radius and angle:
	if( m_isPolar )
	{
		// Radius:
		m_d1 = sqrt(
			( m_lp.x() - m_fp.x() ) * ( m_lp.x() - m_fp.x() ) +
			( m_lp.y() - m_fp.y() ) * ( m_lp.y() - m_fp.y() ) );

		// Angle:
		m_d2 = atan2( m_lp.y() - m_fp.y(), m_lp.x() - m_fp.x() );

		// Define pi/2 as "0.0":
		m_d2 -= VGlobal::pi_2;

		m_p = m_fp;
	}
	else
	// Calculate width and height:
	{
		m_d1 = m_lp.x() - m_fp.x();
		m_d2 = m_lp.y() - m_fp.y();

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
			m_fp.x() - ( m_sign1 == -1 ? m_d1 : 0.0 ) );
// TODO: revert when we introduce y-mirroring:
		m_p.setY(
			m_fp.y() + ( m_sign2 == -1 ? m_d2 : 0.0 ) );

		if ( m_isCentered )
		{
			m_p.setX( m_p.x() - m_sign1 * qRound( m_d1 * 0.5 ) );
			m_p.setY( m_p.y() - m_sign2 * qRound( m_d2 * 0.5 ) );
		}
	}
}

#endif

