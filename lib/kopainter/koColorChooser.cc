/* -*- C++ -*-


  This file is part of KIllustrator.
  Copyright (C) 2000 Igor Janssen (rm@linux.ru.net)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as
  published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include "koColorChooser.h"

#include <koFrameButton.h>
#include <qlayout.h>
#include <qspinbox.h>
#include <kdebug.h>
#include <koColorSlider.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kcolordialog.h>

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
  clr = new RGBWidget(this);
  btn_RGB->setMaximumHeight(20);
  btn_CMYK->setMaximumHeight(20);
  btn_Grey->setMaximumHeight(20);
  btn_HSB->setMaximumHeight(20);
  btn_LAB->setMaximumHeight(20);
  mGrid->addWidget( btn_RGB, 0, 0 );
  mGrid->addMultiCellWidget( clr,1, 1, 1, 4 );
  mGrid->addWidget( btn_RGB, 0, 0 );
  mGrid->addWidget( btn_CMYK, 0, 1 );
  mGrid->addWidget( btn_Grey, 0, 2 );
  mGrid->addWidget( btn_HSB, 0, 3 );
  mGrid->addWidget( btn_LAB, 0, 4 );
  mGrid->addWidget( clr_patch, 1, 0 );

  setFixedSize(200, 100);

  connect(clr, SIGNAL(colorChanged(const KoColor&)), this, SLOT(slotChangeColor(const KoColor&)));
}

void KoColorChooser::slotChangeColor(const KoColor& c)
 {
  clr_patch->setColor(c.color());
  emit colorChanged(c);
 }

RGBWidget::RGBWidget(QWidget *parent) : QWidget(parent)
{
  // init with defaults
  m_c = KoColor::white();

  // setup color sliders
  m_pRSlider = new KoColorSlider(this);
  m_pRSlider->setMaximumHeight(20);
  m_pRSlider->slotSetRange(0, 255);

  m_pGSlider = new KoColorSlider(this);
  m_pGSlider->setMaximumHeight(20);
  m_pGSlider->slotSetRange(0, 255);

  m_pBSlider = new KoColorSlider(this);
  m_pBSlider->setMaximumHeight(20);
  m_pBSlider->slotSetRange(0, 255);
 
  // setup slider labels
  m_pRLabel = new QLabel("R", this);
  m_pRLabel->setFixedWidth(16);
  m_pRLabel->setFixedHeight(20);
  m_pGLabel = new QLabel("G", this);
  m_pGLabel->setFixedWidth(16);
  m_pGLabel->setFixedHeight(20);
  m_pBLabel = new QLabel("B", this);
  m_pBLabel->setFixedWidth(16);
  m_pBLabel->setFixedHeight(20);
 
  // setup spin box
  m_pRIn = new QSpinBox(0, 255, 1, this);
  m_pRIn->setFixedWidth(42);
  m_pRIn->setFixedHeight(20);
  m_pGIn = new QSpinBox(0, 255, 1, this);
  m_pGIn->setFixedWidth(42);
  m_pGIn->setFixedHeight(20);
  m_pBIn = new QSpinBox(0, 255, 1, this);
  m_pBIn->setFixedWidth(42);
  m_pBIn->setFixedHeight(20);

  // connect color sliders
  connect(m_pRSlider, SIGNAL(valueChanged(int)), this,
		  SLOT(slotRSliderChanged(int)));
  connect(m_pGSlider, SIGNAL(valueChanged(int)), this,
		  SLOT(slotGSliderChanged(int)));
  connect(m_pBSlider, SIGNAL(valueChanged(int)), this,
		  SLOT(slotBSliderChanged(int)));

  // connect spin box
  connect(m_pRIn, SIGNAL(valueChanged (int)), this,
		  SLOT(slotRInChanged(int)));
  connect(m_pGIn, SIGNAL(valueChanged (int)), this,
		  SLOT(slotGInChanged(int)));
  connect(m_pBIn, SIGNAL(valueChanged (int)), this,
		  SLOT(slotBInChanged(int)));
}

void RGBWidget::resizeEvent(QResizeEvent *)
{
  // I know a QGridLayout would look nicer,
  // but it does not use the space as good as I want it to.

  int y1 = height()/3;
  int y2 = 2 * y1;

  int labelY =(y1 - m_pRLabel->height())/2 - 4;
  if (labelY < 0) 
	labelY = 0;

  m_pRLabel->move(2, 0 + labelY);
  m_pGLabel->move(2, y1 + labelY);
  m_pBLabel->move(2, y2 + labelY);

  int x1 = m_pRLabel->pos().x() + m_pRLabel->width();

  int inY =(y1 - m_pRIn->height())/2 - 4;
  if (inY < 0) 
	inY = 0;

  m_pRIn->move(width() - m_pRIn->width(), 0 + inY);
  m_pGIn->move(width() - m_pRIn->width(), y1 + inY);
  m_pBIn->move(width() - m_pRIn->width(), y2 + inY);

  int x2 = width() - m_pRIn->width() - 2;

  m_pRSlider->resize(QSize(x2 - x1, y1));
  m_pGSlider->resize(QSize(x2 - x1, y1));
  m_pBSlider->resize(QSize(x2 - x1, y1));

  m_pRSlider->move(x1, 0 + (y1 - m_pRSlider->height())/2);
  m_pGSlider->move(x1, y1 + (y1 - m_pRSlider->height())/2);
  m_pBSlider->move(x1, y2 + (y1 - m_pRSlider->height())/2);
}

void RGBWidget::slotSetColor(const KoColor&c)
{
  m_c = c;
  int r = c.R();
  int g = c.G();
  int b = c.B();

  m_pRSlider->slotSetColor1( QColor( 0, g, b ) );
  m_pRSlider->slotSetColor2( QColor( 255, g, b ) );
  m_pRSlider->slotSetValue( r );
  m_pRIn->setValue( r );

  m_pGSlider->slotSetColor1( QColor( r, 0, b ) );
  m_pGSlider->slotSetColor2( QColor( r, 255, b ) );
  m_pGSlider->slotSetValue( g );
  m_pGIn->setValue( g );

  m_pBSlider->slotSetColor1( QColor( r, g, 0 ) );
  m_pBSlider->slotSetColor2( QColor( r, g, 255 ) );
  m_pBSlider->slotSetValue( b );
  m_pBIn->setValue( b );
}
  
void RGBWidget::slotRSliderChanged(int r)
{
  int g = m_c.G();
  int b = m_c.B();

  m_c = KoColor( r, g, b, KoColor::cs_RGB );

  m_pGSlider->slotSetColor1( QColor( r, 0, b ) );
  m_pGSlider->slotSetColor2( QColor( r, 255, b ) );

  m_pBSlider->slotSetColor1( QColor( r, g, 0 ) );
  m_pBSlider->slotSetColor2( QColor( r, g, 255 ) );

  m_pRIn->setValue( r );

  emit colorChanged(m_c);
}

void RGBWidget::slotGSliderChanged(int g)
{
  int r = m_c.R();
  int b = m_c.B();

  m_c = KoColor( r, g, b, KoColor::cs_RGB );

  m_pRSlider->slotSetColor1( QColor( 0, g, b ) );
  m_pRSlider->slotSetColor2( QColor( 255, g, b ) );

  m_pBSlider->slotSetColor1( QColor( r, g, 0 ) );
  m_pBSlider->slotSetColor2( QColor( r, g, 255 ) );

  m_pGIn->setValue( g );

  emit colorChanged(m_c);
}

void RGBWidget::slotBSliderChanged(int b)
{
  int r = m_c.R();
  int g = m_c.G();

  m_c = KoColor( r, g, b, KoColor::cs_RGB );

  m_pRSlider->slotSetColor1( QColor( 0, g, b ) );
  m_pRSlider->slotSetColor2( QColor( 255, g, b ) );

  m_pGSlider->slotSetColor1( QColor( r, 0, b ) );
  m_pGSlider->slotSetColor2( QColor( r, 255, b ) );

  m_pBIn->setValue( b );

  emit colorChanged(m_c);
}

void RGBWidget::slotRInChanged(int r)
{
  int g = m_c.G();
  int b = m_c.B();

  m_c = KoColor( r, g, b, KoColor::cs_RGB );

  m_pGSlider->slotSetColor1( QColor( r, 0, b ) );
  m_pGSlider->slotSetColor2( QColor( r, 255, b ) );

  m_pBSlider->slotSetColor1( QColor( r, g, 0 ) );
  m_pBSlider->slotSetColor2( QColor( r, g, 255 ) );

  m_pRSlider->slotSetValue( r );

  emit colorChanged(m_c);
}

void RGBWidget::slotGInChanged(int g)
{
  int r = m_c.R();
  int b = m_c.B();

  m_c = KoColor( r, g, b, KoColor::cs_RGB );

  m_pRSlider->slotSetColor1( QColor( 0, g, b ) );
  m_pRSlider->slotSetColor2( QColor( 255, g, b ) );

  m_pBSlider->slotSetColor1( QColor( r, g, 0 ) );
  m_pBSlider->slotSetColor2( QColor( r, g, 255 ) );

  m_pGSlider->slotSetValue( g );

  emit colorChanged(m_c);
}

void RGBWidget::slotBInChanged(int b)
{
  int r = m_c.R();
  int g = m_c.G();

  m_c = KoColor( r, g, b, KoColor::cs_RGB );

  m_pRSlider->slotSetColor1( QColor( 0, g, b ) );
  m_pRSlider->slotSetColor2( QColor( 255, g, b ) );

  m_pGSlider->slotSetColor1( QColor( r, 0, b ) );
  m_pGSlider->slotSetColor2( QColor( r, 255, b ) );

  m_pBSlider->slotSetValue( b );

  emit colorChanged(m_c);
}

#include "koColorChooser.moc"
