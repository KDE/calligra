/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VCTOOLRECTANGLE_H__
#define __VCTOOLRECTANGLE_H__

#include <qpoint.h>

#include "karbon_view.h"
#include "vpoint.h"
#include "vtool.h"

class KarbonPart;
class VCDlgRectangle;

// A singleton state to create a rectangle.

class VCToolRectangle : public VTool
{
public:
	virtual ~VCToolRectangle();
	static VCToolRectangle* instance( KarbonPart* part );

	virtual bool eventFilter( KarbonView* view, QEvent* event );

protected:
	VCToolRectangle( KarbonPart* part );

private:
	// inline helper functions:
	void recalcCoords();
	void drawTemporaryObject( KarbonView* view );

	static VCToolRectangle* s_instance;

	KarbonPart* m_part;
	VCDlgRectangle* m_dialog;

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
VCToolRectangle::recalcCoords()
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
VCToolRectangle::drawTemporaryObject( KarbonView* view )
{
	QPainter painter( view->canvasWidget()->viewport() );
	painter.setPen( Qt::black );
	painter.setRasterOp( Qt::NotROP );

	// Qt's drawRect() behaves sometimes oddly. we have to paint the rect
	// this way:

	painter.moveTo( m_tl );
	painter.lineTo( m_br.x(), m_tl.y() );
	painter.lineTo( m_br );
	painter.lineTo( m_tl.x(), m_br.y() );
	painter.lineTo( m_tl );
}

#endif
