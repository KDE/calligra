/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VCTOOLELLIPSE_H__
#define __VCTOOLELLIPSE_H__

#include <qbitmap.h>
#include <qpainter.h>
#include <qpoint.h>

#include "karbon_view.h"
#include "vccmd_ellipse.h"
#include "vpath.h"
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
		m_tl.setX( m_fp.x() - m_signW * qRound( m_width  * 0.5 ) );
		m_tl.setY( m_fp.y() - m_signH * qRound( m_height * 0.5 ) );
		m_br.setX( m_fp.x() + m_signW * qRound( m_width  * 0.5 ) );
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
	
	VCCmdEllipse* cmd =
		new VCCmdEllipse( m_part, m_tl.x(), m_tl.y(), m_br.x(), m_br.y() );

	VPath* path = cmd->createPath();
	path->setState( VObject::edit );
	path->draw( painter, path->boundingBox() );

	delete( cmd );
	delete( path );
}

#endif
