/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VCTOOLELLIPSE_H__
#define __VCTOOLELLIPSE_H__

#include <qpixmap.h>
#include <qpoint.h>

#include "karbon_view.h"
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
	bool m_isCircle;
	bool m_isCentered;

	// mouse coordinates::
	QPoint m_fp;
	QPoint m_lp;
	// painting coordinates:
	QPoint m_tl;
	QPoint m_br;
	QPoint m_center;
};

inline void
VCToolEllipse::recalcCoords()
{
	int width;
	int height;

	if ( m_isCircle )
	{
		width  = m_lp.x() - m_fp.x();
		height = m_fp.y() - m_lp.y();

		if ( QABS( width ) > QABS( height ) )
			height = ( height < 0 ? -1 : 1 ) * QABS( width );
		else
			width = ( width < 0 ? -1 : 1 ) * QABS( height );
	}
	else
	{
		width  = m_lp.x() - m_fp.x();
		height = m_fp.y() - m_lp.y();
	}

	if ( m_isCentered )
	{
		m_tl.setX( qRound( m_fp.x() - width*0.5 ) );
		m_tl.setY( qRound( m_fp.y() + height*0.5 ) );
		m_br.setX( qRound( m_fp.x() + width*0.5 ) );
		m_br.setY( qRound( m_fp.y() - height*0.5 ) );
		m_center = m_fp;
	}
	else
	{
		m_tl.setX( m_fp.x() );
		m_tl.setY( m_fp.y() );
		m_br.setX( m_fp.x() + width );
		m_br.setY( m_fp.y() - height );
		m_center.setX( m_fp.x() + qRound( width * 0.5 ) );
		m_center.setY( m_fp.y() - qRound( height * 0.5 ) );
	}
}

inline void
VCToolEllipse::drawTemporaryObject( KarbonView* view )
{
/*	QPixmap pixmap;
	QPainter painter(&pixmap);

	painter.setPen( Qt::black );

	painter.drawEllipse(
		m_tl.x(), m_tl.y(),
		m_br.x() - m_tl.x(), m_br.y() - m_tl.y() );

	painter.drawLine(
		m_center.x() - 2, m_center.y() - 2,
		m_center.x() + 2, m_center.y() + 2 );
	painter.drawLine(
		m_center.x() - 2, m_center.y() + 2,
		m_center.x() + 2, m_center.y() - 2 );

	bitBlt(
		view->canvasWidget()->viewport(), 0, 0,
		&pixmap, 0, 0, -1, -1,
		Qt::NotROP ); */

	QPainter painter( view->canvasWidget()->viewport() );
	painter.setPen( Qt::black );
	painter.setRasterOp( Qt::NotROP );

	painter.drawEllipse(
		m_tl.x(), m_tl.y(),
		m_br.x() - m_tl.x(), m_br.y() - m_tl.y() );

	// paint little center cross (lines must not cross due to ROP):
	if (
		( QABS( m_br.x() - m_tl.x() ) > 4 ) &&
		( QABS( m_tl.y() - m_br.y() ) > 4 ) )
	{
		painter.drawLine(
			m_center.x() - 2, m_center.y() - 2,
			m_center.x() + 2, m_center.y() + 2 );
		painter.drawLine(
			m_center.x() - 2, m_center.y() + 2,
			m_center.x() - 1, m_center.y() + 1 );
		painter.drawLine(
			m_center.x() + 1, m_center.y() - 1,
			m_center.x() + 2, m_center.y() - 2 );
	}
}

#endif
