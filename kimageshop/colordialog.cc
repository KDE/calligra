/*
 *  colordialog.cc - part of KImageShop
 *
 *  Copyright (c) 1999 Matthias Elter  <me@kde.org>
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

#include <qframe.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qpainter.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qspinbox.h>

#include <klocale.h>
#include <kdebug.h>

#include "kdualcolorbtn.h"
#include "colorframe.h"
#include "colorslider.h"
#include "colordialog.h"

ColorDialog::ColorDialog(QWidget *parent) : KFloatingDialog(parent)
{
  setCaption(i18n("Color chooser"));
  resize(280, 190);
  setMinimumWidth(280);
  setMinimumHeight(190);
  m_pBase = new ColorChooserWidget(this);
  setBaseWidget(m_pBase);
}

ColorDialog::~ColorDialog()
{
  delete m_pBase;
}

ColorChooserWidget::ColorChooserWidget(QWidget *parent) : QWidget(parent)
{
  m_pRGBWidget = new RGBWidget(this);
  m_pColorFrame = new ColorFrame(this);
  m_pColorButton = new KDualColorButton(this);
  
  m_pGrayButton = new QPushButton("Gray", this);
  m_pRGBButton = new QPushButton("RGB", this);
  m_pHSBButton = new QPushButton("HSB", this);
  m_pCMYKButton = new QPushButton("CMYK", this);
  m_pLABButton = new QPushButton("LAB", this);

  m_fg = KColor(0,0,0);;
  m_bg = KColor(255,255,255);

  m_pRGBWidget->slotSetColor(QColor(0,0,0));

  connect(m_pColorButton, SIGNAL(fgChanged(const QColor &)), m_pColorFrame, SLOT(slotSetColor1(const QColor &)));
  connect(m_pColorButton, SIGNAL(bgChanged(const QColor &)), m_pColorFrame, SLOT(slotSetColor2(const QColor &)));
  connect(m_pColorFrame, SIGNAL(colorSelected(const QColor &)), m_pColorButton, SLOT(slotSetForeground(const QColor &)));
}

ColorChooserWidget::~ColorChooserWidget()
{
  delete m_pRGBWidget;
  delete m_pColorFrame;
  delete m_pColorButton;
  delete m_pGrayButton;
  delete m_pRGBButton;
  delete m_pHSBButton;
  delete m_pCMYKButton;
  delete m_pLABButton;
}

void ColorChooserWidget::resizeEvent(QResizeEvent *e)
{
  // color model buttons
  int w = width();
  int h = height();
  m_pLABButton->setGeometry(w-32, 1, 30, 18);
  m_pCMYKButton->setGeometry(w-72, 1, 40, 18);
  m_pHSBButton->setGeometry(w-102, 1, 30, 18);
  m_pRGBButton->setGeometry(w-132, 1, 30, 18);
  m_pGrayButton->setGeometry(w-162, 1, 30, 18);
  
  m_pColorButton->setGeometry(2, 5, 40, 40);
  m_pColorFrame->setGeometry(2, h-24, w-4, 22);
  m_pRGBWidget->setGeometry(44,22,w-46,h-48);
}

RGBWidget::RGBWidget(QWidget *parent) : QWidget(parent)
{
  //m_pLayout = new QGridLayout(this, 3, 3);

  m_pRSlider = new ColorSlider(this);
  m_pRSlider->setMaximumHeight(30);
  m_pRSlider->slotSetRange(0, 255);

  m_pGSlider = new ColorSlider(this);
  m_pGSlider->setMaximumHeight(30);
  m_pGSlider->slotSetRange(0, 255);

  m_pBSlider = new ColorSlider(this);
  m_pBSlider->setMaximumHeight(30);
  m_pBSlider->slotSetRange(0, 255);
 
  m_pRLabel = new QLabel("R", this);
  m_pRLabel->setFixedWidth(20);
  m_pRLabel->setFixedHeight(20);
  m_pGLabel = new QLabel("G", this);
  m_pGLabel->setFixedWidth(20);
  m_pGLabel->setFixedHeight(20);
  m_pBLabel = new QLabel("B", this);
  m_pBLabel->setFixedWidth(20);
  m_pBLabel->setFixedHeight(20);
 
  m_pRIn = new QSpinBox(0, 255, 1, this);
  m_pRIn->setFixedWidth(42);
  m_pRIn->setFixedHeight(20);
  m_pGIn = new QSpinBox(0, 255, 1, this);
  m_pGIn->setFixedWidth(42);
  m_pGIn->setFixedHeight(20);
  m_pBIn = new QSpinBox(0, 255, 1, this);
  m_pBIn->setFixedWidth(42);
  m_pBIn->setFixedHeight(20);

  /*
	m_pLayout->addWidget(m_pRLabel, 0, 0);
	m_pLayout->addWidget(m_pRSlider, 0, 1);
	m_pLayout->addWidget(m_pRIn, 0, 2);
	
	m_pLayout->addWidget(m_pGLabel, 1, 0);
	m_pLayout->addWidget(m_pGSlider, 1, 1);
	m_pLayout->addWidget(m_pGIn, 1, 2);
	
	m_pLayout->addWidget(m_pBLabel, 2, 0);
	m_pLayout->addWidget(m_pBSlider, 2, 1);
	m_pLayout->addWidget(m_pBIn, 2, 2);
  */
  connect(m_pRSlider, SIGNAL(colorSelected(const QColor&)), this, SLOT(slotRedChanged(const QColor &)));
  connect(m_pGSlider, SIGNAL(colorSelected(const QColor&)), this, SLOT(slotGreenChanged(const QColor &)));
  connect(m_pBSlider, SIGNAL(colorSelected(const QColor&)), this, SLOT(slotBlueChanged(const QColor &)));

  //connect(m_pRIn, SIGNAL(valueChanged (int)), m_pRSlider, SLOT(slotSetValue(int)));
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

