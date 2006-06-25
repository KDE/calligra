/* This file is part of the KDE project
   Copyright (C) 2001, 2002, 2003 The Karbon Developers

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <qdom.h>
#include <qpainter.h>
#include <qmatrix.h>
#include <QRegExp>

#include <KoUnit.h>
#include <KoStore.h>
#include <KoXmlWriter.h>
#include <KoXmlNS.h>
#include <KoGenStyles.h>

#include "vcomposite.h"
#include "vcomposite_iface.h"
#include "vfill.h"
#include "vpainter.h"
#include "vsegment.h"
#include "vstroke.h"
#include "vvisitor.h"
#include "vpath.h"
#include "commands/vtransformcmd.h"
#include "vdocument.h"

#include <kdebug.h>


VPath::VPath( VObject* parent, VState state )
	: VObject( parent, state ), m_fillRule( winding )
{
	m_paths.setAutoDelete( true );

	// add an initial path:
	m_paths.append( new VSubpath( this ) );

	// we need a stroke for boundingBox() at anytime:
	m_stroke = new VStroke( this );
	m_fill = new VFill();

	m_drawCenterNode = false;
}

VPath::VPath( const VPath& composite )
	: VObject( composite ), SVGPathParser()
{
	m_paths.setAutoDelete( true );

	VSubpath* path;

	VSubpathListIterator itr( composite.m_paths );
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
	m_matrix = composite.m_matrix;
}

VPath::~VPath()
{
}

/*
DCOPObject* VPath::dcopObject()
{
	if ( !m_dcop )
		m_dcop = new VPathIface( this );

	return m_dcop;
}
*/

