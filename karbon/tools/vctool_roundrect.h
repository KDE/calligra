/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VCTOOLROUNDRECT_H__
#define __VCTOOLROUNDRECT_H__

#include <qpainter.h>
#include <qpoint.h>

#include "karbon_view.h"
#include "vccmd_roundrect.h"
#include "vpath.h"
#include "vtool.h"

class KarbonPart;
class VCDlgRoundRect;

// A singleton state to create a rectangle.

class VCToolRoundRect : public VTool
{
public:
	virtual ~VCToolRoundRect();
	static VCToolRoundRect* instance( KarbonPart* part );

	virtual bool eventFilter( KarbonView* view, QEvent* event );

protected:
	VCToolRoundRect( KarbonPart* part );

private:
	// inline helper functions:
	void recalcCoords();
	void drawTemporaryObject( KarbonView* view );

	static VCToolRoundRect* s_instance;

	KarbonPart* m_part;
	VCDlgRoundRect* m_dialog;

	double m_round;

	bool m_isDragging;
	bool m_isSquare;
	bool m_isCentered;

	// mouse coordinates::
	QPoint m_fp;
	QPoint m_lp;
	// painting coordinates:
	QPoint m_tl;
	QPoint m_br;
};

inline void
VCToolRoundRect::recalcCoords()
{
	int width;
	int height;

	if ( m_isSquare )
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
	}
	else
	{
		m_tl.setX( m_fp.x() );
		m_tl.setY( m_fp.y() );
		m_br.setX( m_fp.x() + width );
		m_br.setY( m_fp.y() - height );
	}
}

inline void
VCToolRoundRect::drawTemporaryObject( KarbonView* view )
{
	QPainter painter( view->canvasWidget()->viewport() );
	painter.setPen( Qt::black );
	painter.setRasterOp( Qt::NotROP );

	// Qt's drawRoundRect() behaves differently, that's why we have
	// to take the long way home here.

	// let the command create the necessary qpointarray for us:
	VCCmdRoundRect* cmd =
		new VCCmdRoundRect( m_part, m_tl.x(), m_tl.y(), m_br.x(), m_br.y(), m_round );

	VPath* path = cmd->createPath();

	painter.drawPolygon( path->getQPointArray( view->zoomFactor() ) );

	delete( cmd );
	delete( path );
}

#endif
