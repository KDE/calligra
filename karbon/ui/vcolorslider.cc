/* This file is part of the KDE project
   Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
   Copyright (C) 2002, The Karbon Developers
*/

/* vcolorslider.cc */

#include <qlabel.h>
#include <qspinbox.h>
#include <kselect.h>
#include <qlayout.h>

#include "vcolorslider.h"

VColorSlider::VColorSlider( QWidget *parent, const char *name )
    : QWidget( parent, name )
{
	init();
}

// Label, left color, right color, min, max, value ...
VColorSlider::VColorSlider( const QString &label, const QColor &col1, const QColor &col2, int min, int max, int value, QWidget *parent, const char *name )
    : QWidget( parent, name )
{
	init();
	setLabel( label );
	setColors( col1, col2 );
	setMinValue( min );
	setMaxValue( max );
	setValue( value );
}

void VColorSlider::init()
{
	QHBoxLayout *layout = new QHBoxLayout( this, 3 );
	mLabel = new QLabel ( this );
	mSelector = new KGradientSelector( KSelector::Horizontal, this );
	mSpinBox = new QSpinBox ( this );
	layout->addWidget( mLabel );
	layout->addWidget( mSelector, 2 );
	layout->addWidget( mSpinBox );
	setValue(0);
	setMinValue(0);
	setMaxValue(255);
	connect( mSpinBox, SIGNAL( valueChanged ( int ) ), this, SLOT( updateFromSpinBox( int ) ) );
	connect( mSelector, SIGNAL( valueChanged ( int ) ), this, SLOT( updateFromSelector( int ) ) );
	layout->activate();
}

// Sets the description of the slider
void VColorSlider::setLabel(const QString &label)
{
	mLabel->setText( label );
}

// Sets the colors for the slider. Color1 is left, Color2 is right
void VColorSlider::setColors( const QColor &color1, const QColor &color2 )
{
	mSelector->setColors( color1, color2 );
}

//Sets the value of the spinbox (and the value of the vcolorslider)
void VColorSlider::setValue( int value )
{
	mSpinBox->setValue( value );
	mSelector->setValue( value );
}

void VColorSlider::setMinValue( int value )
{
	mSpinBox->setMinValue( value );
	mSelector->setMinValue( value );
}

void VColorSlider::setMaxValue( int value )
{
	mSpinBox->setMaxValue( value );
	mSelector->setMaxValue( value );
}

int VColorSlider::value()
{
	return( mSpinBox->value() );
}

void VColorSlider::updateFromSpinBox( int value )
{
	if ( value != mSelector->value() )
	{
		mSelector->setValue( value );
		emit valueChanged( value );
	}
}

void VColorSlider::updateFromSelector( int value )
{
	if ( value != mSpinBox->value() )
	{
		mSpinBox->setValue( value );
		emit valueChanged( value );
	}
}

VColorSlider::~VColorSlider() { }

#include "vcolorslider.moc"
