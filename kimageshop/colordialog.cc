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
#include <qbuttongroup.h>

#include <klocale.h>
#include <kdebug.h>

#include "colorframe.h"
#include "colorslider.h"
#include "colordialog.h"

ColorDialog::ColorDialog(QWidget *parent) : KFloatingDialog(parent)
{
  setCaption(i18n("Color chooser"));
  resize(260, 180);
  setMinimumWidth(210);
  setMinimumHeight(120);
  m_pBase = new ColorChooserWidget(this);
  m_pBase->slotSetFGColor(QColor(0, 0, 0));
  m_pBase->slotSetBGColor(QColor(255, 255, 255));
  setBaseWidget(m_pBase);
}

ColorDialog::~ColorDialog()
{
  delete m_pBase;
}

ColorChooserWidget::ColorChooserWidget(QWidget *parent) : QWidget(parent)
{
  m_pRGBWidget = new RGBWidget(this);
  m_pGreyWidget = new GreyWidget(this);
  m_pColorFrame = new ColorFrame(this);
  m_pColorButton = new KDualColorButton(this);
  
  m_pGreyButton = new QPushButton("Grey", this);
  m_pGreyButton->setToggleButton(true);
  m_pRGBButton = new QPushButton("RGB", this);
  m_pRGBButton->setToggleButton(true);
  m_pHSBButton = new QPushButton("HSB", this);
  m_pHSBButton->setToggleButton(true);
  m_pCMYKButton = new QPushButton("CMYK", this);
  m_pCMYKButton->setToggleButton(true);
  m_pLABButton = new QPushButton("LAB", this);
  m_pLABButton->setToggleButton(true);

  connect(m_pGreyButton, SIGNAL(clicked()), this,
		  SLOT(slotShowGrey()));
  connect(m_pRGBButton, SIGNAL(clicked()), this,
		  SLOT(slotShowRGB()));
  connect(m_pHSBButton, SIGNAL(clicked()), this,
		  SLOT(slotShowHSB()));
  connect(m_pCMYKButton, SIGNAL(clicked()), this,
		  SLOT(slotShowCMYK()));
  connect(m_pLABButton, SIGNAL(clicked()), this,
		  SLOT(slotShowLAB()));

  // connect dual-color button
  connect(m_pColorButton, SIGNAL(fgChanged(const QColor &)), this,
		  SLOT(slotColorButtonFGChanged(const QColor &)));
  connect(m_pColorButton, SIGNAL(bgChanged(const QColor &)), this,
		  SLOT(slotColorButtonBGChanged(const QColor &)));
  connect(m_pColorButton, SIGNAL(currentChanged(KDualColorButton::DualColor)), this,
		  SLOT(slotColorButtonCurrentChanged(KDualColorButton::DualColor)));
  
  // connect color frame
  connect(m_pColorFrame, SIGNAL(colorSelected(const QColor &)), this,
		  SLOT(slotColorFrameChanged(const QColor &)));

  // connect RGB widget
  connect(m_pRGBWidget, SIGNAL(colorChanged(const QColor &)), this,
		  SLOT(slotRGBWidgetChanged(const QColor &)));

  slotShowRGB();
}

ColorChooserWidget::~ColorChooserWidget()
{
  delete m_pRGBWidget;
  delete m_pGreyWidget;
  delete m_pColorFrame;
  delete m_pColorButton;
  delete m_pGreyButton;
  delete m_pRGBButton;
  delete m_pHSBButton;
  delete m_pCMYKButton;
  delete m_pLABButton;
}

void ColorChooserWidget::slotRGBWidgetChanged(const QColor& c)
{
  KDualColorButton::DualColor current = m_pColorButton->current();

  if (current == KDualColorButton::Foreground)
	{
	  m_pColorButton->slotSetForeground(c);
	  m_pColorFrame->slotSetColor1(c);
	}
  else if (current == KDualColorButton::Background)
	{
	  m_pColorButton->slotSetBackground(c);
	  m_pColorFrame->slotSetColor2(c);
	}
  m_pGreyWidget->slotSetColor(c);
}