void
VPath::draw( VPainter* painter, const QRectF *rect ) const
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

	VSubpathListIterator itr( m_paths );

	// draw simplistic contour:
	if( state() == edit )
	{
		for( itr.toFirst(); itr.current(); ++itr )
		{
			if( !itr.current()->isEmpty() )
			{
				painter->newPath();
				// TODO: needs porting to Qt4
				// painter->setRasterOp( Qt::XorROP );
				painter->setPen( QColor( "yellow" ) );
				painter->setBrush( Qt::NoBrush );

				VSubpathIterator jtr( *( itr.current() ) );
				for( ; jtr.current(); ++jtr )
				{
					jtr.current()->draw( painter );
				}

				painter->strokePath();
			}
		}
	}
	else if( state() != edit )
	{
		// paint fill:
		painter->newPath();
		painter->setFillRule( m_fillRule );

		for( itr.toFirst(); itr.current(); ++itr )
		{
			if( !itr.current()->isEmpty() )
			{
				VSubpathIterator jtr( *( itr.current() ) );
				for( ; jtr.current(); ++jtr )
				{
					jtr.current()->draw( painter );
				}
			}
		}

		// TODO: needs porting to Qt4
		// painter->setRasterOp( Qt::CopyROP );
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

const QPointF&
VPath::currentPoint() const
{
	return m_paths.getLast()->currentPoint();
}

bool
VPath::moveTo( const QPointF& p )
{
	// Append a new subpath if current subpath is not empty.
	if( !m_paths.getLast()->isEmpty() )
	{
		VSubpath* path = new VSubpath( this );
		m_paths.append( path );
	}

	return m_paths.getLast()->moveTo( p );
}

bool
VPath::lineTo( const QPointF& p )
{
	return m_paths.getLast()->lineTo( p );
}

bool
VPath::curveTo(
	const QPointF& p1, const QPointF& p2, const QPointF& p3 )
{
	return m_paths.getLast()->curveTo( p1, p2, p3 );
}

bool
VPath::curve1To( const QPointF& p2, const QPointF& p3 )
{
	return m_paths.getLast()->curve1To( p2, p3 );
}

bool
VPath::curve2To( const QPointF& p1, const QPointF& p3 )
{
	return m_paths.getLast()->curve2To( p1, p3 );
}

bool
VPath::arcTo( const QPointF& p1, const QPointF& p2, const double r )
{
	return m_paths.getLast()->arcTo( p1, p2, r );
}

void
VPath::close()
{
	m_paths.getLast()->close();

	// Append a new subpath.
	VSubpath* path = new VSubpath( this );
	path->moveTo( currentPoint() );
	m_paths.append( path );
}

bool
VPath::isClosed() const
{
	return m_paths.getLast()->isEmpty() || m_paths.getLast()->isClosed();
}

void
VPath::combine( const VPath& composite )
{
	VSubpathListIterator itr( composite.m_paths );
	for( ; itr.current(); ++itr )
	{
		combinePath( *( itr.current() ) );
	}
}

void
VPath::combinePath( const VSubpath& path )
{
	VSubpath* p = path.clone();
	p->setParent( this );

	// TODO: do complex inside tests instead:
	// Make new segments clock wise oriented:

	m_paths.append( p );
	m_fillRule = fillMode();
}

bool
VPath::pointIsInside( const QPointF& p ) const
{
	// Check if point is inside boundingbox.
	if( !boundingBox().contains( p ) )
		return false;


	VSubpathListIterator itr( m_paths );

	for( itr.toFirst(); itr.current(); ++itr )
	{
		if( itr.current()->pointIsInside( p ) )
			return true;
	}

	return false;
}

bool
VPath::intersects( const VSegment& segment ) const
{
	// Check if boundingboxes intersect.
	if( !boundingBox().intersects( segment.boundingBox() ) )
		return false;


	VSubpathListIterator itr( m_paths );

	for( itr.toFirst(); itr.current(); ++itr )
	{
		if( itr.current()->intersects( segment ) )
			return true;
	}

	return false;
}


VFillRule
VPath::fillMode() const
{
	return ( m_paths.count() > 1 ) ? evenOdd : winding;
}

const QRectF&
VPath::boundingBox() const
{
	if( m_boundingBoxIsInvalid )
	{
		VSubpathListIterator itr( m_paths );
		itr.toFirst();

		m_boundingBox = itr.current() ? itr.current()->boundingBox() : QRectF();

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

		QString d;
		saveSvgPath( d );
		me.setAttribute( "d", d );

		//writeTransform( me );

		// save fill rule if necessary:
		if( !( m_fillRule == evenOdd ) )
			me.setAttribute( "fillRule", m_fillRule );
	}
}

void
VPath::saveOasis( KoStore *store, KoXmlWriter *docWriter, KoGenStyles &mainStyles, int &index ) const
{
	if( state() != deleted )
	{
		docWriter->startElement( "draw:path" );

		QString d;
		saveSvgPath( d );
		docWriter->addAttribute( "svg:d", d );

		double x = boundingBox().x();
		double y = boundingBox().y();
		double w = boundingBox().width();
		double h = boundingBox().height();

		docWriter->addAttribute( "svg:viewBox", QString( "%1 %2 %3 %4" ).arg( x ).arg( y ).arg( w ).arg( h ) );
		docWriter->addAttributePt( "svg:x", x );
		docWriter->addAttributePt( "svg:y", y );
		docWriter->addAttributePt( "svg:width", w );
		docWriter->addAttributePt( "svg:height", h );

		VObject::saveOasis( store, docWriter, mainStyles, index );

		QMatrix tmpMat;
		tmpMat.scale( 1, -1 );
		tmpMat.translate( 0, -document()->height() );
	
		QString transform = buildOasisTransform( tmpMat );
		if( !transform.isEmpty() )
			docWriter->addAttribute( "draw:transform", transform );

		docWriter->endElement();
	}
}

void
VPath::saveOasisFill( KoGenStyles &mainStyles, KoGenStyle &stylesobjectauto ) const
{
	if( m_fill )
	{
		QMatrix mat;
		mat.scale( 1, -1 );
		mat.translate( 0, -document()->height() );

		// mirror fill before saving
		VFill fill( *m_fill );
		fill.transform( mat );
		fill.saveOasis( mainStyles, stylesobjectauto );
		// save fill rule if necessary:
		if( !( m_fillRule == evenOdd ) )
			stylesobjectauto.addProperty( "svg:fill-rule", "winding" );
	}
}

void
VPath::transformByViewbox( const QDomElement &element, QString viewbox )
{
	if( ! viewbox.isEmpty() )
	{
		// allow for viewbox def with ',' or whitespace
		QStringList points = viewbox.replace( ',', ' ' ).simplified().split( ' ', QString::SkipEmptyParts );

		double w = KoUnit::parseValue( element.attributeNS( KoXmlNS::svg, "width", QString::null ) );
		double h = KoUnit::parseValue( element.attributeNS( KoXmlNS::svg, "height", QString::null ) );
		double x = KoUnit::parseValue( element.attributeNS( KoXmlNS::svg, "x", QString::null ) );
		double y = KoUnit::parseValue( element.attributeNS( KoXmlNS::svg, "y", QString::null ) );

		QMatrix mat;
		mat.translate( x-KoUnit::parseValue( points[0] ), y-KoUnit::parseValue( points[1] ) );
		mat.scale( w / KoUnit::parseValue( points[2] ) , h / KoUnit::parseValue( points[3] ) );
		VTransformCmd cmd( 0L, mat );
		cmd.visitVPath( *this );
	}
}

bool
VPath::loadOasis( const QDomElement &element, KoOasisLoadingContext &context )
{
	setState( normal );

	QString viewbox;

	if( element.localName() == "path" )
	{
		QString data = element.attributeNS( KoXmlNS::svg, "d", QString::null );
		if( data.length() > 0 )
		{
			loadSvgPath( data );
		}
	
		m_fillRule = element.attributeNS( KoXmlNS::svg, "fill-rule", QString::null ) == "winding" ? winding : evenOdd;

		viewbox = element.attributeNS( KoXmlNS::svg, "viewBox", QString::null );
	}
	else if( element.localName() == "custom-shape" )
	{
		QDomNodeList list = element.childNodes();
		for( int i = 0; i < list.count(); ++i )
		{
			if( list.item( i ).isElement() )
			{
				QDomElement e = list.item( i ).toElement();
				if( e.namespaceURI() != KoXmlNS::draw )
					continue;
				
				if( e.localName() == "enhanced-geometry" )
				{
					QString data = e.attributeNS( KoXmlNS::draw, "enhanced-path", QString::null );
					if( ! data.isEmpty() )
						loadSvgPath( data );

					viewbox = e.attributeNS( KoXmlNS::svg, "viewBox", QString::null );
				}
			}
		}
	}

	transformByViewbox( element, viewbox );

	QString trafo = element.attributeNS( KoXmlNS::draw, "transform", QString::null );
	if( !trafo.isEmpty() )
		transformOasis( trafo );

	return VObject::loadOasis( element, context );
}

void
VPath::load( const QDomElement& element )
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
	for( int i = 0; i < list.count(); ++i )
	{
		if( list.item( i ).isElement() )
		{
			QDomElement child = list.item( i ).toElement();

			if( child.tagName() == "PATH" )
			{
				VSubpath path( this );
				path.load( child );

				combinePath( path );
			}
			else
			{
				VObject::load( child );
			}
		}
	}

	QString trafo = element.attribute( "transform" );
	if( !trafo.isEmpty() )
		transform( trafo );
}

