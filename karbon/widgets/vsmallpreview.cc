/* This file is part of the KDE project
   Made by Tomislav Lukman (tomislav.lukman@ck.t-com.hr)
   Copyright (C) 2005, The Karbon Developers

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

/* vsmallpreview.cc */
#include <qcolor.h>
#include <qframe.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpixmap.h>

#include <klocale.h>

#include "karbon_part.h"
#include "vfill.h"
#include "vkopainter.h"
#include "vstroke.h"

#include "vsmallpreview.h"

#define FRAMEWIDTH		40

VSmallPreview::VSmallPreview( KarbonPart *part, QWidget* parent, const char* name )
	: QWidget( parent, name ), m_part( part )
{
	/* Create widget layout */
	QHBoxLayout *layout = new QHBoxLayout( this, 4 );
	m_strokeLabel = new QLabel( i18n( "Stroke: none" ), this );
	layout->addWidget( m_strokeLabel );
	m_strokeFrame = new QFrame( this );
	m_strokeFrame->setFixedWidth ( FRAMEWIDTH );
	m_strokeFrame->setFrameStyle( QFrame::GroupBoxPanel | QFrame::Plain );
	layout->addWidget( m_strokeFrame );
	m_fillLabel = new QLabel( i18n( "Fill: none" ), this );
	layout->addWidget( m_fillLabel );
	m_fillFrame = new QFrame( this );
	m_fillFrame->setFixedWidth ( FRAMEWIDTH );
	m_fillFrame->setFrameStyle( QFrame::GroupBoxPanel | QFrame::Plain );
	layout->addWidget( m_fillFrame );
	layout->activate();

	m_fill = new VFill();
	m_stroke = new VStroke();

	m_pixmap.resize( m_fillFrame->width(), m_fillFrame->height() );
	m_painter = new VKoPainter( &m_pixmap, m_fillFrame->width(), m_fillFrame->height() );
}

VSmallPreview::~VSmallPreview()
{
	delete( m_painter );
}

void
VSmallPreview::update( const VStroke &s, const VFill &f )
{
	m_fill = &f;
	m_stroke = &s;

	drawStroke( s );
	drawFill( f );
}

void
VSmallPreview::paintEvent( QPaintEvent* /*event*/ )
{
	if( m_stroke != NULL ) drawStroke( *m_stroke );
	if( m_fill != NULL ) drawFill( *m_fill );
}

void
VSmallPreview::drawFill( const VFill &f )
{
	VFill fill;
	VStroke stroke;

	m_painter->begin();
	m_painter->setPen( Qt::NoPen );
	fill.setColor( Qt::white );
	m_painter->setBrush( fill );
	m_painter->drawRect( KoRect( 0, 0, m_fillFrame->width(), m_fillFrame->height() ) );
	
	if( f.type() != VFill::none )
	{
		if( f.type() != VFill::solid )
		{
			if( f.type() == VFill::grad )
			{
				fill.gradient() = f.gradient();
				fill.setType( VFill::grad );
				m_fillLabel->setText( i18n( "Fill: gradient") );
			}
			else
			{
				fill.pattern() = f.pattern();
				fill.setType( VFill::patt );
				m_fillLabel->setText( i18n( "Fill: pattern") );
			}
		}
		else
		{
			m_fillLabel->setText( i18n( "Fill: color") );
			fill.setColor( f.color() );
		}
		//stroke.setColor( Qt::black );
		//stroke.setLineWidth( 1.0 );
		m_painter->setPen( stroke );
		m_painter->setBrush( fill );
		m_painter->drawRect( KoRect( 0, 0, m_fillFrame->width(), m_fillFrame->height() ) );
	}
	
	if( f.type() == VFill::none )
	{
		m_fillLabel->setText( i18n( "Fill: none") );

		fill.setColor( Qt::white );
		m_painter->setBrush( fill );
		m_painter->drawRect( KoRect( 0, 0, m_fillFrame->width(), m_fillFrame->height() ) );

		stroke.setColor( Qt::red );
		stroke.setLineWidth( 2.0 );
		m_painter->setPen( stroke );
		m_painter->newPath();
		m_painter->moveTo( KoPoint( 4, m_fillFrame->height() - 4 ) );
		m_painter->lineTo( KoPoint( m_fillFrame->width() - 4, 4 ) );
		m_painter->strokePath();
	}

	m_painter->end();

	bitBlt( m_fillFrame, m_fillFrame->frameWidth(), m_fillFrame->frameWidth(), &m_pixmap, m_fillFrame->frameWidth(), m_fillFrame->frameWidth(), m_fillFrame->width() - m_fillFrame->frameWidth(), m_fillFrame->height() - m_fillFrame->frameWidth(), CopyROP );
}

void
VSmallPreview::drawStroke( const VStroke &s )
{
	VFill fill;
	VStroke stroke;

	m_painter->begin();
	m_painter->setPen( Qt::NoPen );
	fill.setColor( Qt::white );
	m_painter->setBrush( fill );
	m_painter->drawRect( KoRect( 0, 0, m_strokeFrame->width(), m_strokeFrame->height() ) );
	
	if( s.type() != VStroke::none )
	{
		if( s.type() != VStroke::solid )
		{
			if( s.type() == VStroke::grad )
			{
				fill.gradient() = s.gradient();
				fill.setType( VFill::grad );
				m_strokeLabel->setText( i18n( "Stroke: gradient") );
			}
			else
			{
				fill.pattern() = s.pattern();
				fill.setType( VFill::patt );
				m_strokeLabel->setText( i18n( "Stroke: pattern") );
			}
		}
		else
		{
			m_strokeLabel->setText( i18n( "Stroke: color") );
			fill.setColor( s.color() );
		}
		m_painter->setPen( stroke );
		m_painter->setBrush( fill );
		m_painter->drawRect( KoRect( 0, 0, m_strokeFrame->width(), m_strokeFrame->height() ) );
	}
	
	if( s.type() == VStroke::none )
	{
		m_strokeLabel->setText( i18n( "Stroke: none") );

		fill.setColor( Qt::white );
		m_painter->setBrush( fill );
		m_painter->drawRect( KoRect( 0, 0, m_strokeFrame->width(), m_strokeFrame->height() ) );

		stroke.setColor( Qt::red );
		stroke.setLineWidth( 2.0 );
		m_painter->setPen( stroke );
		m_painter->newPath();
		m_painter->moveTo( KoPoint( 4, m_strokeFrame->height() - 4 ) );
		m_painter->lineTo( KoPoint( m_strokeFrame->width() - 4, 4 ) );
		m_painter->strokePath();
	}

	m_painter->end();

	bitBlt( m_strokeFrame, m_strokeFrame->frameWidth(), m_strokeFrame->frameWidth(), &m_pixmap, m_strokeFrame->frameWidth(), m_strokeFrame->frameWidth(), m_strokeFrame->width() - m_strokeFrame->frameWidth(), m_strokeFrame->height() - m_strokeFrame->frameWidth(), CopyROP );
}

#include "vsmallpreview.moc"

