/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VCTOOLSELECT_H__
#define __VCTOOLSELECT_H__

#include <qpainter.h>
#include <qpoint.h>

#include "karbon_view.h"
#include "vpath.h"
#include "vtool.h"

class KarbonPart;
//class VCDlgSelect;

// A singleton state to select object(s)

class VCToolSelect : public VTool
{
public:
	virtual ~VCToolSelect();
	static VCToolSelect* instance( KarbonPart* part );

	virtual bool eventFilter( KarbonView* view, QEvent* event );

protected:
	VCToolSelect( KarbonPart* part );

private:
	// inline helper functions:
	void recalcCoords();
	void drawTemporaryObject( KarbonView* view );

	static VCToolSelect* s_instance;

	KarbonPart* m_part;
	//VCDlgSelect* m_dialog;

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
VCToolSelect::recalcCoords()
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
		m_tl.setX( qRound( m_fp.x() - width ) );
		m_tl.setY( qRound( m_fp.y() + height ) );
		m_br.setX( qRound( m_fp.x() + width ) );
		m_br.setY( qRound( m_fp.y() - height ) );
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
VCToolSelect::drawTemporaryObject( KarbonView* view )
{
	QPainter painter( view->canvasWidget()->viewport() );

	// Make a simple selection rectangle.
	// Maybe a simple drawRect would be quicker though.
    VPath* path = new VPath();

    path->moveTo( m_tl.x(), m_tl.y() );
    path->lineTo( m_br.x(), m_tl.y() );
    path->lineTo( m_br.x(), m_br.y() );
    path->lineTo( m_tl.x(), m_br.y() );
    path->close();

	path->setState( VObject::edit );
	path->draw( painter, path->boundingBox() );

	delete( path );
}

#endif