void
VPath::loadSvgPath( const QString &d )
{
	//QTime s;s.start();
	parseSVG( d, true );
	//kDebug(38000) << "Parsing time : " << s.elapsed() << endl;
}

void
VPath::saveSvgPath( QString &d ) const
{
	// save paths to svg:
	VSubpathListIterator itr( m_paths );
	for( itr.toFirst(); itr.current(); ++itr )
	{
		if( !itr.current()->isEmpty() )
			itr.current()->saveSvgPath( d );
	}
}

void
VPath::svgMoveTo( double x1, double y1, bool )
{
	moveTo( QPointF( x1, y1 ) );
}

void
VPath::svgLineTo( double x1, double y1, bool )
{
	lineTo( QPointF( x1, y1 ) );
}

void
VPath::svgCurveToCubic( double x1, double y1, double x2, double y2, double x, double y, bool )
{
	curveTo( QPointF( x1, y1 ), QPointF( x2, y2 ), QPointF( x, y ) );
}

void
VPath::svgClosePath()
{
	close();
}

void
VPath::accept( VVisitor& visitor )
{
	visitor.visitVPath( *this );
}

void
VPath::transform( const QString &transform )
{
	VTransformCmd cmd( 0L, parseTransform( transform ) );
	cmd.visitVPath( *this );
}

void
VPath::transformOasis( const QString &transform )
{
	VTransformCmd cmd( 0L, parseOasisTransform( transform ) );
	cmd.visitVPath( *this );
}

