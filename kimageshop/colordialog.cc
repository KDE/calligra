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

#include "kdualcolorbtn.h"
#include "colordialog.h"


ColorDialog::ColorDialog(QWidget *parent) : KFloatingDialog(parent)
{
  setCaption( i18n( "Color chooser" ) );

  m_pBase = new QWidget(this);
  resize(280, 190);
  setFixedWidth(280);
  setFixedHeight(190);

  m_pRGBWidget = new RGBWidget(m_pBase);
  m_pRGBWidget->setBackgroundColor(blue);
  m_pGradient = new GradientFrame(m_pBase);
  m_pGradient->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  m_pGradient->setBackgroundColor(white);
  m_pColorButton = new KDualColorButton(m_pBase);

  m_pGrayButton = new QPushButton("Gray", m_pBase);
  m_pRGBButton = new QPushButton("RGB", m_pBase);
  m_pHSBButton = new QPushButton("HSB", m_pBase);
  m_pCMYKButton = new QPushButton("CMYK", m_pBase);
  m_pLABButton = new QPushButton("LAB", m_pBase);
}

ColorDialog::~ColorDialog() {}

void ColorDialog::resizeEvent(QResizeEvent *e)
{
  KFloatingDialog::resizeEvent(e);
  m_pBase->setGeometry(_left(), _top(), _width(), _height());

  // color model buttons
  int w = m_pBase->width();
  int h = m_pBase->height();
  m_pLABButton->setGeometry(w-32, 1, 30, 18);
  m_pCMYKButton->setGeometry(w-72, 1, 40, 18);
  m_pHSBButton->setGeometry(w-102, 1, 30, 18);
  m_pRGBButton->setGeometry(w-132, 1, 30, 18);
  m_pGrayButton->setGeometry(w-162, 1, 30, 18);

  m_pColorButton->setGeometry(2, 5, 40, 40);
  m_pGradient->setGeometry(2, h-24, w-4, 22);
  m_pRGBWidget->setGeometry(44,22,w-46,h-48);
}

GradientFrame::GradientFrame(QWidget *parent) : QFrame(parent)
{
  setFrameStyle(Panel | Sunken);
  setBackgroundColor(white);
}
GradientFrame::~GradientFrame() {}

void GradientFrame::drawContents(QPainter *p)
{
}

RGBWidget::RGBWidget(QWidget *parent) : QWidget(parent) {}
RGBWidget::~RGBWidget() {}

#include "colordialog.moc"
