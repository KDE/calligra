/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VCTOOLSTAR_H__
#define __VCTOOLSTAR_H__

#include <qpainter.h>
#include <qpoint.h>

#include "karbon_view.h"
#include "vccmd_star.h"
#include "vcdlg_star.h"
#include "vpath.h"
#include "vtool.h"

class KarbonPart;

// A singleton state to create a star

class VCToolStar : public VTool
{
public:
	virtual ~VCToolStar();
	static VCToolStar* instance( KarbonPart* part );

	virtual bool eventFilter( KarbonView* view, QEvent* event );

protected:
	VCToolStar( KarbonPart* part );

private:
	// inline helper functions:
	void recalcCoords();
	void drawTemporaryObject( KarbonView* view );

	static VCToolStar* s_instance;

	KarbonPart* m_part;
	VCDlgStar* m_dialog;

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
};

inline void
VCToolStar::recalcCoords()
{
	if ( m_isSquare )
	{
		m_width  = m_lp.x() - m_fp.x();
		m_height = m_fp.y() - m_lp.y();

		if ( QABS( m_width ) > QABS( m_height ) )
			m_height = ( m_height < 0 ? -1 : 1 ) * QABS( m_width );
		else
			m_width = ( m_width < 0 ? -1 : 1 ) * QABS( m_height );
	}
	else
	{
		m_width  = m_lp.x() - m_fp.x();
		m_height = m_fp.y() - m_lp.y();
	}

	if ( m_isCentered )
	{
		m_tl.setX( qRound( m_fp.x() - m_width*0.5 ) );
		m_tl.setY( qRound( m_fp.y() + m_height*0.5 ) );
		m_br.setX( qRound( m_fp.x() + m_width*0.5 ) );
		m_br.setY( qRound( m_fp.y() - m_height*0.5 ) );
	}
	else
	{
		m_tl.setX( m_fp.x() );
		m_tl.setY( m_fp.y() );
		m_br.setX( m_fp.x() + m_width );
		m_br.setY( m_fp.y() - m_height );
	}
}

inline void
VCToolStar::drawTemporaryObject( KarbonView* view )
{
	QPainter painter( view->canvasWidget()->viewport() );

	VCCmdStar* cmd =
		new VCCmdStar( m_part, m_tl.x(), m_tl.y(),
			qRound( m_width / 2 ),
			qRound( m_dialog->valueInnerR() *
				m_width / ( m_dialog->valueOuterR() * 2 ) ),
			m_dialog->valueEdges() );

	VPath* path = cmd->createPath();
	path->setState( VObject::edit );
	path->draw( painter, path->boundingBox() );

	delete( cmd );
	delete( path );
}

#endif
