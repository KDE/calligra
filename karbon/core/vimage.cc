/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include "vimage.h"
#include "vpainter.h"
#include "vvisitor.h"
#include "vpath.h"
#include "vfill.h"
#include "vstroke.h"

#include <qdom.h>
#include <qimage.h>
#include <koRect.h>

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
}

VImage::VImage( const VImage &other ) : VObject( other )
{
	if( other.m_image )
		m_image = new QImage( *other.m_image );
	else
		m_image			= 0L;
	m_fname			= other.m_fname;
	m_boundingBox	= other.m_boundingBox;
}

VImage::~VImage()
{
	delete m_image;
}

void
VImage::draw( VPainter *painter, const KoRect * ) const
{
	kdDebug() << "VImage::draw" << endl;
	if( state() == edit )
	{
		KoRect bbox = m_boundingBox;//.transform( m_matrix );
		kdDebug() << "m_boundingBox : " << m_boundingBox << endl;
	    painter->moveTo( bbox.topLeft() );
	    painter->lineTo( bbox.topRight() );
	    painter->lineTo( bbox.bottomRight() );
	    painter->lineTo( bbox.bottomLeft() );
	    painter->lineTo( bbox.topLeft() );

		painter->setRasterOp( Qt::XorROP );
		//painter->setPen( stroke() );
		painter->setPen( Qt::yellow );
		painter->setBrush( Qt::NoBrush );
		painter->strokePath();
		return;
	}

	//painter->setWorldMatrix( m_matrix );

	//*m_image = m_image->smoothScale( m_image->width() * zoomFactor, m_image->height() * zoomFactor, QImage::ScaleMin );
	//m_boundingBox = KoRect( 0, 0, m_image->width(), m_image->height() );
	//m_boundingBox = m_boundingBox.transform( m_matrix );
	painter->drawImage( *m_image, m_matrix );
}

void
VImage::transform( const QWMatrix& m )
{
	QWMatrix m2 = m;
	//m_matrix *= m2.scale( 1.0, -1.0 );
	m_matrix *= m;
	kdDebug() << "dx : " << m.dx() << ", dy : " << m.dy() << endl;
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
	QDomElement me = element.ownerDocument().createElement( "IMAGE" );
	element.appendChild( me );

	me.setAttribute( "fname", m_fname );
}

void
VImage::load( const QDomElement& element )
{
	m_fname = element.attribute( "fname" );
	kdDebug() << "VImage::load : " << m_fname.latin1() << endl;
	delete m_image;
	m_image = new QImage( m_fname );
	if( m_image->depth() != 32 )
        *m_image = m_image->convertDepth( 32 );
	m_image->setAlphaBuffer( true );
	*m_image = m_image->swapRGB();
	m_boundingBox = KoRect( 0, 0, m_image->width(), m_image->height() );
}

void
VImage::accept( VVisitor& visitor )
{
	visitor.visitVImage( *this );
}

