/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VCTOOLRECTANGLE_H__
#define __VCTOOLRECTANGLE_H__

#include <qpoint.h>
#include <qrect.h>

#include "karbon_view.h"
#include "vtool.h"

class VCDlgRectangle;

class KarbonPart;

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
	void recalcRect();
	void drawTemporaryRect( KarbonView* view );

	static VCToolRectangle* s_instance;

	KarbonPart* m_part;
	VCDlgRectangle* m_dialog;

	bool m_isDragging;
	bool m_isSquare;
	bool m_isCentered;

	// temporary variables for the gui:
	QPoint m_fp;
	QPoint m_lp;
	QRect m_rect;
};

inline void
VCToolRectangle::recalcRect()
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
		m_rect.setRight( m_fp.x() + width );
		m_rect.setBottom( m_fp.y() - height );
	}
}

inline void
VCToolRectangle::drawTemporaryRect( KarbonView* view )
{
	QPainter painter( view->canvas()->viewport() );
	painter.setPen( Qt::black );
	painter.setRasterOp( Qt::NotROP );
	// erase old rect:
	painter.drawRect( m_rect );
}

#endif
