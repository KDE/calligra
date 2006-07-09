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
#include <KoRect.h>

#include <render/vqpainter.h>

#include <kdebug.h>

VImage::VImage( VObject *parent, const QString &fname ) : VObject( parent ), m_image( 0L ), m_fname( fname )
{
	m_stroke = new VStroke( this );
	m_fill = new VFill();
	m_image = new QImage( m_fname );
	if( m_image->depth() != 32 )
        *m_image = m_image->convertDepth( 32 );
	m_image->setAlphaBuffer( true );
	*m_image = m_image->swapRGB();
	*m_image = m_image->mirror( false, true );
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
VImage::draw( VPainter *painter, const KoRect * ) const
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
		KoRect bbox = KoRect( 0, 0, m_image->width(), m_image->height() );
		KoPoint tl = bbox.topLeft().transform( m_matrix );
		KoPoint tr = bbox.topRight().transform( m_matrix );
		KoPoint bl = bbox.bottomLeft().transform( m_matrix );
		KoPoint br = bbox.bottomRight().transform( m_matrix );

	    painter->moveTo( tl );
	    painter->lineTo( tr );
	    painter->lineTo( br );
	    painter->lineTo( bl );
	    painter->lineTo( tl );

		painter->setRasterOp( Qt::XorROP );
		//painter->setPen( stroke() );
		painter->setPen( Qt::yellow );
		painter->setBrush( Qt::NoBrush );
		painter->strokePath();
		return;
	}

	//painter->setWorldMatrix( m_matrix );

	//*m_image = m_image->smoothScale( m_image->width() * zoomFactor, m_image->height() * zoomFactor, QImage::ScaleMin );
	m_boundingBox = KoRect( 0, 0, m_image->width(), m_image->height() );
	m_boundingBox = m_boundingBox.transform( m_matrix );
	if( !m_image->isNull() )
		painter->drawImage( *m_image, m_matrix );
}

void
VImage::transform( const QWMatrix& m )
{
	//QWMatrix m2 = m;
	//m_matrix *= m2.scale( 1.0, -1.0 );
	m_matrix *= m;
	kdDebug(38000) << "dx : " << m.dx() << ", dy : " << m.dy() << endl;
	m_boundingBox = m_boundingBox.transform( m );
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
	kdDebug(38000) << "VImage::load : " << m_fname.latin1() << endl;
	delete m_image;
	m_image = new QImage( m_fname );
	if( m_image->depth() != 32 )
        *m_image = m_image->convertDepth( 32 );
	m_image->setAlphaBuffer( true );
	*m_image = m_image->swapRGB();
	*m_image = m_image->mirror( false, true );
	m_boundingBox = KoRect( 0, 0, m_image->width(), m_image->height() );
}

void
VImage::accept( VVisitor& visitor )
{
	visitor.visitVImage( *this );
}

