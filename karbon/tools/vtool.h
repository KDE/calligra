/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VTOOL_H__
#define __VTOOL_H__

#include <qpoint.h>

class QEvent;

class KarbonPart;
class KarbonView;
class VCommand;

class VTool
{
public:
	VTool( KarbonPart* part = 0L, bool polar = false );
	virtual ~VTool() {}

	// derived tools implement their specialised commands:
	virtual VCommand* createCmdFromDialog( const QPoint& point ) = 0;
	virtual VCommand* createCmdFromDragging( const QPoint& tl, const QPoint& br ) = 0;

	// draw the object while it is edited:
	virtual void drawTemporaryObject(
		KarbonView* view, const QPoint& tl, const QPoint& br ) = 0;

	virtual bool eventFilter( KarbonView* view, QEvent* event );

	KarbonPart* part() const { return m_part; }

private:
	inline void recalcCoords();

	// that's our part:
	KarbonPart* m_part;

	// states:
	bool m_isDragging;
	bool m_isSquare;
	bool m_isCentered;

	// calculate tl, br or radius,angular?
	bool m_calcPolar;

	// (input) mouse coordinates:
	QPoint m_fp;
	QPoint m_lp;

	// (output) painting coordinates:
	QPoint m_tl;
	QPoint m_br;
};

inline void
VTool::recalcCoords()
{
	// calculate angle and radius:
	if( m_calcPolar )
	{
		m_tl = m_fp;
		m_br = m_lp;
	}
	else
	// calculate width and height:
	{
		int m_width;
		int m_height;
		int m_signW;
		int m_signH;

		m_width  = m_lp.x() - m_fp.x();
		m_height = m_lp.y() - m_fp.y();
		m_signW = m_width  < 0 ? -1 : +1;
		m_signH = m_height < 0 ? -1 : +1;

		// the contour doesnt "belong" to the shape. 
		m_width  = QABS( m_width );
		m_height = QABS( m_height );

		if ( m_isSquare )
		{
			if ( m_width > m_height )
				m_height = m_width;
			else
				m_width = m_height;
		}

		if ( m_isCentered )
		{
			m_tl.setX( m_fp.x() - m_signW * qRound( m_width ) );
			m_tl.setY( m_fp.y() - m_signH * qRound( m_height) );
			m_br.setX( m_fp.x() + m_signW * qRound( m_width ) );
			m_br.setY( m_fp.y() + m_signH * qRound( m_height) );
		}
		else
		{
			m_tl = m_fp;
			m_br.setX( m_fp.x() + m_signW * m_width );
			m_br.setY( m_fp.y() + m_signH * m_height );
		}
	}
}

#endif
