/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VCTOOLROUNDRECT_H__
#define __VCTOOLROUNDRECT_H__

#include <qpoint.h>
#include <qrect.h>

#include "karbon_view.h"
#include "vccmd_roundrect.h"
#include "vpath.h"
#include "vpoint.h"
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
	void recalcRect();
	void drawTemporaryRect( KarbonView* view );

	static VCToolRoundRect* s_instance;

	KarbonPart* m_part;
	VCDlgRoundRect* m_dialog;

	double m_round;

	bool m_isDragging;
	bool m_isSquare;
	bool m_isCentered;

	// temporary variables for the gui:
	QPoint m_fp;
	QPoint m_lp;
	QRect m_rect;
};

inline void
VCToolRoundRect::recalcRect()
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
		m_rect.setLeft( qRound( m_fp.x() - width*0.5 ) );
		m_rect.setTop( qRound( m_fp.y() + height*0.5 ) );
		m_rect.setRight( qRound( m_fp.x() + width*0.5 ) );
		m_rect.setBottom( qRound( m_fp.y() - height*0.5 ) );
	}
	else
	{
		m_rect.setLeft( m_fp.x() );
		m_rect.setTop( m_fp.y() );
		m_rect.setRight( m_fp.x() + width );
		m_rect.setBottom( m_fp.y() - height );
	}
}

inline void
VCToolRoundRect::drawTemporaryRect( KarbonView* view )
{
	QPainter painter( view->canvasWidget()->viewport() );
	painter.setPen( Qt::black );
	painter.setRasterOp( Qt::NotROP );

	// Qt's drawRoundRect() behaves differntely, that's why we have
	// to take the long way home here.

	VPoint tl;
	VPoint br;
	tl.setFromQPoint( m_rect.topLeft(), view->zoomFactor() );
	br.setFromQPoint( m_rect.bottomRight(), view->zoomFactor() );

	// let the command create the necessary qpointarray for us:
	VCCmdRoundRect* cmd =
		new VCCmdRoundRect( m_part, tl.x(), tl.y(), br.x(), br.y(), m_round );

	VPath* path = cmd->createPath();

	painter.drawPolygon( path->getQPointArray( view->zoomFactor() ) );

	delete( cmd );
	delete( path );
}

#endif
