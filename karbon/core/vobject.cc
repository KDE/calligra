/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include <qdom.h>

#include "vfill.h"
#include "vobject.h"
#include "vstroke.h"


VObject::VObject( VObject* parent, VState state )
{
	m_stroke = 0L;
	m_fill = 0L;
	
	m_parent = parent;
	m_state = state;

	invalidateBoundingBox();
}

VObject::VObject( const VObject& obj )
{
	m_stroke = new VStroke( *obj.m_stroke );
	m_fill = new VFill( *obj.m_fill );
	
	m_parent = obj.m_parent;
	m_state = obj.m_state;

	invalidateBoundingBox();
}

VObject::~VObject()
{
	delete( m_stroke );
	delete( m_fill );
}

void
VObject::setStroke( const VStroke& stroke )
{
	if( !m_stroke )
		m_stroke = new VStroke();

	*m_stroke = stroke;
}

void
VObject::setFill( const VFill& fill )
{
	if( !m_fill )
		m_fill = new VFill();

	*m_fill = fill;
}

void
VObject::save( QDomElement& element ) const
{
	if( m_stroke )
		m_stroke->save( element );

	if( m_fill )
		m_fill->save( element );
}

void
VObject::load( const QDomElement& element )
{
	if( !m_stroke )
		m_stroke = new VStroke();

	if( !m_fill )
		m_fill = new VFill();


	if( element.tagName() == "STROKE" )
	{
		m_stroke->load( element );
	}
	else if( element.tagName() == "FILL" )
	{
		m_fill->load( element );
	}
}

