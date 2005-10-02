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

/* vcolorslider.cc */

#include <qlayout.h>
#include <qlabel.h>
#include <knuminput.h>
#include <kselect.h>

#include "vcolorslider.h"

VColorSlider::VColorSlider( QWidget* parent, const char* name )
	: QWidget( parent, name )
{
	init();
}

// Label, left color, right color, min, max, value ...
VColorSlider::VColorSlider( const QString& label, const QColor& col1,
	const QColor& col2, int min, int max, int value, QWidget* parent, const char* name )
		: QWidget( parent, name )
{
	init();
	setLabel( label );
	setColors( col1, col2 );
	setMinValue( min );
	setMaxValue( max );
	setValue( value );
}

VColorSlider::~VColorSlider()
{
}

void VColorSlider::init()
{
	QHBoxLayout *layout = new QHBoxLayout( this, 3 );

	m_label = new QLabel( this );
	m_gradientSelect = new KGradientSelector( KSelector::Horizontal, this );
	m_spinBox = new KIntSpinBox( this );

	layout->addWidget( m_label );
	layout->addWidget( m_gradientSelect, 2 );
	layout->addWidget( m_spinBox );

	setValue( 0 );
	setMinValue( 0 );
	setMaxValue( 255 );

	connect( m_spinBox, SIGNAL( valueChanged ( int ) ), this, SLOT( updateFrom_spinBox( int ) ) );
	connect( m_gradientSelect, SIGNAL( valueChanged ( int ) ), this, SLOT( updateFrom_gradientSelect( int ) ) );

	layout->activate();
}

// Sets the description of the slider
void VColorSlider::setLabel( const QString& label )
{
	m_label->setText( label );
}

// Sets the colors for the slider. Color1 is left, Color2 is right
void VColorSlider::setColors( const QColor& color1, const QColor& color2 )
{
	m_gradientSelect->setColors( color1, color2 );
}

//Sets the value of the spinbox (and the value of the vcolorslider)
void VColorSlider::setValue( int value )
{
	m_spinBox->setValue( value );
	m_gradientSelect->setValue( value );
}

void VColorSlider::setMinValue( int value )
{
	m_spinBox->setMinValue( value );
	m_gradientSelect->setMinValue( value );
}

void VColorSlider::setMaxValue( int value )
{
	m_spinBox->setMaxValue( value );
	m_gradientSelect->setMaxValue( value );
}

int VColorSlider::value()
{
	return( m_spinBox->value() );
}

void VColorSlider::updateFrom_spinBox( int value )
{
	if ( value != m_gradientSelect->value() )
	{
		m_gradientSelect->setValue( value );
		emit valueChanged( value );
	}
}

void VColorSlider::updateFrom_gradientSelect( int value )
{
	if ( value != m_spinBox->value() )
	{
		m_spinBox->setValue( value );
		emit valueChanged( value );
	}
}

#include "vcolorslider.moc"

