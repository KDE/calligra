/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <qdom.h>

#include "vstroke.h"

VStroke::VStroke( const double width, const VLineCap cap, const VLineJoin join,
			const double miterLimit )
	: m_lineWidth( width ), m_lineCap( cap ), m_lineJoin( join ),
		m_miterLimit( miterLimit )
{
}

void
VStroke::save( QDomElement& element ) const
{
	QDomElement me = element.ownerDocument().createElement( "STROKE" );
	element.appendChild( me );


}

void
VStroke::load( const QDomElement& element )
{

}

