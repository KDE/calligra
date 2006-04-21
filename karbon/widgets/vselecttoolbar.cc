/* This file is part of the KDE project
   Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

/* vselecttoolbar.cc */

#include <qlabel.h>

#include <klocale.h>
#include <kdebug.h>

#include "KoUnitWidgets.h"
#include "vselecttoolbar.h"
#include "karbon_view.h"
#include "karbon_part.h"
#include "vselection.h"
#include "vtransformcmd.h"
#include <KoRect.h>

VSelectToolBar::VSelectToolBar( KarbonView *view, const char* name ) : KToolBar( view, name ), m_view( view )
{
	setCaption( i18n( "Object Properties" ) );
	QLabel *x_label = new QLabel( i18n( "X:" ), this, "kde toolbar widget" );
	insertWidget( 0, x_label->width(), x_label );
	m_x = new KoUnitDoubleSpinBox( this, 0.0, 1000.0, 0.5 );
	connect( m_x, SIGNAL( valueChanged( double ) ), this, SLOT( slotXChanged( double ) ) );
	insertWidget( 1, m_x->width(), m_x );
	QLabel *y_label = new QLabel( i18n( "Y:" ), this, "kde toolbar widget" );
	insertWidget( 2, y_label->width(), y_label );
	m_y = new KoUnitDoubleSpinBox( this, 0.0, 1000.0, 0.5 );
	connect( m_y, SIGNAL( valueChanged( double ) ), this, SLOT( slotYChanged( double ) ) );
	insertWidget( 3, m_y->width(), m_y );
	
	insertSeparator( 4 );
	QLabel *w_label = new QLabel( i18n( "selection width", "Width:" ), this, "kde toolbar widget" );
	insertWidget( 5, w_label->width(), w_label );
	m_width = new KoUnitDoubleSpinBox( this, 0.0, 1000.0, 0.5 );
	connect( m_width, SIGNAL( valueChanged( double ) ), this, SLOT( slotWidthChanged( double ) ) );
	insertWidget( 6, m_width->width(), m_width );
	QLabel *h_label = new QLabel( i18n( "Height:" ), this, "kde toolbar widget" );
	insertWidget( 7, h_label->width(), h_label );
	m_height = new KoUnitDoubleSpinBox( this, 0.0, 1000.0, 0.5 );
	connect( m_height, SIGNAL( valueChanged( double ) ), this, SLOT( slotHeightChanged( double ) ) );
	insertWidget( 8, m_height->width(), m_height );

	connect( m_view, SIGNAL( selectionChange() ), this, SLOT( slotSelectionChanged() ) );
}

VSelectToolBar::~VSelectToolBar()
{
}

void
VSelectToolBar::slotXChanged( double newval )
{
	double dx = newval - m_view->part()->document().selection()->boundingBox().topLeft().x();
	m_view->part()->addCommand( new VTranslateCmd( &m_view->part()->document(), dx, 0.0 ), true );
}

void
VSelectToolBar::slotYChanged( double newval )
{
	double dy = newval - m_view->part()->document().selection()->boundingBox().topLeft().y();
	m_view->part()->addCommand( new VTranslateCmd( &m_view->part()->document(), 0.0, dy ), true );
}

void
VSelectToolBar::slotWidthChanged( double newval )
{
	if( newval != 0.0 )
	{
		double sx = newval / m_view->part()->document().selection()->boundingBox().width();
		KoPoint sp = m_view->part()->document().selection()->boundingBox().topLeft();
		m_view->part()->addCommand( new VScaleCmd( &m_view->part()->document(), sp, sx, 1.0 ), true );
	}
}

void
VSelectToolBar::slotHeightChanged( double newval )
{
	if( newval != 0.0 )
	{
		double sy = newval / m_view->part()->document().selection()->boundingBox().height();
		KoPoint sp = m_view->part()->document().selection()->boundingBox().bottomLeft();
		m_view->part()->addCommand( new VScaleCmd( &m_view->part()->document(), sp, 1.0, sy ), true );
	}
}

void
VSelectToolBar::slotSelectionChanged()
{
	m_x->blockSignals( true );
	m_y->blockSignals( true );
	m_width->blockSignals( true );
	m_height->blockSignals( true );
	KoRect rect = m_view->part()->document().selection()->boundingBox();
	m_x->setValue( rect.topLeft().x() );
	m_y->setValue( rect.topLeft().y() );
	m_width->setValue( rect.width() );
	m_height->setValue( rect.height() );
	m_x->blockSignals( false );
	m_y->blockSignals( false );
	m_width->blockSignals( false );
	m_height->blockSignals( false );
}

#include "vselecttoolbar.moc"

