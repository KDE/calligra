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

#include <qgroupbox.h>
#include <qlabel.h>

#include <kcolorbutton.h>
#include <kcombobox.h>
#include <klocale.h>

#include "vgradientdlg.h"

VGradientDlg::VGradientDlg( QWidget* parent, const char* name )
	: KDialogBase( parent, name, true, i18n( "Insert Gradient" ), Ok | Cancel )
{
	// add input fields on the left:
	QGroupBox* group = new QGroupBox( 2, Qt::Horizontal, i18n( "Properties" ), this );

	new QLabel( i18n( "Start color:" ), group );
	m_startColor = new KColorButton( group );
	new QLabel( i18n( "End color:" ), group );
	m_endColor = new KColorButton( group );
	new QLabel( i18n( "Gradient target:" ), group );
	m_gradientFill = new KComboBox( false, group );
	m_gradientFill->insertItem( i18n( "Stroke" ), 0 );
	m_gradientFill->insertItem( i18n( "Fill" ), 1 );
	new QLabel( i18n( "Gradient repeat:" ), group );
	m_gradientRepeat = new KComboBox( false, group );
	m_gradientRepeat->insertItem( i18n( "None" ), 0 );
	m_gradientRepeat->insertItem( i18n( "Reflect" ), 1 );
	m_gradientRepeat->insertItem( i18n( "Repeat" ), 2 );
	new QLabel( i18n( "Gradient type:" ), group );
	m_gradientType = new KComboBox( false, group );
	m_gradientType->insertItem( i18n( "Linear" ), 0 );
	m_gradientType->insertItem( i18n( "Radial" ), 1 );
	//m_gradientType->insertItem( i18n( "Conical" ), 2 );
	group->setMinimumWidth( 300 );

	// signals and slots:
	connect( this, SIGNAL( okClicked() ), this, SLOT( accept() ) );
	connect( this, SIGNAL( cancelClicked() ), this, SLOT( reject() ) );
	
	setMainWidget( group );
	setFixedSize( baseSize() );
}

QColor
VGradientDlg::startColor() const
{
	return m_startColor->color();
}

QColor
VGradientDlg::endColor() const
{
	return m_endColor->color();
}

int
VGradientDlg::gradientRepeat() const
{
	return m_gradientRepeat->currentItem();
}

int
VGradientDlg::gradientType() const
{
	return m_gradientType->currentItem();
}

bool
VGradientDlg::gradientFill() const
{
	return m_gradientFill->currentItem() == 1l;
}

void
VGradientDlg::setGradientRepeat( int type )
{
	m_gradientRepeat->setCurrentItem( type );
}

void
VGradientDlg::setGradientType( int type )
{
	m_gradientType->setCurrentItem( type );
}

void
VGradientDlg::setGradientFill( bool b )
{
	m_gradientFill->setCurrentItem( b );
}

void
VGradientDlg::setStartColor( const QColor &c )
{
	m_startColor->setColor( c );
}

void
VGradientDlg::setEndColor( const QColor &c )
{
	m_endColor->setColor( c );
}

#include "vgradientdlg.moc"

