/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VMTOOLSELECT_H__
#define __VMTOOLSELECT_H__

#include "vtool.h"

class KarbonPart;
class KarbonView;

// A singleton state to select object(s)

class VMToolSelect : public VTool
{
public:
	virtual ~VMToolSelect();
	static VMToolSelect* instance( KarbonPart* part );

	virtual VCommand* createCmd( double x, double y, double d1, double d2 );

	virtual void drawTemporaryObject(
		KarbonView* view, const QPoint& p, double d1, double d2 );

protected:
	VMToolSelect( KarbonPart* part );

	virtual inline void recalcCoords();

private:
	static VMToolSelect* s_instance;
	KarbonView* m_view;	// to get zoomFactor
	enum { NoTransform, Moving, Scaling, Rotating } m_TransformState;
};

inline void
VMToolSelect::recalcCoords()
{
	// calculate radius and angle:
	if( m_calcPolar )
	{
		// radius:
		m_d1 = sqrt(
			( m_lp.x() - m_fp.x() ) * ( m_lp.x() - m_fp.x() ) +
			( m_lp.y() - m_fp.y() ) * ( m_lp.y() - m_fp.y() ) );

		// angle:
		m_d2 = atan2( ( m_lp.y() - m_fp.y() ), ( m_lp.x() - m_fp.x() ) );

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

		m_p.setX(
			m_fp.x()); // - static_cast<int>( m_sign1 < 0.0 ? m_d1 : 0.0 ) );
		m_p.setY(
			m_fp.y()); // - static_cast<int>( m_sign2 < 0.0 ? m_d2 : 0.0 ) );

		if ( m_isCentered )
		{
			m_p.setX( m_p.x() - m_sign1 * qRound( m_d1 * 0.5 ) );
			m_p.setY( m_p.y() - m_sign2 * qRound( m_d2 * 0.5 ) );
		}
	}
}
#endif
