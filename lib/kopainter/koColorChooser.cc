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

KoColorChooser::KoColorChooser(QWidget *parent, const char *name):
QWidget(parent, name)
{
  mGrid = new QGridLayout(this, 3, 5);

  btnRGB = new KoFrameButton("RGB", this);
  btnRGB->setFixedHeight(16);
  btnRGB->setToggleButton(true);
  btnHSB = new KoFrameButton("HSB", this);
  btnHSB->setFixedHeight(16);
  btnHSB->setToggleButton(true);
  btnHSB->setEnabled(false);
  btnCMYK = new KoFrameButton("CMYK", this);
  btnCMYK->setFixedHeight(16);
  btnCMYK->setToggleButton(true);
  btnCMYK->setEnabled(false);
  btnLAB = new KoFrameButton("LAB", this);
  btnLAB->setFixedHeight(16);
  btnLAB->setToggleButton(true);
  btnLAB->setEnabled(false);
  btnGrey = new KoFrameButton("Grey", this);
  btnGrey->setFixedHeight(16);
  btnGrey->setToggleButton(true);

  clr_patch = new KColorPatch(this);

  KoColorFrame *fff = new KoColorFrame(this);   //Temp widget (will be changed to palette)
  fff->setFixedHeight(20);
  fff->slotSetColor1(red);
  fff->slotSetColor2(green);

  mRGBWidget = new RGBWidget(this, this);
  mGreyWidget = new GreyWidget(this, this);

  mGrid->addWidget(btnRGB, 0, 0);
  mGrid->addWidget(btnHSB, 0, 1);
  mGrid->addWidget(btnCMYK, 0, 2);
  mGrid->addWidget(btnLAB, 0, 3);
  mGrid->addWidget(btnGrey, 0, 4);
  mGrid->addMultiCellWidget(mRGBWidget, 1, 1, 1, 4);
  mGrid->addMultiCellWidget(mGreyWidget, 1, 1, 1, 4);
  mGrid->addWidget(clr_patch, 1, 0);
  mGrid->addMultiCellWidget(fff, 2, 2, 0, 4);

  connect(btnRGB, SIGNAL(clicked()), this, SLOT(slotShowRGB()));
  connect(btnHSB, SIGNAL(clicked()), this, SLOT(slotShowHSB()));
  connect(btnCMYK, SIGNAL(clicked()), this, SLOT(slotShowCMYK()));
  connect(btnLAB, SIGNAL(clicked()), this, SLOT(slotShowLAB()));
  connect(btnGrey, SIGNAL(clicked()), this, SLOT(slotShowGrey()));

  connect(mRGBWidget, SIGNAL(colorChanged(const KoColor &)), this, SLOT(slotChangeColor(const KoColor &)));
  connect(this, SIGNAL(colorChanged(const KoColor &)), mRGBWidget, SLOT(slotChangeColor()));
  connect(mGreyWidget, SIGNAL(colorChanged(const KoColor &)), this, SLOT(slotChangeColor(const KoColor &)));
  connect(this, SIGNAL(colorChanged(const KoColor &)), mGreyWidget, SLOT(slotChangeColor()));
  connect(clr_patch, SIGNAL(colorChanged(const QColor &)), this, SLOT(slotChangeColor(const QColor &)));

  slotChangeColor(KoColor::black());
  slotShowRGB();
}

void KoColorChooser::slotShowRGB()
{
  mRGBWidget->show();
  mGreyWidget->hide();
  btnRGB->setOn(true);
  btnGrey->setOn(false);
}

void KoColorChooser::slotShowHSB()
{
  mRGBWidget->hide();
  mGreyWidget->hide();
}

void KoColorChooser::slotShowCMYK()
{
  mRGBWidget->hide();
  mGreyWidget->hide();
}

void KoColorChooser::slotShowLAB()
{
  mRGBWidget->hide();
  mGreyWidget->hide();
}

