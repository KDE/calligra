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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

/* vselecttoolbar.cc */

#include <qlabel.h>

#include <klocale.h>

#include "koUnitWidgets.h"
#include "vselecttoolbar.h"

VSelectToolBar::VSelectToolBar( QWidget* parent, const char* name ) : KToolBar( parent, name )
{
	setCaption( i18n( "Object Properties" ) );
	QLabel *x_label = new QLabel( i18n( "X:" ), this );
	insertWidget( 0, x_label->width(), x_label );
	m_x = new KoUnitDoubleSpinBox( this, 0.0, 1000.0, 0.5 );
	insertWidget( 1, m_x->width(), m_x );
	QLabel *y_label = new QLabel( i18n( "Y:" ), this );
	insertWidget( 2, y_label->width(), y_label );
	m_y = new KoUnitDoubleSpinBox( this, 0.0, 1000.0, 0.5 );
	insertWidget( 3, m_y->width(), m_y );
	
	insertSeparator( 4 );
	QLabel *w_label = new QLabel( i18n( "Width:" ), this );
	insertWidget( 5, w_label->width(), w_label );
	m_width = new KoUnitDoubleSpinBox( this, 0.0, 1000.0, 0.5 );
	insertWidget( 6, m_width->width(), m_width );
	QLabel *h_label = new QLabel( i18n( "Height:" ), this );
	insertWidget( 7, h_label->width(), h_label );
	m_height = new KoUnitDoubleSpinBox( this, 0.0, 1000.0, 0.5 );
	insertWidget( 8, m_height->width(), m_height );
	
}

VSelectToolBar::~VSelectToolBar()
{
}

#include "vselecttoolbar.moc"