QMatrix
VPath::parseTransform( const QString &transform )
{
	QMatrix result;

	// Split string for handling 1 transform statement at a time
	QStringList subtransforms = transform.split(')', QString::SkipEmptyParts);
	QStringList::ConstIterator it = subtransforms.begin();
	QStringList::ConstIterator end = subtransforms.end();
	for(; it != end; ++it)
	{
		QStringList subtransform = (*it).split('(', QString::SkipEmptyParts);

		subtransform[0] = subtransform[0].trimmed().toLower();
		subtransform[1] = subtransform[1].simplified();
		QRegExp reg("[,( ]");
		QStringList params = subtransform[1].split(reg, QString::SkipEmptyParts);

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

	return result;
}

QMatrix
VPath::parseOasisTransform( const QString &transform )
{
	QMatrix result;

	// Split string for handling 1 transform statement at a time
	QStringList subtransforms = transform.split(')', QString::SkipEmptyParts);
	QStringList::ConstIterator it = subtransforms.begin();
	QStringList::ConstIterator end = subtransforms.end();
	for(; it != end; ++it)
	{
		QStringList subtransform = (*it).split('(', QString::SkipEmptyParts);

		subtransform[0] = subtransform[0].trimmed().toLower();
		subtransform[1] = subtransform[1].simplified();
		QRegExp reg("[,( ]");
		QStringList params = subtransform[1].split(reg, QString::SkipEmptyParts);

		if(subtransform[0].startsWith(";") || subtransform[0].startsWith(","))
			subtransform[0] = subtransform[0].right(subtransform[0].length() - 1);

		if(subtransform[0] == "rotate")
		{
			// TODO find out what oo2 really does when rotating, it seems severly broken
			if(params.count() == 3)
			{
				double x = KoUnit::parseValue( params[1] );
				double y = KoUnit::parseValue( params[2] );

				result.translate(x, y);
				// oo2 rotates by radians
				result.rotate( params[0].toDouble()*VGlobal::one_pi_180 );
				result.translate(-x, -y);
			}
			else
			{
				// oo2 rotates by radians
				result.rotate( params[0].toDouble()*VGlobal::one_pi_180 );
			}
		}
		else if(subtransform[0] == "translate")
		{
			if(params.count() == 2)
			{
				double x = KoUnit::parseValue( params[0] );
				double y = KoUnit::parseValue( params[1] );
				result.translate(x, y);
			}
			else    // Spec : if only one param given, assume 2nd param to be 0
				result.translate( KoUnit::parseValue( params[0] ) , 0);
		}
		else if(subtransform[0] == "scale")
		{
			if(params.count() == 2)
				result.scale(params[0].toDouble(), params[1].toDouble());
			else    // Spec : if only one param given, assume uniform scaling
				result.scale(params[0].toDouble(), params[0].toDouble());
		}
		else if(subtransform[0] == "skewx")
			result.shear(tan(params[0].toDouble()), 0.0F);
		else if(subtransform[0] == "skewy")
			result.shear(tan(params[0].toDouble()), 0.0F);
		else if(subtransform[0] == "skewy")
			result.shear(0.0F, tan(params[0].toDouble()));
		else if(subtransform[0] == "matrix")
		{
			if(params.count() >= 6)
				result.setMatrix(params[0].toDouble(), params[1].toDouble(), params[2].toDouble(), params[3].toDouble(), KoUnit::parseValue( params[4] ), KoUnit::parseValue( params[5] ) );
		}
	}

	return result;
}

QString
VPath::buildSvgTransform() const
{
	return buildSvgTransform( m_matrix );
}

QString 
VPath::buildSvgTransform( const QMatrix &mat ) const
{
	QString transform;
	if( !mat.isIdentity() )
	{
		transform = QString(  "matrix(%1, %2, %3, %4, %5, %6)" ).arg( mat.m11() )
																.arg( mat.m12() )
																.arg( mat.m21() )
																.arg( mat.m22() )
																.arg( mat.dx() )
																.arg( mat.dy() );
	}
	return transform;
}

QString
VPath::buildOasisTransform() const
{
	return buildSvgTransform( m_matrix );
}

QString 
VPath::buildOasisTransform( const QMatrix &mat ) const
{
	QString transform;
	if( !mat.isIdentity() )
	{
		transform = QString(  "matrix(%1, %2, %3, %4, %5pt, %6pt)" ).arg( mat.m11() )
																.arg( mat.m12() )
																.arg( mat.m21() )
																.arg( mat.m22() )
																.arg( mat.dx() )
																.arg( mat.dy() );
	}
	return transform;
}
