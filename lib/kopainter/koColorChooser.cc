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
  btnHSV = new KoFrameButton("HSV", this);
  btnHSV->setFixedHeight(16);
  btnHSV->setToggleButton(true);
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

  mColorPatch = new KColorPatch(this);

  mColorSelector = new KHSSelector(this);
  mColorSelector->setFixedHeight(20);

  mRGBWidget = new RGBWidget(this, this);
  mHSVWidget = new HSVWidget(this, this);
  mGreyWidget = new GreyWidget(this, this);

  mGrid->addWidget(btnRGB, 0, 0);
  mGrid->addWidget(btnHSV, 0, 1);
  mGrid->addWidget(btnCMYK, 0, 2);
  mGrid->addWidget(btnLAB, 0, 3);
  mGrid->addWidget(btnGrey, 0, 4);
  mGrid->addMultiCellWidget(mRGBWidget, 1, 1, 1, 4);
  mGrid->addMultiCellWidget(mHSVWidget, 1, 1, 1, 4);
  mGrid->addMultiCellWidget(mGreyWidget, 1, 1, 1, 4);
  mGrid->addWidget(mColorPatch, 1, 0);
  mGrid->addMultiCellWidget(mColorSelector, 2, 2, 0, 4);

  connect(btnRGB, SIGNAL(clicked()), this, SLOT(slotShowRGB()));
  connect(btnHSV, SIGNAL(clicked()), this, SLOT(slotShowHSV()));
  connect(btnCMYK, SIGNAL(clicked()), this, SLOT(slotShowCMYK()));
  connect(btnLAB, SIGNAL(clicked()), this, SLOT(slotShowLAB()));
  connect(btnGrey, SIGNAL(clicked()), this, SLOT(slotShowGrey()));

  connect(mRGBWidget, SIGNAL(colorChanged(const KoColor &)), this, SLOT(slotChangeColor(const KoColor &)));
  connect(this, SIGNAL(colorChanged(const KoColor &)), mRGBWidget, SLOT(slotChangeColor()));
  connect(mHSVWidget, SIGNAL(colorChanged(const KoColor &)), this, SLOT(slotChangeColor(const KoColor &)));
  connect(this, SIGNAL(colorChanged(const KoColor &)), mHSVWidget, SLOT(slotChangeColor()));
  connect(mGreyWidget, SIGNAL(colorChanged(const KoColor &)), this, SLOT(slotChangeColor(const KoColor &)));
  connect(this, SIGNAL(colorChanged(const KoColor &)), mGreyWidget, SLOT(slotChangeColor()));
  connect(mColorPatch, SIGNAL(colorChanged(const QColor &)), this, SLOT(slotChangeColor(const QColor &)));
  connect(mColorSelector, SIGNAL(valueChanged(int, int)), this, SLOT(slotChangeXY(int, int)));

  slotChangeColor(KoColor::black());
  slotShowRGB();
}

void KoColorChooser::slotShowRGB()
{
  mRGBWidget->show();
  mHSVWidget->hide();
  mGreyWidget->hide();
  btnRGB->setOn(true);
  btnHSV->setOn(false);
  btnGrey->setOn(false);
}

void KoColorChooser::slotShowHSV()
{
  mRGBWidget->hide();
  mHSVWidget->show();
  mGreyWidget->hide();
  btnRGB->setOn(false);
  btnHSV->setOn(true);
  btnGrey->setOn(false);
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
  mHSVWidget->hide();
  mGreyWidget->show();
  btnRGB->setOn(false);
  btnHSV->setOn(false);
  btnGrey->setOn(true);
}

void KoColorChooser::slotChangeXY(int h, int s)
{
  slotChangeColor(KoColor(h, s, 192, KoColor::cs_HSV));
}

void KoColorChooser::slotChangeColor(const QColor &c)
{
  slotChangeColor(KoColor(c));
}

