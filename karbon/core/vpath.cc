/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <qdom.h>
#include <qpainter.h>
#include <qwmatrix.h>

#include <koPoint.h>
#include <koRect.h>

#include "vpainter.h"
#include "vpath.h"
#include "vpath_bounding.h"

#include <kdebug.h>


VPath::VPath()
	: VObject()
{
	m_segmentLists.setAutoDelete( true );

	// add an initial segmentlist:
	m_segmentLists.append( new VSegmentList() );
}

VPath::VPath( const VPath& path )
	: VObject( path )
{
	m_segmentLists.setAutoDelete( true );

	QPtrListIterator<VSegmentList> itr( path.m_segmentLists );
	for( itr.toFirst(); itr.current(); ++itr )
	{
		m_segmentLists.append( new VSegmentList( *( itr.current() ) ) );
	}
}

VPath::~VPath()
{
}

void
VPath::draw( VPainter *painter, const QRect& rect,
	const double zoomFactor )
{
	if( state() == state_deleted )
		return;

	if( !rect.intersects( boundingBox( zoomFactor ) ) )
		return;

	painter->save();
	painter->setZoomFactor( zoomFactor );

	QPtrListIterator<VSegmentList> itr( m_segmentLists );

	if( state() != state_edit )
	{
		// paint fill:
		for( itr.toFirst(); itr.current(); ++itr )
		{
			VSegmentListIterator jtr( *( itr.current() ) );
			for( ; jtr.current(); ++jtr )
			{
				if( jtr.current()->type() == segment_curve  )
					painter->curveTo(
						jtr.current()->ctrlPoint1(),
						jtr.current()->ctrlPoint2(),
						jtr.current()->knot2() );
				else if( jtr.current()->type() == segment_curve1  )
					painter->curveTo(
						jtr.current()->knot1(),
						jtr.current()->ctrlPoint2(),
						jtr.current()->knot2() );
				else if( jtr.current()->type() == segment_curve2  )
					painter->curveTo(
						jtr.current()->ctrlPoint1(),
						jtr.current()->knot2(),
						jtr.current()->knot2() );
				else if(
					jtr.current()->type() == segment_line ||
					jtr.current()->type() == segment_end  )
				{
					painter->lineTo( jtr.current()->knot2() );
				}
				else
					painter->moveTo( jtr.current()->knot2() );
			}
		}

		painter->setRasterOp( Qt::CopyROP );
		painter->setPen( Qt::NoPen );
		painter->setBrush( fill() );
		painter->fillPath();

		// draw stroke:
		for( itr.toFirst(); itr.current(); ++itr )
		{
			VSegmentListIterator jtr( *( itr.current() ) );
			for( ; jtr.current(); ++jtr )
			{
				if( jtr.current()->type() == segment_curve  )
					painter->curveTo(
						jtr.current()->ctrlPoint1(),
						jtr.current()->ctrlPoint2(),
						jtr.current()->knot2() );
				else if( jtr.current()->type() == segment_curve1  )
					painter->curveTo(
						jtr.current()->knot1(),
						jtr.current()->ctrlPoint2(),
						jtr.current()->knot2() );
				else if( jtr.current()->type() == segment_curve2  )
					painter->curveTo(
						jtr.current()->ctrlPoint1(),
						jtr.current()->knot2(),
						jtr.current()->knot2() );
				else if(
					jtr.current()->type() == segment_line ||
					jtr.current()->type() == segment_end  )
				{
					painter->lineTo( jtr.current()->knot2() );
				}
				else
					painter->moveTo( jtr.current()->knot2() );
			}
		}

		painter->setRasterOp( Qt::CopyROP );
		painter->setPen( stroke() );
		painter->setBrush( Qt::NoBrush );
		painter->strokePath();
	}

	// draw simplistic contour:
	if( state() == state_edit || state() == state_selected )
	{
		for( itr.toFirst(); itr.current(); ++itr )
		{
			VSegmentListIterator jtr( *( itr.current() ) );
			for( ; jtr.current(); ++jtr )
			{
				if( jtr.current()->type() == segment_curve  )
					painter->curveTo(
						jtr.current()->ctrlPoint1(),
						jtr.current()->ctrlPoint2(),
						jtr.current()->knot2() );
				else if( jtr.current()->type() == segment_curve1  )
					painter->curveTo(
						jtr.current()->knot1(),
						jtr.current()->ctrlPoint2(),
						jtr.current()->knot2() );
				else if( jtr.current()->type() == segment_curve2  )
					painter->curveTo(
						jtr.current()->ctrlPoint1(),
						jtr.current()->knot2(),
						jtr.current()->knot2() );
				else if(
					jtr.current()->type() == segment_line ||
					jtr.current()->type() == segment_end  )
				{
					painter->lineTo( jtr.current()->knot2() );
				}
				else
					painter->moveTo( jtr.current()->knot2() );
			}
		}

		painter->setRasterOp( Qt::XorROP );
		painter->setPen( Qt::yellow );
		painter->setBrush( Qt::NoBrush );
		painter->strokePath();
	}

	// draw small boxes for path nodes:
	if( state() == state_selected )
	{
		QPainter qpainter( painter->device() );
		qpainter.setRasterOp( Qt::CopyROP );
		qpainter.setPen( Qt::NoPen );
		qpainter.setBrush( Qt::blue.light() );

		for( itr.toFirst(); itr.current(); ++itr )
		{
			VSegmentListIterator jtr( *( itr.current() ) );
			for( ; jtr.current(); ++jtr )
			{
				qpainter.drawRect(
					qRound( zoomFactor * jtr.current()->knot2().x() ) - 2,
					qRound( zoomFactor * jtr.current()->knot2().y() ) - 2, 5, 5 );
			}
		}
	}

	painter->restore();
}

