/* This file is part of the KDE project
   Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
   Copyright (C) 2002, The Karbon Developers
*/

/* vcolorslider.cc */

#include <qlabel.h>
#include <qspinbox.h>
#include <kselect.h>

#include "vcolorslider.h"

VColorSlider::VColorSlider(QWidget *parent, const char *name)
    : QWidget(parent, name)
{
	init();
}

void VColorSlider::init()
{
	mLabel = new QLabel ( this );
	mSpinBox = new QSpinBox ( this );
	mSelector = new KGradientSelector( KSelector::Horizontal, this );
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
void VColorSlider::setValue( int &value )
{
	mSpinBox->setValue ( value );
	mSelector->setValue ( value );
}

void VColorSlider::setMinValue ( int &value )
{
	mSpinBox->setMinValue ( value );
	mSelector->setMinValue ( value );
}

void VColorSlider::setMaxValue ( int &value )
{
	mSpinBox->setMaxValue ( value );
	mSelector->setMaxValue ( value );
}

VColorSlider::~VColorSlider() { }

#include "vcolorslider.moc"