void KoColorChooser::slotChangeColor(const KoColor &c)
{
  mColor = c;
  mColorPatch->setColor(mColor.color());
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
  emit colorChanged(KoColor( r, g, b, KoColor::cs_RGB));
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

/*           HSVWidget         */

HSVWidget::HSVWidget(KoColorChooser *aCC, QWidget *parent):
QWidget(parent)
{
  mCC = aCC;

  QGridLayout *mGrid = new QGridLayout(this, 3, 3);
  /* setup color sliders */
  mHSlider = new KoColorSlider(this);
  mHSlider->setMaximumHeight(20);
  mHSlider->slotSetRange(0, 359);

  mSSlider = new KoColorSlider(this);
  mSSlider->setMaximumHeight(20);
  mSSlider->slotSetRange(0, 255);

  mVSlider = new KoColorSlider(this);
  mVSlider->setMaximumHeight(20);
  mVSlider->slotSetRange(0, 255);

  /* setup slider labels */
  mHLabel = new QLabel("H", this);
  mHLabel->setFixedWidth(16);
  mHLabel->setFixedHeight(20);
  mSLabel = new QLabel("S", this);
  mSLabel->setFixedWidth(16);
  mSLabel->setFixedHeight(20);
  mVLabel = new QLabel("V", this);
  mVLabel->setFixedWidth(16);
  mVLabel->setFixedHeight(20);

  /* setup spin box */
  mHIn = new QSpinBox(0, 359, 1, this);
  mHIn->setFixedWidth(42);
  mHIn->setFixedHeight(20);
  mSIn = new QSpinBox(0, 255, 1, this);
  mSIn->setFixedWidth(42);
  mSIn->setFixedHeight(20);
  mVIn = new QSpinBox(0, 255, 1, this);
  mVIn->setFixedWidth(42);
  mVIn->setFixedHeight(20);

  mGrid->addWidget(mHLabel, 0, 0);
  mGrid->addWidget(mSLabel, 1, 0);
  mGrid->addWidget(mVLabel, 2, 0);
  mGrid->addWidget(mHSlider, 0, 1);
  mGrid->addWidget(mSSlider, 1, 1);
  mGrid->addWidget(mVSlider, 2, 1);
  mGrid->addWidget(mHIn, 0, 2);
  mGrid->addWidget(mSIn, 1, 2);
  mGrid->addWidget(mVIn, 2, 2);

  setFixedHeight(60);

  /* connect color sliders */
  connect(mHSlider, SIGNAL(valueChanged(int)), this, SLOT(slotHSliderChanged(int)));
  connect(mSSlider, SIGNAL(valueChanged(int)), this, SLOT(slotSSliderChanged(int)));
  connect(mVSlider, SIGNAL(valueChanged(int)), this, SLOT(slotVSliderChanged(int)));

  /* connect spin box */
  connect(mHIn, SIGNAL(valueChanged(int)), this, SLOT(slotHInChanged(int)));
  connect(mSIn, SIGNAL(valueChanged(int)), this, SLOT(slotSInChanged(int)));
  connect(mVIn, SIGNAL(valueChanged(int)), this, SLOT(slotVInChanged(int)));
}

void HSVWidget::slotChangeColor()
{
  int h = mCC->color().H();
  int s = mCC->color().S();
  int v = mCC->color().V();

  mHSlider->slotSetColor1(KoColor(0, s, v, KoColor::cs_HSV).color());
  mHSlider->slotSetColor2(KoColor(359, s, v, KoColor::cs_HSV).color());
  mHSlider->slotSetValue(h);
  mHIn->setValue(h);

  mSSlider->slotSetColor1(KoColor(h, 0, v, KoColor::cs_HSV).color());
  mSSlider->slotSetColor2(KoColor(h, 255, v, KoColor::cs_HSV).color());
  mSSlider->slotSetValue(s);
  mSIn->setValue(s);

  mVSlider->slotSetColor1(KoColor(h, s, 0, KoColor::cs_HSV).color());
  mVSlider->slotSetColor2(KoColor(h, s, 255, KoColor::cs_HSV).color());
  mVSlider->slotSetValue(v);
  mVIn->setValue(v);
}
  
void HSVWidget::slotHSliderChanged(int h)
{
  int s = mCC->color().S();
  int v = mCC->color().V();
  emit colorChanged(KoColor(h, s, v, KoColor::cs_HSV));
}

void HSVWidget::slotSSliderChanged(int s)
{
  int h = mCC->color().H();
  int v = mCC->color().V();
  emit colorChanged(KoColor(h, s, v, KoColor::cs_HSV));
}

void HSVWidget::slotVSliderChanged(int v)
{
  int h = mCC->color().H();
  int s = mCC->color().S();
  emit colorChanged(KoColor(h, s, v, KoColor::cs_HSV));
}

void HSVWidget::slotHInChanged(int h)
{
  int s = mCC->color().S();
  int v = mCC->color().V();
  emit colorChanged(KoColor(h, s, v, KoColor::cs_HSV));
}

void HSVWidget::slotSInChanged(int s)
{
  int h = mCC->color().H();
  int v = mCC->color().V();
  emit colorChanged(KoColor(h, s, v, KoColor::cs_HSV));
}

void HSVWidget::slotVInChanged(int v)
{
  int h = mCC->color().H();
  int s = mCC->color().S();
  emit colorChanged(KoColor(h, s, v, KoColor::cs_HSV));
}

/*          GreyWidget         */

GreyWidget::GreyWidget(KoColorChooser *aCC, QWidget *parent):
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