void RGBWidget::slotSetColor(const QColor&c)
{
  m_pRSlider->slotSetColor1(QColor(0, c.green(), c.blue()));
  m_pRSlider->slotSetColor2(QColor(255, c.green(), c.blue()));
  m_pRSlider->slotSetValue(c.red());

  m_pGSlider->slotSetColor1(QColor(c.red(), 0, c.blue()));
  m_pGSlider->slotSetColor2(QColor(c.red(), 255, c.blue()));
  m_pGSlider->slotSetValue(c.green());

  m_pBSlider->slotSetColor1(QColor(c.red(), c.green(), 0));
  m_pBSlider->slotSetColor2(QColor(c.red(), c.green(), 255));
  m_pBSlider->slotSetValue(c.blue());
}
  
void RGBWidget::slotRedChanged(const QColor& c)
{
  m_pGSlider->slotSetColor1(QColor(c.red(), 0, c.blue()));
  m_pGSlider->slotSetColor2(QColor(c.red(), 255, c.blue()));

  m_pBSlider->slotSetColor1(QColor(c.red(), c.green(), 0));
  m_pBSlider->slotSetColor2(QColor(c.red(), c.green(), 255));

  emit colorChanged(c);
}

void RGBWidget::slotGreenChanged(const QColor& c)
{
  m_pRSlider->slotSetColor1(QColor(0, c.green(), c.blue()));
  m_pRSlider->slotSetColor2(QColor(255, c.green(), c.blue()));

  m_pBSlider->slotSetColor1(QColor(c.red(), c.green(), 0));
  m_pBSlider->slotSetColor2(QColor(c.red(), c.green(), 255));

  emit colorChanged(c);
}

void RGBWidget::slotBlueChanged(const QColor& c)
{
  m_pRSlider->slotSetColor1(QColor(0, c.green(), c.blue()));
  m_pRSlider->slotSetColor2(QColor(255, c.green(), c.blue()));

  m_pGSlider->slotSetColor1(QColor(c.red(), 0, c.blue()));
  m_pGSlider->slotSetColor2(QColor(c.red(), 255, c.blue()));

  emit colorChanged(c);
}

RGBWidget::~RGBWidget()
{
  delete m_pRSlider;
  delete m_pGSlider;
  delete m_pBSlider;
  //delete m_pLayout;
}

#include "colordialog.moc"
