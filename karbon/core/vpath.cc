/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <qdom.h>
#include <qpainter.h>
#include <qwmatrix.h>

#include <koPoint.h>
#include <koRect.h>

#include "vfill.h"
#include "vpainter.h"
#include "vpath.h"
#include "vsegment.h"
#include "vstroke.h"
#include "vvisitor.h"

#include <kdebug.h>


VPath::VPath( VObject* parent, VState state )
	: VObject( parent, state )
{
	m_segmentLists.setAutoDelete( true );

	// add an initial segmentlist:
	m_segmentLists.append( new VSegmentList( this ) );

	// we need a stroke for boundingBox() at anytime:
	m_stroke = new VStroke( this );
	m_fill = new VFill();

	m_drawCenterNode = false;
}

VPath::VPath( const VPath& path )
	: VObject( path )
{
	m_segmentLists.setAutoDelete( true );

	VSegmentList* list;

	QPtrListIterator<VSegmentList> itr( path.m_segmentLists );
	for( itr.toFirst(); itr.current(); ++itr )
	{
		list = itr.current()->clone();
		list->setParent( this );
		m_segmentLists.append( list );
	}

	if ( path.stroke() )
		setStroke( *path.stroke() );

	if ( path.fill() )
		setFill( *path.fill() );

	m_drawCenterNode = false;
}

VPath::~VPath()
{
}

