/*
 *  integerwidget.cc - part of KImageShop
 *
 *  A convenience widget for setting integer values
 *  Consists of a SpinBox and a slider
 *
 *  Copyright (c) 1999 Carsten Pfeiffer <pfeiffer@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <qlayout.h>
#include <qspinbox.h>

#include "integerwidget.h"


IntegerWidget::IntegerWidget( int min, int max, QWidget *parent,
			    const char *name )
  : QWidget( parent, name )
{
  spinBox = new QSpinBox( min, max, 1, this, "spinbox" );
  slider = new QSlider( min, max, 1, min, QSlider::Horizontal, this, "sld" );

  connect( slider, SIGNAL( valueChanged(int) ), spinBox, SLOT( setValue(int)));
  connect( spinBox, SIGNAL( valueChanged(int) ),
	   this, SLOT( setSliderValue(int) ));

  layout = 0L;
  initGUI();
}


IntegerWidget::~IntegerWidget()
{
  delete spinBox;
  delete slider;
  delete layout;
}


// the current set value
int IntegerWidget::value()
{
  return spinBox->value();
}


// set the value
void IntegerWidget::setValue( int value )
{
  slider->setValue( value );
}


// set the range, the widget should cover
void IntegerWidget::setRange( int min, int max )
{
  spinBox->setRange( min, max );
  slider->setRange( min, max );
}


// important - there must be an easy way the get straight focus to the 
// editwidget, to quickly set the value via keyboard
void IntegerWidget::setEditFocus()
{
  spinBox->setFocus();
  //  spinBox->selectAll(); // does not work, yet, need to subclass QSpinBox
}


// set where to paint tickmarks for the slider
void IntegerWidget::setTickmarks( QSlider::TickSetting s )
{
  slider->setTickmarks( s );
}

void IntegerWidget::setTickInterval( int value )
{
  slider->setTickInterval( value );
}

int IntegerWidget::tickInterval() const
{
  return slider->tickInterval();
}



void IntegerWidget::initGUI()
{
  if ( layout )
    return;

  layout = new QHBoxLayout( this, 5, -1, "hbox layout" );
  layout->addWidget( spinBox );
  layout->addWidget( slider );
}


// update the slider position when the spinbox value has changed and emit
// the new value
void IntegerWidget::setSliderValue( int value )
{
  slider->setValue( value );
  emit valueChanged( value );
}



#include "integerwidget.moc"
