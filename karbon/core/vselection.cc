/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <qpainter.h>
#include <qrect.h>

#include "vselection.h"

#include <kdebug.h>


VSelection::VSelection( VObject* parent, VState /*state*/ )
	: VGroup( parent )
{
	m_qrect = new QRect();
}

VSelection::VSelection( const VSelection& selection )
	: VGroup( selection )
{
	m_qrect = new QRect();
}

VSelection::~VSelection()
{
	delete( m_qrect );
}

void
VSelection::draw( QPainter* painter, double zoomFactor ) const
{
	if( objects().count() == 0 )
		return;

	// get bounding box:
	const KoRect& rect = boundingBox();

	// scale with zoomFactor to displaycoords:
	m_qrect->setCoords(
		qRound( rect.left() * zoomFactor ),
		qRound( rect.top() * zoomFactor ),
		qRound( rect.right() * zoomFactor ),
		qRound( rect.bottom() * zoomFactor ) );

	painter->setBrush( Qt::NoBrush );
	painter->setPen( Qt::blue.light() );

	// draw handle rect:
	painter->drawRect( *m_qrect );
}