const KoPoint&
VPath::currentPoint() const
{
	return m_segmentLists.getLast()->currentPoint();
}

bool
VPath::moveTo( const KoPoint& p )
{
	if( isClosed() ) return false;

	// move "begin" when path is still empty:
	if( m_segmentLists.getLast()->moveTo( p ) )
	{
		return true;
	}
	// otherwise create a new subpath:
	else
	{
		// add an initial segmentlist:
		VSegmentList* list = new VSegmentList();
		m_segmentLists.append( list );
		m_segmentLists.getLast()->moveTo( p );
	}

	return false;
}

bool
VPath::lineTo( const KoPoint& p )
{
	return m_segmentLists.getLast()->lineTo( p );
}

bool
VPath::curveTo(
	const KoPoint& p1, const KoPoint& p2, const KoPoint& p3 )
{
	return m_segmentLists.getLast()->curveTo( p1, p2, p3 );
}

bool
VPath::curve1To( const KoPoint& p2, const KoPoint& p3 )
{
	return m_segmentLists.getLast()->curve1To( p2, p3 );
}

bool
VPath::curve2To( const KoPoint& p1, const KoPoint& p3 )
{
	return m_segmentLists.getLast()->curve2To( p1, p3 );
}

bool
VPath::arcTo( const KoPoint& p1, const KoPoint& p2, const double r )
{
	return m_segmentLists.getLast()->arcTo( p1, p2, r );
}

void
VPath::close()
{
	m_segmentLists.getLast()->close();
}

bool
VPath::isClosed() const
{
	return m_segmentLists.getLast()->isClosed();
}

VPath*
VPath::booleanOp( const VPath* /*path*/, int /*type*/ ) const
{
	return 0L;
}

void
VPath::combine( const VPath& path )
{
	QPtrListIterator<VSegmentList> itr( path.m_segmentLists );
	for( ; itr.current(); ++itr )
	{
		combineSegmentList( *( itr.current() ) );
	}
}

void
VPath::combineSegmentList( const VSegmentList& segmentList )
{
	m_segmentLists.append( new VSegmentList( segmentList ) );
}

void
VPath::transform( const QWMatrix& m )
{
	QPtrListIterator<VSegmentList> itr( m_segmentLists );
	for( itr.toFirst(); itr.current(); ++itr )
	{
		itr.current()->transform( m );
	}
}

QRect
VPath::boundingBox( const double zoomFactor ) const
{
	QRect rect;
	VPathBounding bb;

	QPtrListIterator<VSegmentList> itr( m_segmentLists );
	for( itr.toFirst(); itr.current(); ++itr )
	{
		bb.calculate( rect, zoomFactor, *( itr.current() ) );
	}
	return rect;
}

bool
VPath::intersects( const QRect& rect, const double zoomFactor ) const
{
	VPathBounding bb;
	QPtrListIterator<VSegmentList> itr( m_segmentLists );
	for( itr.toFirst(); itr.current(); ++itr )
	{
		if( bb.intersects( rect, zoomFactor, *( itr.current() ) ) )
			return true;
	}

	return false;
}

VObject*
VPath::clone()
{
	return new VPath( *this );
}

void
VPath::save( QDomElement& element ) const
{
	if( state() != state_deleted )
	{
		QDomElement me = element.ownerDocument().createElement( "PATH" );
		element.appendChild( me );

		VObject::save( me );

		// save segmentlists:
		QPtrListIterator<VSegmentList> itr( m_segmentLists );
		for( itr.toFirst(); itr.current(); ++itr )
		{
			itr.current()->save( me );
		}
	}
}

void
VPath::load( const QDomElement& element )
{
	m_segmentLists.clear();
	setState( state_normal );

	QDomNodeList list = element.childNodes();
	for( uint i = 0; i < list.count(); ++i )
	{
		if( list.item( i ).isElement() )
		{
			QDomElement child = list.item( i ).toElement();

			if( child.tagName() == "SEGMENTS" )
			{
				VSegmentList sl;
				sl.load( child );
				combineSegmentList( sl );
			}
			else
			{
				VObject::load( child );
			}
		}
	}
}
