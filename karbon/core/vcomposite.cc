/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qdom.h>
#include <qpainter.h>
#include <qwmatrix.h>

#include <koPoint.h>
#include <koRect.h>

#include "vcomposite.h"
#include "vfill.h"
#include "vpainter.h"
#include "vsegment.h"
#include "vstroke.h"
#include "vvisitor.h"

#include <kdebug.h>


VComposite::VComposite( VObject* parent, VState state )
	: VObject( parent, state )
{
	m_paths.setAutoDelete( true );

	// add an initial path:
	m_paths.append( new VPath( this ) );

	// we need a stroke for boundingBox() at anytime:
	m_stroke = new VStroke( this );
	m_fill = new VFill();

	m_drawCenterNode = false;
}

VComposite::VComposite( const VComposite& composite )
	: VObject( composite )
{
	m_paths.setAutoDelete( true );

	VPath* path;

	VPathListIterator itr( composite.m_paths );
	for( itr.toFirst(); itr.current(); ++itr )
	{
		path = itr.current()->clone();
		path->setParent( this );
		m_paths.append( path );
	}

	if ( composite.stroke() )
		setStroke( *composite.stroke() );

	if ( composite.fill() )
		setFill( *composite.fill() );

	m_drawCenterNode = false;
}

VComposite::~VComposite()
{
}

//TODO: better move this into vpainter?
static void
drawNode( VPainter* painter, const KoPoint& p, int width, double zoomFactor )
{
	painter->moveTo(
		KoPoint(
			p.x() - width / zoomFactor,
			p.y() - width / zoomFactor ) );
	painter->lineTo(
		KoPoint(
			p.x() + width / zoomFactor,
			p.y() - width / zoomFactor ) );
	painter->lineTo(
		KoPoint(
			p.x() + width / zoomFactor,
			p.y() + width / zoomFactor ) );
	painter->lineTo(
		KoPoint(
			p.x() - width / zoomFactor,
			p.y() + width / zoomFactor ) );
	painter->lineTo(
		KoPoint(
			p.x() - width / zoomFactor,
			p.y() - width / zoomFactor ) );
}

void
VComposite::draw( VPainter* painter, const KoRect* rect ) const
{
	if(
		state() == deleted ||
		state() == hidden ||
		state() == hidden_locked )
	{
		return;
	}

	double zoomFactor = painter->zoomFactor();

	if( rect && !rect->intersects( boundingBox() ) )
		return;

	painter->save();
	//painter->setZoomFactor( zoomFactor );

	VPathListIterator itr( m_paths );

	if( state() != edit )
	{
		// paint fill:
		painter->newPath();

		for( itr.toFirst(); itr.current(); ++itr )
		{
			VPathIterator jtr( *( itr.current() ) );
			for( ; jtr.current(); ++jtr )
			{
				jtr.current()->draw( painter );
			}
		}

		//kdDebug() << "bbox.x : " << boundingBox().x() << endl;
		//kdDebug() << "bbox.y : " << boundingBox().y() << endl;
		//kdDebug() << "bbox.width : " << boundingBox().width() << endl;
		//kdDebug() << "bbox.height : " << boundingBox().height() << endl;

		painter->setRasterOp( Qt::CopyROP );
		painter->setPen( Qt::NoPen );
		painter->setBrush( *fill() );
		painter->fillPath();

		// draw stroke:
		painter->setRasterOp( Qt::CopyROP );
		painter->setPen( *stroke() );
		painter->setBrush( Qt::NoBrush );
		painter->strokePath();

		//m_boundingBox = painter->boundingBox();

		//kdDebug() << "bbox.x : " << m_boundingBox.x() << endl;
		//kdDebug() << "bbox.y : " << m_boundingBox.y() << endl;
		//kdDebug() << "bbox.width : " << m_boundingBox.width() << endl;
		//kdDebug() << "bbox.height : " << m_boundingBox.height() << endl;
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

			VPathIterator jtr( *( itr.current() ) );
			for( ; jtr.current(); ++jtr )
			{
				jtr.current()->draw( painter );
			}

			painter->strokePath();
		}

		// Draw center node:
		if( m_drawCenterNode )
		{
			const KoPoint center = boundingBox().center();

			painter->newPath();
			painter->setRasterOp( Qt::XorROP );
			painter->setPen( Qt::NoPen );
			painter->setBrush( Qt::yellow );

			drawNode( painter, center, 2, zoomFactor );

			painter->fillPath();
		}
	}

	// Draw nodes and control lines:
	if( state() == selected )
	{
		itr.toFirst();
		//++itr;		// Skip "begin".

		for( ; itr.current(); ++itr )
		{
			VPathIterator jtr( *( itr.current() ) );
			++jtr;
			for( ; jtr.current(); ++jtr )
			{
				painter->newPath();
				painter->setRasterOp( Qt::NotROP );

				VColor color;
				color.set( 0.5, 0.5, 1.0 );
				VStroke stroke( color );
				stroke.setLineWidth( 1.0 );
				painter->setPen( stroke );
				painter->setBrush( Qt::NoBrush );

				if( jtr.current()->type() == VSegment::curve )
				{
					painter->newPath();

					// Draw control lines:
					if(
						jtr.current()->prev() &&
						( jtr.current()->ctrlPoint1Selected() ||
						  jtr.current()->prev()->knotSelected() ) )
					{
						painter->moveTo(
							jtr.current()->prev()->knot() );
						painter->lineTo(
							jtr.current()->ctrlPoint1() );

						painter->strokePath();
					}

					if(
						jtr.current()->ctrlPoint2Selected() ||
						jtr.current()->knotSelected() )
					{
						painter->moveTo(
							jtr.current()->ctrlPoint2() );
						painter->lineTo(
							jtr.current()->knot() );

						painter->strokePath();
					}

					// Draw control node1:
					painter->newPath();

					if(
						jtr.current()->prev() &&
						( jtr.current()->ctrlPoint1Selected() ||
						  jtr.current()->prev()->knotSelected() ) )
					{
						drawNode( painter, jtr.current()->ctrlPoint1(), 3, zoomFactor );
						painter->setBrush( Qt::blue.light() );
						painter->fillPath();
					}
					else
						painter->setBrush( Qt::NoBrush );

					painter->strokePath();


					// Draw control node2:
					painter->newPath();

					if(
						jtr.current()->ctrlPoint2Selected() ||
						jtr.current()->knotSelected() )
					{
						drawNode( painter, jtr.current()->ctrlPoint2(), 3, zoomFactor );

						painter->setBrush( Qt::blue.light() );
						painter->fillPath();
					}
					else
						painter->setBrush( Qt::NoBrush );

					painter->strokePath();
				}

				// Draw knot:
				painter->newPath();

				drawNode( painter, jtr.current()->knot(), 3, zoomFactor );

				if( jtr.current()->knotSelected() )
				{
					painter->setBrush( Qt::blue.light() );
					painter->fillPath();
				}
				else
					painter->setBrush( Qt::NoBrush );

				painter->strokePath();
			}
		}

		// Draw a center node:
		if( m_drawCenterNode )
		{
			painter->newPath();
			painter->setRasterOp( Qt::NotROP );
			painter->setPen( Qt::NoPen );
			painter->setBrush( Qt::blue.light() );

			drawNode( painter, boundingBox().center(), 3, zoomFactor );

			painter->fillPath();
		}
	}

	painter->restore();
}

