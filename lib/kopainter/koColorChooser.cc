/* This file is part of the KDE project
  Copyright (c) 1999 Matthias Elter <me@kde.org>
  Copyright (c) 2001 Igor Janssen <rm@linux.ru.net>

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

#include "koColorChooser.h"

#include <qlayout.h>
#include <qspinbox.h>

#include <klocale.h>
#include <kiconloader.h>
#include <kcolordialog.h>
#include <koFrameButton.h>
#include <koColorSlider.h>
#include <kdebug.h>

KoColorChooser::KoColorChooser(QWidget *parent, const char *name):
QWidget(parent, name)
{
  mGrid = new QGridLayout(this, 2, 5);
  btn_RGB = new KoFrameButton("RGB", this);
  btn_CMYK = new KoFrameButton("CMYK", this);
  btn_Grey = new KoFrameButton("Grey", this);
  btn_HSB = new KoFrameButton("HSB", this);
  btn_LAB = new KoFrameButton("LAB", this);
  clr_patch = new KColorPatch(this);
  mRGBWidget = new RGBWidget(this, this);
  btn_RGB->setMaximumHeight(20);
  btn_CMYK->setMaximumHeight(20);
  btn_Grey->setMaximumHeight(20);
  btn_HSB->setMaximumHeight(20);
  btn_LAB->setMaximumHeight(20);
  mGrid->addWidget(btn_RGB, 0, 0);
  mGrid->addWidget(btn_CMYK, 0, 1);
  mGrid->addWidget(btn_Grey, 0, 2);
  mGrid->addWidget(btn_HSB, 0, 3);
  mGrid->addWidget(btn_LAB, 0, 4);
  mGrid->addMultiCellWidget(mRGBWidget, 1, 1, 1, 4);
  mGrid->addWidget(clr_patch, 1, 0);
  
  connect(mRGBWidget, SIGNAL(colorChanged(const KoColor &)), this, SLOT(slotChangeColor(const KoColor &)));
  connect(this, SIGNAL(colorChanged(const KoColor &)), mRGBWidget, SLOT(slotChangeColor()));
  //TODO D&D color to KColorPatch
  slotChangeColor(KoColor::black());
}

void KoColorChooser::slotChangeColor(const KoColor &c)
{
  mColor = c;
  clr_patch->setColor(mColor.color());
  emit colorChanged(mColor);
}

RGBWidget::RGBWidget(KoColorChooser *aCC, QWidget *parent):
QWidget(parent)
{
  mCC = aCC;

  kdDebug() << "RGB WIDGET!!!" << parent << endl;

  QGridLayout *mGrid = new QGridLayout(this, 3, 3);
  /* setup color sliders */
  mRSlider = new KoColorSlider(this);
  mRSlider->setMaximumHeight(20);
  mRSlider->slotSetRange(0, 255);

  mGSlider = new KoColorSlider(this);
  mGSlider->setMaximumHeight(20);
  mGSlider->slotSetRange(0, 255);

  mBSlider = new KoColorSlider(this);
  mBSlider->setMaximumHeight(20);
  mBSlider->slotSetRange(0, 255);

  /* setup slider labels */
  mRLabel = new QLabel("R", this);
  mRLabel->setFixedWidth(16);
  mRLabel->setFixedHeight(20);
  mGLabel = new QLabel("G", this);
  mGLabel->setFixedWidth(16);
  mGLabel->setFixedHeight(20);
  mBLabel = new QLabel("B", this);
  mBLabel->setFixedWidth(16);
  mBLabel->setFixedHeight(20);

  /* setup spin box */
  mRIn = new QSpinBox(0, 255, 1, this);
  mRIn->setFixedWidth(42);
  mRIn->setFixedHeight(20);
  mGIn = new QSpinBox(0, 255, 1, this);
  mGIn->setFixedWidth(42);
  mGIn->setFixedHeight(20);
  mBIn = new QSpinBox(0, 255, 1, this);
  mBIn->setFixedWidth(42);
  mBIn->setFixedHeight(20);

  mGrid->addWidget(mRLabel, 0, 0);
  mGrid->addWidget(mGLabel, 1, 0);
  mGrid->addWidget(mBLabel, 2, 0);
  mGrid->addWidget(mRSlider, 0, 1);
  mGrid->addWidget(mGSlider, 1, 1);
  mGrid->addWidget(mBSlider, 2, 1);
  mGrid->addWidget(mRIn, 0, 2);
  mGrid->addWidget(mGIn, 1, 2);
  mGrid->addWidget(mBIn, 2, 2);

  /* connect color sliders */
  connect(mRSlider, SIGNAL(valueChanged(int)), this, SLOT(slotRSliderChanged(int)));
  connect(mGSlider, SIGNAL(valueChanged(int)), this, SLOT(slotGSliderChanged(int)));
  connect(mBSlider, SIGNAL(valueChanged(int)), this, SLOT(slotBSliderChanged(int)));

  /* connect spin box */
  connect(mRIn, SIGNAL(valueChanged(int)), this, SLOT(slotRInChanged(int)));
  connect(mGIn, SIGNAL(valueChanged(int)), this, SLOT(slotGInChanged(int)));
  connect(mBIn, SIGNAL(valueChanged(int)), this, SLOT(slotBInChanged(int)));
}