void
VPath::draw( VPainter *painter, const KoRect& rect ) const
{
	if(
		state() == deleted ||
		state() == hidden ||
		state() == hidden_locked )
	{
		return;
	}

	double zoomFactor = painter->zoomFactor();

	if( zoomFactor != 1 && !rect.intersects( boundingBox() ) )
		return;

	painter->save();
	//painter->setZoomFactor( zoomFactor );

	QPtrListIterator<VSegmentList> itr( m_segmentLists );

	if( state() != edit )
	{
		// paint fill:
		painter->newPath();
		for( itr.toFirst(); itr.current(); ++itr )
		{
			VSegmentListIterator jtr( *( itr.current() ) );
			for( ; jtr.current(); ++jtr )
			{
				if( jtr.current()->type() == VSegment::curve  )
					painter->curveTo(
						jtr.current()->ctrlPoint1(),
						jtr.current()->ctrlPoint2(),
						jtr.current()->knot() );
				else if( jtr.current()->type() == VSegment::line )
				{
					painter->lineTo( jtr.current()->knot() );
				}
				else
					painter->moveTo( jtr.current()->knot() );
			}
		}

		painter->setRasterOp( Qt::CopyROP );
		painter->setPen( Qt::NoPen );
		painter->setBrush( *fill() );
		painter->fillPath();

		// draw stroke:
		painter->setRasterOp( Qt::CopyROP );
		painter->setPen( *stroke() );
		painter->setBrush( Qt::NoBrush );
		painter->strokePath();
	}

	// draw simplistic contour:
	if( state() == edit )//|| state() == selected )
	{
		for( itr.toFirst(); itr.current(); ++itr )
		{
			painter->newPath();
			painter->setRasterOp( Qt::XorROP );
			painter->setPen( Qt::yellow );
			painter->setBrush( Qt::NoBrush );

			VSegmentListIterator jtr( *( itr.current() ) );
			for( ; jtr.current(); ++jtr )
			{
				if( jtr.current()->type() == VSegment::curve  )
					painter->curveTo(
						jtr.current()->ctrlPoint1(),
						jtr.current()->ctrlPoint2(),
						jtr.current()->knot() );
				else if( jtr.current()->type() == VSegment::line )
				{
					painter->lineTo( jtr.current()->knot() );
				}
				else
					painter->moveTo( jtr.current()->knot() );
			}

			painter->strokePath();
		}

		// draw a "knot" at the center:
		if( m_drawCenterNode )
		{
			const KoPoint center = boundingBox().center();

			painter->newPath();
			painter->setRasterOp( Qt::XorROP );
			painter->setPen( Qt::NoPen );
			painter->setBrush( Qt::yellow );

			painter->moveTo(
				KoPoint(
					center.x() - 2 / zoomFactor,
					center.y() - 2 / zoomFactor ) );
			painter->lineTo(
				KoPoint(
					center.x() + 2 / zoomFactor,
					center.y() - 2 / zoomFactor ) );
			painter->lineTo(
				KoPoint(
					center.x() + 2 / zoomFactor,
					center.y() + 2 / zoomFactor ) );
			painter->lineTo(
				KoPoint(
					center.x() - 2 / zoomFactor,
					center.y() + 2 / zoomFactor ) );
			painter->fillPath();
		}
	}

	// draw small boxes for path nodes:
	if( state() == selected || state() == edit )
	{
		for( itr.toFirst(); itr.current(); ++itr )
		{
			VSegmentListIterator jtr( *( itr.current() ) );
			for( ; jtr.current(); ++jtr )
			{
				painter->newPath();
				painter->setRasterOp( Qt::NotROP );
				painter->setPen( Qt::NoPen );

				if( jtr.current()->hasSelectedNodes() )
					painter->setBrush( Qt::blue.light() );
				else
					painter->setBrush( Qt::NoBrush );

				if( jtr.current()->type() == VSegment::curve )
				{
					if( jtr.current()->hasSelectedNodes() )
					{
						if( jtr.current()->ctrlPointFixing() == VSegment::none ||
							jtr.current()->ctrlPointFixing() == VSegment::second )
						{
							if( jtr.current()->prev()  )
							{
								painter->moveTo( KoPoint(
									jtr.current()->prev()->knot().x() - 3 / zoomFactor,
									jtr.current()->prev()->knot().y() - 3 / zoomFactor ) );
								painter->lineTo( KoPoint(
									jtr.current()->prev()->knot().x() + 3 / zoomFactor,
									jtr.current()->prev()->knot().y() - 3 / zoomFactor ) );
								painter->lineTo( KoPoint(
									jtr.current()->prev()->knot().x() + 3 / zoomFactor,
									jtr.current()->prev()->knot().y() + 3 / zoomFactor ) );
								painter->lineTo( KoPoint(
									jtr.current()->prev()->knot().x() - 3 / zoomFactor,
									jtr.current()->prev()->knot().y() + 3 / zoomFactor ) );
								painter->fillPath();
								painter->newPath();
								painter->setRasterOp( Qt::NotROP );
								painter->setBrush( Qt::blue.light() );
							}
							painter->moveTo( KoPoint(
								jtr.current()->ctrlPoint1().x() - 3 / zoomFactor,
								jtr.current()->ctrlPoint1().y() - 3 / zoomFactor ) );
							painter->lineTo( KoPoint(
								jtr.current()->ctrlPoint1().x() + 3 / zoomFactor,
								jtr.current()->ctrlPoint1().y() - 3 / zoomFactor ) );
							painter->lineTo( KoPoint(
								jtr.current()->ctrlPoint1().x() + 3 / zoomFactor,
								jtr.current()->ctrlPoint1().y() + 3 / zoomFactor ) );
							painter->lineTo( KoPoint(
								jtr.current()->ctrlPoint1().x() - 3 / zoomFactor,
								jtr.current()->ctrlPoint1().y() + 3 / zoomFactor ) );

							painter->fillPath();
						}

					if( jtr.current()->ctrlPointFixing() == VSegment::none ||
						jtr.current()->ctrlPointFixing() == VSegment::first )
					{
						painter->newPath();
						painter->setRasterOp( Qt::NotROP );
						painter->setBrush( Qt::blue.light() );

						painter->moveTo( KoPoint(
							jtr.current()->ctrlPoint2().x() - 3 / zoomFactor,
							jtr.current()->ctrlPoint2().y() - 3 / zoomFactor ) );
						painter->lineTo( KoPoint(
							jtr.current()->ctrlPoint2().x() + 3 / zoomFactor,
							jtr.current()->ctrlPoint2().y() - 3 / zoomFactor ) );
						painter->lineTo( KoPoint(
							jtr.current()->ctrlPoint2().x() + 3 / zoomFactor,
							jtr.current()->ctrlPoint2().y() + 3 / zoomFactor ) );
						painter->lineTo( KoPoint(
							jtr.current()->ctrlPoint2().x() - 3 / zoomFactor,
							jtr.current()->ctrlPoint2().y() + 3 / zoomFactor ) );

						painter->fillPath();
					}

					// draw knot always
					painter->newPath();
					painter->setRasterOp( Qt::NotROP );
					painter->setBrush( Qt::blue.light() );

					painter->moveTo( KoPoint(
							jtr.current()->knot().x() - 3 / zoomFactor,
							jtr.current()->knot().y() - 3 / zoomFactor ) );
					painter->lineTo( KoPoint(
							jtr.current()->knot().x() + 3 / zoomFactor,
							jtr.current()->knot().y() - 3 / zoomFactor ) );
					painter->lineTo( KoPoint(
							jtr.current()->knot().x() + 3 / zoomFactor,
							jtr.current()->knot().y() + 3 / zoomFactor ) );
					painter->lineTo( KoPoint(
							jtr.current()->knot().x() - 3 / zoomFactor,
							jtr.current()->knot().y() + 3 / zoomFactor ) );
					painter->fillPath();


					VStroke stroke;
					stroke.setLineWidth( 1.0 / zoomFactor );
					stroke.setColor( Qt::blue.light().rgb() );
					painter->setPen( stroke );

					// line between ctrl points
					painter->newPath();
					if( jtr.current()->ctrlPointFixing() == VSegment::none )
					{
						if( jtr.current()->prev()  )
						{
							// prev knot to 1st ctrl point
							painter->moveTo( KoPoint(
									jtr.current()->prev()->knot().x(),
									jtr.current()->prev()->knot().y() ) );
							painter->lineTo( KoPoint(
									jtr.current()->ctrlPoint1().x(),
									jtr.current()->ctrlPoint1().y() ) );
							painter->strokePath();
							VStroke stroke;
							stroke.setLineWidth( 1.0 / zoomFactor );
							stroke.setColor( Qt::blue.light().rgb() );
							painter->setPen( stroke );
							painter->newPath();
						}
						painter->moveTo( KoPoint(
								jtr.current()->ctrlPoint2().x(),
								jtr.current()->ctrlPoint2().y() ) );
						painter->lineTo( KoPoint(
								jtr.current()->knot().x(),
								jtr.current()->knot().y() ) );
					}
					else if( jtr.current()->ctrlPointFixing() == VSegment::first )
					{
						painter->moveTo( KoPoint(
								jtr.current()->knot().x(),
								jtr.current()->knot().y() ) );
						painter->lineTo( KoPoint(
								jtr.current()->ctrlPoint2().x(),
								jtr.current()->ctrlPoint2().y() ) );
					}
					else
					{
						painter->moveTo( KoPoint(
								jtr.current()->knot().x(),
								jtr.current()->knot().y() ) );
						painter->lineTo( KoPoint(
								jtr.current()->ctrlPoint1().x(),
								jtr.current()->ctrlPoint1().y() ) );
					}
					painter->strokePath();
					}
				}
				else
				{
					painter->moveTo(
						KoPoint(
							jtr.current()->knot().x() - 2 / zoomFactor,
							jtr.current()->knot().y() - 2 / zoomFactor ) );
					painter->lineTo(
						KoPoint(
							jtr.current()->knot().x() + 2 / zoomFactor,
							jtr.current()->knot().y() - 2 / zoomFactor ) );
					painter->lineTo(
						KoPoint(
							jtr.current()->knot().x() + 2 / zoomFactor,
							jtr.current()->knot().y() + 2 / zoomFactor ) );
					painter->lineTo(
						KoPoint(
							jtr.current()->knot().x() - 2 / zoomFactor,
							jtr.current()->knot().y() + 2 / zoomFactor ) );

					painter->fillPath();
				}
			}
		}

		// draw a "knot" at the center:
		if( m_drawCenterNode )
		{
			const KoPoint center = boundingBox().center();

			painter->newPath();
			painter->setRasterOp( Qt::NotROP );
			painter->setPen( Qt::NoPen );
			painter->setBrush( Qt::blue.light() );

			painter->moveTo(
				KoPoint(
					center.x() - 2 / zoomFactor,
					center.y() - 2 / zoomFactor ) );
			painter->lineTo(
				KoPoint(
					center.x() + 2 / zoomFactor,
					center.y() - 2 / zoomFactor ) );
			painter->lineTo(
				KoPoint(
					center.x() + 2 / zoomFactor,
					center.y() + 2 / zoomFactor ) );
			painter->lineTo(
				KoPoint(
					center.x() - 2 / zoomFactor,
					center.y() + 2 / zoomFactor ) );
			painter->fillPath();
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
		VSegmentList* list = new VSegmentList( this );
		list->moveTo( p );
		m_segmentLists.append( list );
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
	VSegmentList* list = segmentList.clone();
	list->setParent( this );
	m_segmentLists.append( list );
}

void
VPath::transform( const QWMatrix& m )
{
	QPtrListIterator<VSegmentList> itr( m_segmentLists );
	for( itr.toFirst(); itr.current(); ++itr )
	{
		itr.current()->transform( m );
	}

	if( m_stroke->type() == VStroke::grad )
		m_stroke->gradient().transform( m );
	if( m_fill->type() == VFill::grad )
		m_fill->gradient().transform( m );
}

const KoRect&
VPath::boundingBox() const
{
	if( m_boundingBoxIsInvalid )
	{
		// clear:
		m_boundingBox = KoRect();

		QPtrListIterator<VSegmentList> itr( m_segmentLists );
		for( itr.toFirst(); itr.current(); ++itr )
		{
			m_boundingBox |= itr.current()->boundingBox();
		}

		// take line width into account:
		m_boundingBox.setCoords(
			m_boundingBox.left()   - 0.5 * stroke()->lineWidth(),
			m_boundingBox.top()    - 0.5 * stroke()->lineWidth(),
			m_boundingBox.right()  + 0.5 * stroke()->lineWidth(),
			m_boundingBox.bottom() + 0.5 * stroke()->lineWidth() );

		m_boundingBoxIsInvalid = false;
	}

	return m_boundingBox;
}

VPath*
VPath::clone() const
{
	return new VPath( *this );
}

void
VPath::save( QDomElement& element ) const
{
	if( state() != deleted )
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
	setState( normal );

	QDomNodeList list = element.childNodes();
	for( uint i = 0; i < list.count(); ++i )
	{
		if( list.item( i ).isElement() )
		{
			QDomElement child = list.item( i ).toElement();

			if( child.tagName() == "SEGMENTS" )
			{
				VSegmentList sl( this );
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

void
VPath::accept( VVisitor& visitor )
{
	visitor.visitVPath( *this );
}