const KoPoint&
VComposite::currentPoint() const
{
	return m_paths.getLast()->currentPoint();
}

bool
VComposite::moveTo( const KoPoint& p )
{
	if( isClosed() ) return false;

	// move "begin" when path is still empty:
	if( m_paths.getLast()->moveTo( p ) )
	{
		return true;
	}
	// otherwise create a new subpath:
	else
	{
		// add an initial path:
		VPath* path = new VPath( this );
		path->moveTo( p );
		m_paths.append( path );
	}

	return false;
}

bool
VComposite::lineTo( const KoPoint& p )
{
	return m_paths.getLast()->lineTo( p );
}

bool
VComposite::curveTo(
	const KoPoint& p1, const KoPoint& p2, const KoPoint& p3 )
{
	return m_paths.getLast()->curveTo( p1, p2, p3 );
}

bool
VComposite::curve1To( const KoPoint& p2, const KoPoint& p3 )
{
	return m_paths.getLast()->curve1To( p2, p3 );
}

bool
VComposite::curve2To( const KoPoint& p1, const KoPoint& p3 )
{
	return m_paths.getLast()->curve2To( p1, p3 );
}

bool
VComposite::arcTo( const KoPoint& p1, const KoPoint& p2, const double r )
{
	return m_paths.getLast()->arcTo( p1, p2, r );
}

void
VComposite::close()
{
	m_paths.getLast()->close();
}

bool
VComposite::isClosed() const
{
	return m_paths.getLast()->isClosed();
}

void
VComposite::combine( const VComposite& composite )
{
	VPathListIterator itr( composite.m_paths );
	for( ; itr.current(); ++itr )
	{
		combinePath( *( itr.current() ) );
	}
}

void
VComposite::combinePath( const VPath& path )
{
	VPath* p = path.clone();
	p->setParent( this );

// TODO: do complex inside tests instead:
	// Make new segments clock wise oriented:
	if( m_paths.count() > 0 )
	{
		if( p->counterClockwise() )
		{
			p->revert();
		}
	}
	else
	{
		if( !( p->counterClockwise() ) )
		{
			p->revert();
		}
	}

	m_paths.append( p );
}

void
VComposite::transform( const QWMatrix& m )
{
	VPathListIterator itr( m_paths );
	for( itr.toFirst(); itr.current(); ++itr )
	{
		itr.current()->transform( m );
	}

	// Apply transformation to gradients too:
	if( m_stroke->type() == VStroke::grad )
		m_stroke->gradient().transform( m );
	if( m_fill->type() == VFill::grad )
		m_fill->gradient().transform( m );
}

const KoRect&
VComposite::boundingBox() const
{
	if( m_boundingBoxIsInvalid )
	{
		// clear:
		m_boundingBox = KoRect();

		VPathListIterator itr( m_paths );
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

VComposite*
VComposite::clone() const
{
	return new VComposite( *this );
}

void
VComposite::save( QDomElement& element ) const
{
	if( state() != deleted )
	{
		QDomElement me = element.ownerDocument().createElement( "COMPOSITE" );
		element.appendChild( me );

		VObject::save( me );

		// save paths:
		VPathListIterator itr( m_paths );
		for( itr.toFirst(); itr.current(); ++itr )
		{
			itr.current()->save( me );
		}
	}
}

void
VComposite::load( const QDomElement& element )
{
	m_paths.clear();
	setState( normal );

	QDomNodeList list = element.childNodes();
	for( uint i = 0; i < list.count(); ++i )
	{
		if( list.item( i ).isElement() )
		{
			QDomElement child = list.item( i ).toElement();

			if( child.tagName() == "PATH" )
			{
				VPath path( this );
				path.load( child );

				combinePath( path );
			}
			else
			{
				VObject::load( child );
			}
		}
	}
}

void
VComposite::accept( VVisitor& visitor )
{
	visitor.visitVComposite( *this );
}

