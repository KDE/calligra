/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include <qdom.h>

#include "vtext.h"

#include <kdebug.h>


VText::VText()
	: VObject()
{
}

VText::VText( const VText& text )
	: VObject(), m_text( text.m_text )
{
}

VText::~VText()
{
}

void
VText::draw( VPainter* /*painter*/, const QRect& rect,
	const double zoomFactor )
{
	if( state() == state_deleted )
		return;

	if( !rect.intersects( boundingBox( zoomFactor ) ) )
		return;

}

VObject&
VText::transform( const QWMatrix& /*m*/ )
{
	return *this;
}

QRect
VText::boundingBox( const double /*zoomFactor*/ ) const
{
	QRect rect;
	return rect;
}

bool
VText::intersects( const QRect& /*rect*/, const double /*zoomFactor*/ ) const
{
/*	VTextBounding bb;
	QPtrListIterator<VSegmentList> itr( m_segments );
	for( itr.toFirst(); itr.current(); ++itr )
	{
		if( bb.intersects( rect, zoomFactor, *( itr.current() ) ) )
			return true;
	}
*/
	return false;
}

VObject*
VText::clone()
{
	return new VText( *this );
}

void
VText::save( QDomElement& element ) const
{
	if( state() != state_deleted )
	{
		QDomElement me = element.ownerDocument().createElement( "TEXT" );
		element.appendChild( me );

	}
}

void
VText::load( const QDomElement& /*element*/ )
{
}

