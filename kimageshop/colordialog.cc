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

#include <klocale.h>

#include "kdualcolorbtn.h"
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
  m_pRGBWidget->setBackgroundColor(blue);
  m_pGradient = new GradientFrame(this);
  m_pGradient->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  m_pGradient->setBackgroundColor(white);
  m_pColorButton = new KDualColorButton(this);
  
  m_pGrayButton = new QPushButton("Gray", this);
  m_pRGBButton = new QPushButton("RGB", this);
  m_pHSBButton = new QPushButton("HSB", this);
  m_pCMYKButton = new QPushButton("CMYK", this);
  m_pLABButton = new QPushButton("LAB", this);

  m_fg = KColor(255,255,255);;
  m_bg = KColor(0,0,0);
}

ColorChooserWidget::~ColorChooserWidget() {}

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
  m_pGradient->setGeometry(2, h-24, w-4, 22);
  m_pRGBWidget->setGeometry(44,22,w-46,h-48);
}

GradientFrame::GradientFrame(QWidget *parent) : QFrame(parent)
{
  setFrameStyle(Panel | Sunken);
  m_c1 = QColor(255,255,255);
  m_c2 = QColor(0,0,0);
}
GradientFrame::~GradientFrame() {}

void GradientFrame::drawContents(QPainter *p)
{
  QPixmap *pm;
  QRect r = contentsRect();

  if (m_pm.size() != r.size())
	{
	  m_pm.resize(r.width(), r.height());
	  KPixmapEffect::gradient(m_pm, m_c1, m_c2, KPixmapEffect::HorizontalGradient);
	}
  
  pm = &m_pm;
  p->drawPixmap(0, 0, *pm);
}

RGBWidget::RGBWidget(QWidget *parent) : QWidget(parent) {}
RGBWidget::~RGBWidget() {}

#include "colordialog.moc"
