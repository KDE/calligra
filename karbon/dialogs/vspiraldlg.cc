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

#include <qcombobox.h>
#include <qevent.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpainter.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qstring.h>
#include <qwidget.h>

#include <klocale.h>
#include <knuminput.h>
#include "vspiraldlg.h"

VSpiralDlg::VSpiralDlg( QWidget* parent, const char* name )
	: KDialog( parent, name, true, Qt::WStyle_Customize |
		WType_Dialog | Qt::WStyle_NormalBorder | Qt::WStyle_Title )
{
	setCaption( i18n( "Insert Spiral" ) );

	QBoxLayout* outerbox = new QHBoxLayout( this );

	// add input fields on the left:
	QGroupBox* group = new QGroupBox( 2, Qt::Horizontal, i18n( "Properties" ), this );
	outerbox->addWidget( group );

	new QLabel( i18n( "Radius:" ), group );
	m_radius = new KDoubleNumInput( 0, group );
	new QLabel( i18n( "Segments:" ), group );
	m_segments = new QSpinBox( group );
	m_segments->setMinValue( 1 );
	new QLabel( i18n( "Fade:" ), group );
	m_fade = new KDoubleNumInput( 0, group );
	new QLabel( i18n( "Orientation:" ), group );
	m_clockwise = new QComboBox( false,group );
	m_clockwise->insertItem( i18n( "Clockwise" ), 0 );
	m_clockwise->insertItem( i18n( "Counter Clockwise" ), 1 );

	outerbox->addSpacing( 2 );

	// add buttons on the right side:
	QBoxLayout* innerbox = new QVBoxLayout( outerbox );

	innerbox->addStretch();

	QPushButton* okbutton = new QPushButton( i18n( "&Ok" ), this );
	QPushButton* cancelbutton = new QPushButton( i18n( "&Cancel" ), this );

	okbutton->setMaximumSize( okbutton->sizeHint() );
	cancelbutton->setMaximumSize( cancelbutton->sizeHint() );

	okbutton->setFocus();

	innerbox->addWidget( okbutton );
	innerbox->addSpacing( 2 );
	innerbox->addWidget( cancelbutton );

	// signals and slots:
	connect( okbutton, SIGNAL( clicked() ), this, SLOT( accept() ) );
	connect( cancelbutton, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

double
VSpiralDlg::radius() const
{
	return m_radius->value();
}

uint
VSpiralDlg::segments() const
{
	return m_segments->value();
}

double
VSpiralDlg::fade() const
{
    return m_fade->value();
}

bool
VSpiralDlg::clockwise() const
{
	if( m_clockwise->currentItem() == 0 )
		return true;
	else
		return false;
}

void
VSpiralDlg::setRadius( double value )
{
    m_radius->setValue( value );
}

void
VSpiralDlg::setSegments( uint value )
{
	m_segments->setValue( value );
}

void
VSpiralDlg::setFade( double value )
{
    m_fade->setValue( value );
}

void
VSpiralDlg::setClockwise( bool value )
{
	if( value )
		m_clockwise->setCurrentItem( 0 );
	else
		m_clockwise->setCurrentItem( 1 );
}

#include "vspiraldlg.moc"