void RGBWidget::slotChangeColor()
{
  int r = mCC->color().R();
  int g = mCC->color().G();
  int b = mCC->color().B();

  mRSlider->slotSetColor1(QColor(0, g, b));
  mRSlider->slotSetColor2(QColor(255, g, b));
  mRSlider->slotSetValue(r);
  mRIn->setValue(r);

  mGSlider->slotSetColor1(QColor(r, 0, b));
  mGSlider->slotSetColor2(QColor(r, 255, b));
  mGSlider->slotSetValue(g);
  mGIn->setValue(g);

  mBSlider->slotSetColor1(QColor(r, g, 0));
  mBSlider->slotSetColor2(QColor(r, g, 255));
  mBSlider->slotSetValue(b);
  mBIn->setValue(b);
}
  
void RGBWidget::slotRSliderChanged(int r)
{
  int g = mCC->color().G();
  int b = mCC->color().B();
/*  mGSlider->slotSetColor1( QColor( r, 0, b ) );
  mGSlider->slotSetColor2( QColor( r, 255, b ) );

  mBSlider->slotSetColor1( QColor( r, g, 0 ) );
  mBSlider->slotSetColor2( QColor( r, g, 255 ) );

  mRIn->setValue( r );*/

  emit colorChanged(KoColor(r, g, b, KoColor::cs_RGB));
}

void RGBWidget::slotGSliderChanged(int g)
{
  int r = mCC->color().R();
  int b = mCC->color().B();
/*  mRSlider->slotSetColor1( QColor( 0, g, b ) );
  mRSlider->slotSetColor2( QColor( 255, g, b ) );

  mBSlider->slotSetColor1( QColor( r, g, 0 ) );
  mBSlider->slotSetColor2( QColor( r, g, 255 ) );

  mGIn->setValue( g );*/
  emit colorChanged(KoColor( r, g, b, KoColor::cs_RGB ));
}

void RGBWidget::slotBSliderChanged(int b)
{
  int r = mCC->color().R();
  int g = mCC->color().G();
/*  mRSlider->slotSetColor1( QColor( 0, g, b ) );
  mRSlider->slotSetColor2( QColor( 255, g, b ) );

  mGSlider->slotSetColor1( QColor( r, 0, b ) );
  mGSlider->slotSetColor2( QColor( r, 255, b ) );

  mBIn->setValue( b );*/
  emit colorChanged(KoColor(r, g, b, KoColor::cs_RGB));
}

void RGBWidget::slotRInChanged(int r)
{
  int g = mCC->color().G();
  int b = mCC->color().B();
/*  mGSlider->slotSetColor1( QColor( r, 0, b ) );
  mGSlider->slotSetColor2( QColor( r, 255, b ) );

  mBSlider->slotSetColor1( QColor( r, g, 0 ) );
  mBSlider->slotSetColor2( QColor( r, g, 255 ) );

  mRSlider->slotSetValue( r );*/
  emit colorChanged(KoColor(r, g, b, KoColor::cs_RGB));
}

void RGBWidget::slotGInChanged(int g)
{
  int r = mCC->color().R();
  int b = mCC->color().B();
/*  mRSlider->slotSetColor1( QColor( 0, g, b ) );
  mRSlider->slotSetColor2( QColor( 255, g, b ) );

  mBSlider->slotSetColor1( QColor( r, g, 0 ) );
  mBSlider->slotSetColor2( QColor( r, g, 255 ) );

  mGSlider->slotSetValue( g );*/
  emit colorChanged(KoColor(r, g, b, KoColor::cs_RGB));
}

void RGBWidget::slotBInChanged(int b)
{
  int r = mCC->color().R();
  int g = mCC->color().G();
/*  mRSlider->slotSetColor1( QColor( 0, g, b ) );
  mRSlider->slotSetColor2( QColor( 255, g, b ) );

  mGSlider->slotSetColor1( QColor( r, 0, b ) );
  mGSlider->slotSetColor2( QColor( r, 255, b ) );

  mBSlider->slotSetValue( b );*/
  emit colorChanged(KoColor(r, g, b, KoColor::cs_RGB));
}

#include "koColorChooser.moc"
