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
#include <qdatetime.h>
#include <qregexp.h>

#include <koPoint.h>
#include <koRect.h>

#include "vcomposite.h"
#include "vcomposite_iface.h"
#include "vfill.h"
#include "vpainter.h"
#include "vsegment.h"
#include "vstroke.h"
#include "vvisitor.h"
#include "vpath.h"
#include "commands/vtransformcmd.h"

#include <kdebug.h>


VComposite::VComposite( VObject* parent, VState state )
	: VObject( parent, state ), m_fillRule( winding )
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
	: VObject( composite ), SVGPathParser()
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
	m_fillRule = composite.m_fillRule;
}

VComposite::~VComposite()
{
}

DCOPObject* VComposite::dcopObject()
{
	if ( !m_dcop )
		m_dcop = new VCompositeIface( this );

	return m_dcop;
}


void
VComposite::draw( VPainter* painter, const KoRect *rect ) const
{
	if(
		state() == deleted ||
		state() == hidden ||
		state() == hidden_locked )
	{
		return;
	}

	if( rect && !rect->intersects( boundingBox() ) )
		return;

	painter->save();

	VPathListIterator itr( m_paths );

	// draw simplistic contour:
	if( state() == edit )
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
	}
	else if( state() != edit )
	{
		// paint fill:
		painter->newPath();
		painter->setFillRule( m_fillRule );

		for( itr.toFirst(); itr.current(); ++itr )
		{
			VPathIterator jtr( *( itr.current() ) );
			for( ; jtr.current(); ++jtr )
			{
				jtr.current()->draw( painter );
			}
		}

		painter->setRasterOp( Qt::CopyROP );
		painter->setPen( Qt::NoPen );
		painter->setBrush( *fill() );
		painter->fillPath();

		// draw stroke:
		painter->setPen( *stroke() );
		painter->setBrush( Qt::NoBrush );
		painter->strokePath();
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
	// Append a new subpath if current subpath is not empty.
	if( !m_paths.getLast()->isEmpty() )
	{
		VPath* path = new VPath( this );
		path->moveTo( currentPoint() );
		m_paths.append( path );
	}

	return m_paths.getLast()->moveTo( p );
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

	// Append a new subpath.
	VPath* path = new VPath( this );
	path->moveTo( currentPoint() );
	m_paths.append( path );
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

	m_paths.append( p );
	m_fillRule = fillMode();
}

bool
VComposite::pointIsInside( const KoPoint& p ) const
{
	// Check if point is inside boundingbox.
	if( !boundingBox().contains( p, true ) )
		return false;


	VPathListIterator itr( m_paths );

	for( itr.toFirst(); itr.current(); ++itr )
	{
		if( itr.current()->pointIsInside( p ) )
			return true;
	}

	return false;
}

bool
VComposite::intersects( const VSegment& segment ) const
{
	// Check if boundingboxes intersect.
	if( !boundingBox().intersects( segment.boundingBox() ) )
		return false;


	VPathListIterator itr( m_paths );

	for( itr.toFirst(); itr.current(); ++itr )
	{
		if( itr.current()->intersects( segment ) )
			return true;
	}

	return false;
}


VFillRule
VComposite::fillMode() const
{
	return ( m_paths.count() > 1 ) ? evenOdd : winding;
}

const KoRect&
VComposite::boundingBox() const
{
	if( m_boundingBoxIsInvalid )
	{
		VPathListIterator itr( m_paths );
		itr.toFirst();

		m_boundingBox = itr.current() ? itr.current()->boundingBox() : KoRect();

		for( ++itr; itr.current(); ++itr )
			m_boundingBox |= itr.current()->boundingBox();

		if( !m_boundingBox.isNull() )
		{
			// take line width into account:
			m_boundingBox.setCoords(
				m_boundingBox.left()   - 0.5 * stroke()->lineWidth(),
				m_boundingBox.top()    - 0.5 * stroke()->lineWidth(),
				m_boundingBox.right()  + 0.5 * stroke()->lineWidth(),
				m_boundingBox.bottom() + 0.5 * stroke()->lineWidth() );
		}
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
		QDomElement me = element.ownerDocument().createElement( "PATH" );
		element.appendChild( me );

		VObject::save( me );

		QString d;
		saveSvgPath( d );
		me.setAttribute( "d", d );

		// save fill rule if necessary:
		if( !( m_fillRule == evenOdd ) )
			me.setAttribute( "fillRule", m_fillRule );
	}
}


