/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VCTOOLSINUS_H__
#define __VCTOOLSINUS_H__

#include <qpoint.h>

#include "karbon_view.h"
#include "vccmd_sinus.h"
#include "vpath.h"
#include "vpoint.h"
#include "vtool.h"

class KarbonPart;
class VCDlgSinus;

// A singleton state to create a sinus

class VCToolSinus : public VTool
{
public:
	virtual ~VCToolSinus();
	static VCToolSinus* instance( KarbonPart* part );

	virtual bool eventFilter( KarbonView* view, QEvent* event );

protected:
	VCToolSinus( KarbonPart* part );

private:
	// inline helper functions:
	void recalcCoords();
	void drawTemporaryObject( KarbonView* view );

	static VCToolSinus* s_instance;

	KarbonPart* m_part;
	VCDlgSinus* m_dialog;

	uint m_periods;

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
VCToolSinus::recalcCoords()
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
VCToolSinus::drawTemporaryObject( KarbonView* view )
{
	QPainter painter( view->canvasWidget()->viewport() );
	painter.setPen( Qt::black );
	painter.setRasterOp( Qt::NotROP );

	// Qt's drawRoundRect() behaves differntely, that's why we have
	// to take the long way home here.

	VPoint tl;
	VPoint br;
	tl.setFromQPoint( m_tl, view->zoomFactor() );
	br.setFromQPoint( m_br, view->zoomFactor() );

	// let the command create the necessary qpointarray for us:
	VCCmdSinus* cmd =
		new VCCmdSinus( m_part, tl.x(), tl.y(), br.x(), br.y(), m_periods );

	VPath* path = cmd->createPath();

	painter.drawPolyline( path->getQPointArray( view->zoomFactor() ) );

	delete( cmd );
	delete( path );
}

#endif
