/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <qdom.h>

#include "vsegment.h"
#include "vsegmentlist.h"

VSegmentListTraverser::VSegmentListTraverser()
	:  m_previousPoint( 0.0, 0.0 )
{
}

bool
VSegmentListTraverser::traverse( const VSegmentList& list )
{
	VSegmentListIterator itr( list );
	for( ; itr.current(); ++itr )
	{
		if( itr.current()->type() == VSegment::curve )
		{
			if( !curveTo(
				itr.current()->point( 1 ),
				itr.current()->point( 2 ),
				itr.current()->point( 3 ) ) )
			{
				return false;
			}
		}
		else if( itr.current()->type() == VSegment::line )
		{
			if( !lineTo(
				itr.current()->point( 3 ) ) )
			{
				return false;
			}
		}
		else if( itr.current()->type() == VSegment::curve1 )
		{
			if( !curve1To(
				itr.current()->point( 2 ),
				itr.current()->point( 3 ) ) )
			{
				return false;
			}
		}
		else if( itr.current()->type() == VSegment::curve2 )
		{
			if( !curve2To(
				itr.current()->point( 1 ),
				itr.current()->point( 3 ) ) )
			{
				return false;
			}
		}
		else if( itr.current()->type() == VSegment::begin )
		{
			if( !begin(
				itr.current()->point( 3 ) ) )
			{
				return false;
			}
		}
		else if( itr.current()->type() == VSegment::end )
		{
			if( !end(
				itr.current()->point( 3 ) ) )
			{
				return false;
			}
		}
		// fall back:
		else
		{
			if( !lineTo(
				itr.current()->point( 3 ) ) )
			{
				return false;
			}
		}
	}

	return true;
}


VSegment::VSegment()
	: m_type( begin )
{
}

VSegment::VSegment( const VSegment& other )
{
	m_point[0] = other.m_point[0];
	m_point[1] = other.m_point[1];
	m_point[2] = other.m_point[2];
	m_type = other.m_type;
}

void
VSegment::save( QDomElement& element ) const
{
	QDomElement me;

	if( type() == curve )
	{
		me = element.ownerDocument().createElement( "CURVE" );
		me.setAttribute( "x1", m_point[0].x() );
		me.setAttribute( "y1", m_point[0].y() );
		me.setAttribute( "x2", m_point[1].x() );
		me.setAttribute( "y2", m_point[1].y() );
		me.setAttribute( "x3", m_point[2].x() );
		me.setAttribute( "y3", m_point[2].y() );
	}
	else if( type() == curve1 )
	{
		me = element.ownerDocument().createElement( "CURVE1" );
		me.setAttribute( "x2", m_point[1].x() );
		me.setAttribute( "y2", m_point[1].y() );
		me.setAttribute( "x3", m_point[2].x() );
		me.setAttribute( "y3", m_point[2].y() );
	}
	else if( type() == curve2 )
	{
		me = element.ownerDocument().createElement( "CURVE2" );
		me.setAttribute( "x1", m_point[0].x() );
		me.setAttribute( "y1", m_point[0].y() );
		me.setAttribute( "x3", m_point[2].x() );
		me.setAttribute( "y3", m_point[2].y() );
	}
	else if( type() == line )
	{
		me = element.ownerDocument().createElement( "LINE" );
		me.setAttribute( "x", m_point[2].x() );
		me.setAttribute( "y", m_point[2].y() );
	}
	else if( type() == begin )
	{
		me = element.ownerDocument().createElement( "MOVE" );
		me.setAttribute( "x", m_point[2].x() );
		me.setAttribute( "y", m_point[2].y() );
	}
	else
		return;

	element.appendChild( me );

}

void
VSegment::load( const QDomElement& element )
{
	if( element.tagName() == "CURVE" )
	{
		m_type = curve;
		m_point[0].setX( element.attribute( "x1" ).toDouble() );
		m_point[0].setY( element.attribute( "y1" ).toDouble() );
		m_point[1].setX( element.attribute( "x2" ).toDouble() );
		m_point[1].setY( element.attribute( "y2" ).toDouble() );
		m_point[2].setX( element.attribute( "x3" ).toDouble() );
		m_point[2].setY( element.attribute( "y3" ).toDouble() );
	}
	else if( element.tagName() == "CURVE1" )
	{
		m_type = curve1;
		m_point[1].setX( element.attribute( "x2" ).toDouble() );
		m_point[1].setY( element.attribute( "y2" ).toDouble() );
		m_point[2].setX( element.attribute( "x3" ).toDouble() );
		m_point[2].setY( element.attribute( "y3" ).toDouble() );
	}
	else if( element.tagName() == "CURVE2" )
	{
		m_type = curve2;
		m_point[0].setX( element.attribute( "x1" ).toDouble() );
		m_point[0].setY( element.attribute( "y1" ).toDouble() );
		m_point[2].setX( element.attribute( "x3" ).toDouble() );
		m_point[2].setY( element.attribute( "y3" ).toDouble() );
	}
	else if( element.tagName() == "LINE" )
	{
		m_type = line;
		m_point[2].setX( element.attribute( "x" ).toDouble() );
		m_point[2].setY( element.attribute( "y" ).toDouble() );
	}
	else if( element.tagName() == "MOVE" )
	{
		m_type = begin;
		m_point[2].setX( element.attribute( "x" ).toDouble() );
		m_point[2].setY( element.attribute( "y" ).toDouble() );
	}
}