void KoColorChooser::slotShowGrey()
{
  mRGBWidget->hide();
  mGreyWidget->show();
  btnRGB->setOn(false);
  btnGrey->setOn(true);
}

void KoColorChooser::slotChangeColor(const QColor &c)
{
  slotChangeColor(KoColor(c));
}

void KoColorChooser::slotChangeColor(const KoColor &c)
{
  mColor = c;
  clr_patch->setColor(mColor.color());
  emit colorChanged(mColor);
}

/*           RGBWidget         */

RGBWidget::RGBWidget(KoColorChooser *aCC, QWidget *parent):
QWidget(parent)
{
  mCC = aCC;

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

  setFixedHeight(60);

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
  emit colorChanged(KoColor(r, g, b, KoColor::cs_RGB));
}

void RGBWidget::slotGSliderChanged(int g)
{
  int r = mCC->color().R();
  int b = mCC->color().B();
  emit colorChanged(KoColor( r, g, b, KoColor::cs_RGB ));
}

void RGBWidget::slotBSliderChanged(int b)
{
  int r = mCC->color().R();
  int g = mCC->color().G();
  emit colorChanged(KoColor(r, g, b, KoColor::cs_RGB));
}

void RGBWidget::slotRInChanged(int r)
{
  int g = mCC->color().G();
  int b = mCC->color().B();
  emit colorChanged(KoColor(r, g, b, KoColor::cs_RGB));
}

void RGBWidget::slotGInChanged(int g)
{
  int r = mCC->color().R();
  int b = mCC->color().B();
  emit colorChanged(KoColor(r, g, b, KoColor::cs_RGB));
}

void RGBWidget::slotBInChanged(int b)
{
  int r = mCC->color().R();
  int g = mCC->color().G();
  emit colorChanged(KoColor(r, g, b, KoColor::cs_RGB));
}

/*          GreyWidget         */

GreyWidget::GreyWidget(KoColorChooser *aCC, QWidget *parent = 0L):
QWidget(parent)
{
  mCC = aCC;

  QGridLayout *mGrid = new QGridLayout(this, 1, 3);
  /* setup slider */
  mVSlider = new KoColorSlider(this);
  mVSlider->setMaximumHeight(20);
  mVSlider->slotSetRange(0, 255);
  mVSlider->slotSetColor1(QColor(255, 255, 255));
  mVSlider->slotSetColor2(QColor(0, 0, 0));

  /* setup slider label */
  mVLabel = new QLabel("K", this);
  mVLabel->setFixedWidth(18);
  mVLabel->setFixedHeight(20);

  /* setup spin box */
  mVIn = new QSpinBox(0, 255, 1, this);
  mVIn->setFixedWidth(42);
  mVIn->setFixedHeight(20);

  mGrid->addWidget(mVLabel, 0, 0);
  mGrid->addWidget(mVSlider, 0, 1);
  mGrid->addWidget(mVIn, 0, 2);

  setFixedHeight(60);

  /* connect color slider */
  connect(mVSlider, SIGNAL(valueChanged(int)), this, SLOT(slotVSliderChanged(int)));

  /* connect spin box */
  connect(mVIn, SIGNAL(valueChanged(int)), mVSlider, SLOT(slotSetValue(int)));
}

void GreyWidget::slotChangeColor()
{
  double v = mCC->color().R() + mCC->color().G() + mCC->color().B();
  v /= 3.0;
  v = 255.0 - v;
  mVIn->setValue(static_cast<int>(v));
  mVSlider->slotSetValue(static_cast<int>(v));
}
  
void GreyWidget::slotVSliderChanged(int v)
{
  v = 255 - v;
  emit colorChanged(KoColor(v, v, v, KoColor::cs_RGB));
}

void GreyWidget::slotVInChanged(int v)
{
  v = 255 - v;
  emit colorChanged(KoColor(v, v, v, KoColor::cs_RGB));
}

#include "koColorChooser.moc"