void ColorChooserWidget::slotGreyWidgetChanged(const QColor& c)
{
  KDualColorButton::DualColor current = m_pColorButton->current();

  if (current == KDualColorButton::Foreground)
	{
	  m_pColorButton->slotSetForeground(c);
	  m_pColorFrame->slotSetColor1(c);
	}
  else if (current == KDualColorButton::Background)
	{
	  m_pColorButton->slotSetBackground(c);
	  m_pColorFrame->slotSetColor2(c);
	}

  m_pRGBWidget->slotSetColor(c);
}

void ColorChooserWidget::slotColorFrameChanged(const QColor& c)
{
  KDualColorButton::DualColor current = m_pColorButton->current();
  
  if (current == KDualColorButton::Foreground)
	  m_pColorButton->slotSetForeground(c);
  else if (current == KDualColorButton::Background)
	  m_pColorButton->slotSetBackground(c);
  
  m_pRGBWidget->slotSetColor(c);
  m_pGreyWidget->slotSetColor(c);
}

void ColorChooserWidget::slotColorButtonFGChanged(const QColor& c)
{
  m_pColorFrame->slotSetColor1(c);
  
  if (m_pColorButton->current() == KDualColorButton::Foreground)
	{
	  m_pRGBWidget->slotSetColor(c);
	  m_pGreyWidget->slotSetColor(c);
	}
}

void ColorChooserWidget::slotColorButtonBGChanged(const QColor& c)
{
  m_pColorFrame->slotSetColor2(c);

  if (m_pColorButton->current() == KDualColorButton::Background)
	{  
	  m_pRGBWidget->slotSetColor(c);
	  m_pGreyWidget->slotSetColor(c);
	}
}

void ColorChooserWidget::slotColorButtonCurrentChanged(KDualColorButton::DualColor)
{
  m_pColorFrame->slotSetColor1(m_pColorButton->foreground());
  m_pColorFrame->slotSetColor2(m_pColorButton->background());

  m_pRGBWidget->slotSetColor(m_pColorButton->currentColor());
  m_pGreyWidget->slotSetColor(m_pColorButton->currentColor());
}

void ColorChooserWidget::slotSetFGColor(const QColor& c)
{
  m_pColorFrame->slotSetColor1(c);
  m_pColorButton->slotSetForeground(c);

  m_pRGBWidget->slotSetColor(m_pColorButton->currentColor());
  m_pGreyWidget->slotSetColor(m_pColorButton->currentColor());
}

void ColorChooserWidget::slotSetBGColor(const QColor& c)
{
  m_pColorFrame->slotSetColor2(c);
  m_pColorButton->slotSetBackground(c);

  m_pRGBWidget->slotSetColor(m_pColorButton->currentColor());
  m_pGreyWidget->slotSetColor(m_pColorButton->currentColor());
}

void ColorChooserWidget::slotShowGrey()
{
  m_pRGBWidget->hide();
  m_pGreyWidget->show();

  m_pGreyButton->setOn(true);
  m_pRGBButton->setOn(false);
  m_pHSBButton->setOn(false);
  m_pCMYKButton->setOn(false);
  m_pLABButton->setOn(false);
}

void ColorChooserWidget::slotShowRGB()
{
  m_pGreyWidget->hide();
  m_pRGBWidget->show();
  
  m_pRGBButton->setOn(true);
  m_pGreyButton->setOn(false);
  m_pHSBButton->setOn(false);
  m_pCMYKButton->setOn(false);
  m_pLABButton->setOn(false);
}

void ColorChooserWidget::slotShowHSB()
{
  m_pGreyWidget->hide();
  m_pRGBWidget->hide();

  m_pHSBButton->setOn(true);
  m_pGreyButton->setOn(false);
  m_pRGBButton->setOn(false);
  m_pCMYKButton->setOn(false);
  m_pLABButton->setOn(false);
}

void ColorChooserWidget::slotShowCMYK()
{
  m_pGreyWidget->hide();
  m_pRGBWidget->hide();

  m_pCMYKButton->setOn(true);
  m_pGreyButton->setOn(false);
  m_pRGBButton->setOn(false);
  m_pHSBButton->setOn(false);
  m_pLABButton->setOn(false);
}

void ColorChooserWidget::slotShowLAB()
{
  m_pGreyWidget->hide();
  m_pRGBWidget->hide();

  m_pLABButton->setOn(true);
  m_pGreyButton->setOn(false);
  m_pRGBButton->setOn(false);
  m_pHSBButton->setOn(false);
  m_pCMYKButton->setOn(false);
}

