/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VCTOOLELLIPSE_H__
#define __VCTOOLELLIPSE_H__

#include <qbitmap.h>
#include <qpoint.h>

#include "karbon_view.h"
#include "vccmd_ellipse.h"
#include "vpath.h"
#include "vpoint.h"
#include "vtool.h"

class KarbonPart;
class VCDlgEllipse;

// A singleton state to create an ellipse

class VCToolEllipse : public VTool
{
public:
	virtual ~VCToolEllipse();
	static VCToolEllipse* instance( KarbonPart* part );

	virtual bool eventFilter( KarbonView* view, QEvent* event );

protected:
	VCToolEllipse( KarbonPart* part );

private:
	// inline helper functions:
	void recalcCoords();
	void drawTemporaryObject( KarbonView* view );

	static VCToolEllipse* s_instance;

	KarbonPart* m_part;
	VCDlgEllipse* m_dialog;

	bool m_isDragging;
	bool m_isSquare;
	bool m_isCentered;

	// mouse coordinates::
	QPoint m_fp;
	QPoint m_lp;

	// painting coordinates:
	QPoint m_tl;
	QPoint m_br;
	int m_width;
	int m_height;
	int m_signW;
	int m_signH;
};

inline void
VCToolEllipse::recalcCoords()
{
	m_width  = m_lp.x() - m_fp.x();
	m_height = m_lp.y() - m_fp.y();
	m_signW = m_width  < 0 ? -1 : +1;
	m_signH = m_height < 0 ? -1 : +1;
	// ABS():
	m_width  = QABS( m_width ) + 1;
	m_height = QABS( m_height ) + 1;

	if ( m_isSquare )
	{
		if ( m_width > m_height )
			m_height = m_width;
		else
			m_width = m_height;
	}

	if ( m_isCentered )
	{
		m_tl.setX( m_fp.x() - m_signW * qRound( m_width * 0.5 ) );
		m_tl.setY( m_fp.y() - m_signH * qRound( m_height * 0.5 ) );
		m_br.setX( m_fp.x() + m_signW * qRound( m_width * 0.5 ) );
		m_br.setY( m_fp.y() + m_signH * qRound( m_height * 0.5 ) );
	}
	else
	{
		m_tl = m_fp;
		m_br.setX( m_fp.x() + m_signW * m_width );
		m_br.setY( m_fp.y() + m_signH * m_height );
	}
}

inline void
VCToolEllipse::drawTemporaryObject( KarbonView* view )
{
	QPainter painter( view->canvasWidget()->viewport() );
	painter.setPen( Qt::yellow );
	painter.setRasterOp( Qt::XorROP );

	// we need a bitmap to avoid selfinteraction of small shapes and
	// center crosses because of XorROP.

	// For small shapes use the Bitmap for the whole shape:
	if ( m_width < 6 || m_height < 6 )
	{
		QBitmap bm( m_width + 1, m_height + 1, true );
		QPainter p;

		p.begin( &bm );
		// Qt*s drawEllipse actually would be sufficent here, except for one thing:
		// an ellipse with height/width==1 vanishes. that's why we use
		// VCCmdEllipse::createPath. this is a bit slower.
		VPoint br;
		br.setFromQPoint( m_width, m_height, view->zoomFactor() );

		// let the command create the necessary qpointarray for us:
		VCCmdEllipse* cmd =
			new VCCmdEllipse( m_part, 0.0, 0.0, br.x(), br.y() );

		VPath* path = cmd->createPath();

		p.drawPolygon( path->getQPointArray( view->zoomFactor() ) );

		// draw center cross:
		const int center_x = qRound( m_width * 0.5 );
		const int center_y = qRound( m_height * 0.5 );
		p.drawLine(
			center_x - 2, center_y - 2,
			center_x + 2, center_y + 2 );
		p.drawLine(
			center_x - 2, center_y + 2,
			center_x + 2, center_y - 2 );

		p.end();

		bm.setMask( bm );

		painter.drawPixmap(
			m_signW > 0 ? m_tl.x() : m_br.x(),
			m_signH > 0 ? m_tl.y() : m_br.y(), bm );

		delete( cmd );
		delete( path );
	}
	else
	{
		painter.drawEllipse(
			m_tl.x() < m_br.x() ? m_tl.x() : m_br.x(),
			m_tl.y() < m_br.y() ? m_tl.y() : m_br.y(), m_width, m_height );

		QBitmap bm( 5, 5, true );
		QPainter p;

		p.begin( &bm );
		p.drawLine( 0, 0, 4, 4 );
		p.drawLine( 0, 4, 4, 0 );
		p.end();

		bm.setMask( bm );

		painter.drawPixmap(
			m_tl.x() + m_signW * qRound( m_width * 0.5 ) - 2,
			m_tl.y() + m_signH * qRound( m_height * 0.5 ) - 2, bm );
 	}
}

#endif
