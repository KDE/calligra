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

  m_fg = KColor(255,255,255);;
  m_bg = KColor(0,0,0);

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
  m_pRSlider = new ColorSlider(this);
  m_pGSlider = new ColorSlider(this);
  m_pBSlider = new ColorSlider(this);

  m_pRSlider->setMaximumHeight(32);
  m_pGSlider->setMaximumHeight(32);
  m_pBSlider->setMaximumHeight(32);

  m_pVLayout = new QVBoxLayout(this, 3);
  m_pVLayout->addWidget(m_pRSlider);
  m_pVLayout->addWidget(m_pGSlider);
  m_pVLayout->addWidget(m_pBSlider);

}

void RGBWidget::slotSetColor(const QColor&c)
{
  m_pRSlider->slotSetColor1(c);
  m_pRSlider->slotSetColor2(QColor(255, c.green(), c.blue()));

  m_pGSlider->slotSetColor1(c);
  m_pGSlider->slotSetColor2(QColor(c.red(), 255, c.blue()));

  m_pBSlider->slotSetColor1(c);
  m_pBSlider->slotSetColor2(QColor(c.red(), c.green(), 255));
}
  
void RGBWidget::slotRedChanged(const QColor& c)
{
  emit colorChanged(c);
}

void RGBWidget::slotGreenChanged(const QColor& c)
{
  emit colorChanged(c);
}

void RGBWidget::slotBlueChanged(const QColor& c)
{
  emit colorChanged(c);
}

RGBWidget::~RGBWidget()
{
  delete m_pRSlider;
  delete m_pGSlider;
  delete m_pBSlider;
  delete m_pVLayout;
}

#include "colordialog.moc"