void ColorChooserWidget::resizeEvent(QResizeEvent *e)
{
  // color model buttons
  int w = width();
  int h = height();

  m_pLABButton->setGeometry(w-32, 0, 30, 18);
  m_pCMYKButton->setGeometry(w-72, 0, 40, 18);
  m_pHSBButton->setGeometry(w-102, 0, 30, 18);
  m_pRGBButton->setGeometry(w-132, 0, 30, 18);
  m_pGreyButton->setGeometry(w-162, 0, 30, 18);
  
  m_pColorButton->setGeometry(2, 0, 40, 40);
  m_pColorFrame->setGeometry(2, h-24, w-4, 22);
  m_pRGBWidget->setGeometry(42,20,w-44,h-46);
  m_pGreyWidget->setGeometry(42,20,w-44,h-46);
  //kdebug(KDEBUG_INFO, 0, "w: %d h: %d", w, h);
}

RGBWidget::RGBWidget(QWidget *parent) : QWidget(parent)
{
  m_pRSlider = new ColorSlider(this);
  m_pRSlider->setMaximumHeight(25);
  m_pRSlider->slotSetRange(0, 255);

  m_pGSlider = new ColorSlider(this);
  m_pGSlider->setMaximumHeight(25);
  m_pGSlider->slotSetRange(0, 255);

  m_pBSlider = new ColorSlider(this);
  m_pBSlider->setMaximumHeight(25);
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

  connect(m_pRSlider, SIGNAL(colorSelected(const QColor&)), this,
		  SLOT(slotRedChanged(const QColor &)));
  connect(m_pGSlider, SIGNAL(colorSelected(const QColor&)), this,
		  SLOT(slotGreenChanged(const QColor &)));
  connect(m_pBSlider, SIGNAL(colorSelected(const QColor&)), this,
		  SLOT(slotBlueChanged(const QColor &)));

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

  delete m_pRLabel;
  delete m_pGLabel;
  delete m_pBLabel;

  delete m_pRIn;
  delete m_pGIn;
  delete m_pBIn;
}

GreyWidget::GreyWidget(QWidget *parent) : QWidget(parent)
{
  m_pVSlider = new ColorSlider(this);
  m_pVSlider->setMaximumHeight(25);
  m_pVSlider->slotSetRange(0, 255);
  m_pVSlider->slotSetColor1(QColor(255, 255, 255));
  m_pVSlider->slotSetColor2(QColor(0, 0, 0));
 
  m_pVLabel = new QLabel("B", this);
  m_pVLabel->setFixedWidth(20);
  m_pVLabel->setFixedHeight(20);
  
  m_pVIn = new QSpinBox(0, 255, 1, this);
  m_pVIn->setFixedWidth(42);
  m_pVIn->setFixedHeight(20);

  connect(m_pVSlider, SIGNAL(colorSelected(const QColor&)), this,
		  SLOT(slotValueChanged(const QColor &)));
}

void GreyWidget::resizeEvent(QResizeEvent *)
{
  // I know a QGridLayout would look nicer,
  // but it does not use the space as good as I want it to.

  int y = height()/2;

  int labelY =y - m_pVLabel->height()/2 - 4;
  if (labelY < 0) 
	labelY = 0;

  m_pVLabel->move(2, 0 + labelY);

  int x1 = m_pVLabel->pos().x() + m_pVLabel->width();

  int inY =y - m_pVIn->height()/2 - 4;
  if (inY < 0) 
	inY = 0;

  m_pVIn->move(width() - m_pVIn->width(), 0 + inY);

  int x2 = width() - m_pVIn->width() - 2;

  m_pVSlider->resize(QSize(x2 - x1, y));
  m_pVSlider->move(x1, y - m_pVSlider->height()/2);
}

void GreyWidget::slotSetColor(const QColor&c)
{
  m_c = c;
  float v = c.red() + c.green() + c.blue();
  v /= 3;
  m_pVSlider->slotSetValue(static_cast<int>(v));
}
  
void GreyWidget::slotValueChanged(const QColor& c)
{
  emit colorChanged(c);
}

GreyWidget::~GreyWidget()
{
  delete m_pVSlider;
  delete m_pVLabel;
  delete m_pVIn;
}

#include "colordialog.moc"