void
VComposite::load( const QDomElement& element )
{
	setState( normal );

	VObject::load( element );

	QString data = element.attribute( "d" );
	if( data.length() > 0 )
	{
		loadSvgPath( data );
	}
	m_fillRule = element.attribute( "fillRule" ) == 0 ? evenOdd : winding;
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
VComposite::loadSvgPath( const QString &d )
{
	QTime s;s.start();
	parseSVG( d );
	//kdDebug() << "Parsing time : " << s.elapsed() << endl;
}

void
VComposite::saveSvgPath( QString &d ) const
{
	// save paths to svg:
	VPathListIterator itr( m_paths );
	for( itr.toFirst(); itr.current(); ++itr )
	{
		if( !itr.current()->isEmpty() )
			itr.current()->saveSvgPath( d );
	}
}

void
VComposite::svgMoveTo( double x1, double y1 )
{
	moveTo( KoPoint( x1, y1 ) );
}

void
VComposite::svgLineTo( double x1, double y1 )
{
	lineTo( KoPoint( x1, y1 ) );
}

void
VComposite::svgCurveTo( double x1, double y1, double x2, double y2, double x3, double y3 )
{
	curveTo( KoPoint( x1, y1 ), KoPoint( x2, y2 ), KoPoint( x3, y3 ) );
}

void
VComposite::svgClosePath()
{
	close();
}

void
VComposite::accept( VVisitor& visitor )
{
	visitor.visitVComposite( *this );
}

QWMatrix
VComposite::transform( const QString &transform )
{
	QWMatrix result;

	// Split string for handling 1 transform statement at a time
	QStringList subtransforms = QStringList::split(')', transform);
	QStringList::ConstIterator it = subtransforms.begin();
	QStringList::ConstIterator end = subtransforms.end();
	for(; it != end; ++it)
	{
		QStringList subtransform = QStringList::split('(', (*it));

		subtransform[0] = subtransform[0].stripWhiteSpace().lower();
		subtransform[1] = subtransform[1].simplifyWhiteSpace();
		QRegExp reg("[,( ]");
		QStringList params = QStringList::split(reg, subtransform[1]);

		if(subtransform[0].startsWith(";") || subtransform[0].startsWith(","))
			subtransform[0] = subtransform[0].right(subtransform[0].length() - 1);

		if(subtransform[0] == "rotate")
		{
			if(params.count() == 3)
			{
				double x = params[1].toDouble();
				double y = params[2].toDouble();

				result.translate(x, y);
				result.rotate(params[0].toDouble());
				result.translate(-x, -y);
			}
			else
				result.rotate(params[0].toDouble());
		}
		else if(subtransform[0] == "translate")
		{
			if(params.count() == 2)
				result.translate(params[0].toDouble(), params[1].toDouble());
			else    // Spec : if only one param given, assume 2nd param to be 0
				result.translate(params[0].toDouble() , 0);
		}
		else if(subtransform[0] == "scale")
		{
			if(params.count() == 2)
				result.scale(params[0].toDouble(), params[1].toDouble());
			else    // Spec : if only one param given, assume uniform scaling
				result.scale(params[0].toDouble(), params[0].toDouble());
		}
		else if(subtransform[0] == "skewx")
			result.shear(tan(params[0].toDouble() * VGlobal::pi_180), 0.0F);
		else if(subtransform[0] == "skewy")
			result.shear(tan(params[0].toDouble() * VGlobal::pi_180), 0.0F);
		else if(subtransform[0] == "skewy")
			result.shear(0.0F, tan(params[0].toDouble() * VGlobal::pi_180));
		else if(subtransform[0] == "matrix")
		{
			if(params.count() >= 6)
				result.setMatrix(params[0].toDouble(), params[1].toDouble(), params[2].toDouble(), params[3].toDouble(), params[4].toDouble(), params[5].toDouble());
		}
	}

	VTransformCmd cmd( 0L, result );
	cmd.visitVComposite( *this );
	return result;
}

void
VComposite::writeTransform( QDomElement &me ) const
{
	if( !m_matrix.isIdentity() )
	{
		QString transform = QString("matrix(%1, %2, %3, %4, %5, %6)").arg( m_matrix.m11() )
																	.arg( m_matrix.m12() )
																	.arg( m_matrix.m21() )
																	.arg( m_matrix.m22() )
																	.arg( m_matrix.dx() )
																	.arg( m_matrix.dy() );
		me.setAttribute( "transform", transform );
	}
}

