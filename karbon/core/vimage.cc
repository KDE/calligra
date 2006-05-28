/* This file is part of the KDE project
   Copyright (C) 2001, 2002, 2003 The Karbon Developers
*/

#include "vimage.h"
#include "vpainter.h"
#include "vvisitor.h"
#include "vpath.h"
#include "vfill.h"
#include "vstroke.h"

#include <qdom.h>
#include <qimage.h>
#include <QRectF>
#include <QPointF>

#include <render/vqpainter.h>

#include <kdebug.h>

VImage::VImage( VObject *parent, const QString &fname ) : VObject( parent ), m_image( 0L ), m_fname( fname )
{
	m_stroke = new VStroke( this );
	m_fill = new VFill();
	m_image = new QImage( m_fname );
	if( m_image->depth() != 32 )
        *m_image = m_image->convertToFormat( QImage::Format_ARGB32 );
	// TODO: Check if the inversion also needs to include the alpha channel !!!
	// Previous, in Qt3, the swapRGB function was used.
	m_image->invertPixels(QImage::InvertRgb);
	*m_image = m_image->mirrored( false /*horizontal*/, true /*vertical*/ );
}

VImage::VImage( const VImage &other ) : VObject( other )
{
	if( other.m_image )
		m_image = new QImage( *other.m_image );
	else
		m_image			= 0L;
	m_fname			= other.m_fname;
	m_boundingBox	= other.m_boundingBox;
	m_matrix		= other.m_matrix;
}

VImage::~VImage()
{
	delete m_image;
}

void
VImage::draw( VPainter *painter, const QRectF * ) const
{
	if(
		state() == deleted ||
		state() == hidden ||
		state() == hidden_locked )
	{
		return;
	}

	if( state() == edit )
	{
		QRectF bbox = QRectF( 0, 0, m_image->width(), m_image->height() );

		double x, y;

		m_matrix.map(bbox.topLeft().x(), bbox.topLeft().y(), &x, &y);
		QPointF tl = QPointF(x,y);

		m_matrix.map(bbox.topRight().x(), bbox.topRight().y(), &x, &y);
		QPointF tr = QPointF(x,y);

		m_matrix.map(bbox.bottomLeft().x(), bbox.bottomLeft().y(), &x, &y);
		QPointF bl = QPointF(x,y);

		m_matrix.map(bbox.bottomRight().x(), bbox.bottomRight().y(), &x, &y);
		QPointF br = QPointF(x,y);

	    painter->moveTo( tl );
	    painter->lineTo( tr );
	    painter->lineTo( br );
	    painter->lineTo( bl );
	    painter->lineTo( tl );

		// TODO: Needs porting to Qt4
		// painter->setRasterOp( Qt::XorROP );

		//painter->setPen( stroke() );
		painter->setPen( Qt::yellow );
		painter->setBrush( Qt::NoBrush );
		painter->strokePath();
		return;
	}

	//painter->setMatrix( m_matrix );

	//*m_image = m_image->smoothScale( m_image->width() * zoomFactor, m_image->height() * zoomFactor, QImage::ScaleMin );
	m_boundingBox = QRectF( 0, 0, m_image->width(), m_image->height() );
	// TODO: QRectF doesn't contain a transform function: m_boundingBox = m_boundingBox.transform( m_matrix );
	if( !m_image->isNull() )
		painter->drawImage( *m_image, m_matrix );
}

void
VImage::transform( const QMatrix& m )
{
	//QMatrix m2 = m;
	//m_matrix *= m2.scale( 1.0, -1.0 );
	m_matrix *= m;
	kDebug(38000) << "dx : " << m.dx() << ", dy : " << m.dy() << endl;
	// TODO: QRectF doesn't contain a transform function: m_boundingBox = m_boundingBox.transform( m );
}

VObject *
VImage::clone() const
{
	return new VImage( *this );
}

void
VImage::save( QDomElement& element ) const
{
	if( state() != deleted )
	{
		QDomElement me = element.ownerDocument().createElement( "IMAGE" );
		element.appendChild( me );

		me.setAttribute( "fname", m_fname );
		me.setAttribute( "m11", m_matrix.m11() );
		me.setAttribute( "m12", m_matrix.m12() );
		me.setAttribute( "m21", m_matrix.m21() );
		me.setAttribute( "m22", m_matrix.m22() );
		me.setAttribute( "dx", m_matrix.dx() );
		me.setAttribute( "dy", m_matrix.dy() );
	}
}

void
VImage::load( const QDomElement& element )
{
	setState( normal );
	m_fname = element.attribute( "fname" );
	m_matrix.setMatrix( element.attribute( "m11", "1.0" ).toDouble(),
						element.attribute( "m12", "0.0" ).toDouble(),
						element.attribute( "m21", "0.0" ).toDouble(),
						element.attribute( "m22", "1.0" ).toDouble(),
						element.attribute( "dx", "0.0" ).toDouble(),
						element.attribute( "dy", "0.0" ).toDouble() );
	kDebug(38000) << "VImage::load : " << m_fname.toLatin1() << endl;
	delete m_image;
	m_image = new QImage( m_fname );
	if( m_image->depth() != 32 )
        *m_image = m_image->convertToFormat( QImage::Format_ARGB32 );
	// TODO: Check if the inversion also needs to include the alpha channel !!!
	// Previous, in Qt3, the swapRGB function was used.
	m_image->invertPixels(QImage::InvertRgb);
	*m_image = m_image->mirrored( false /*horizontal*/ , true /*vertical*/ );
	m_boundingBox = QRectF( 0, 0, m_image->width(), m_image->height() );
}

void
VImage::accept( VVisitor& visitor )
{
	visitor.visitVImage( *this );
}

