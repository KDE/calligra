/*
 *  kis_colorchooser.cc - part of KImageShop
 *
 *  Copyright (c) 1999 Matthias Elter  <elter@kde.org>
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
#include <qpainter.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qbuttongroup.h>

#include <klocale.h>
#include <kdebug.h>
#include <kglobal.h>

#include "colorframe.h"
#include "colorslider.h"

#include "kis_colorchooser.h"


KisColorChooser::KisColorChooser(QWidget *parent) : QWidget(parent)
{
    // init with defaults
    m_fg = KisColor::white();
    m_bg = KisColor::black();
    m_active = ac_Foreground;
  
    // setup color frame
    m_pColorFrame = new ColorFrame(this);

    // connect color frame
    connect(m_pColorFrame, SIGNAL(colorSelected(const QColor &)), this,
		  SLOT(slotColorFrameChanged(const QColor &)));

    // setup color widgets
    m_pRGBWidget = new RGBWidget(this);
    m_pGreyWidget = new GreyWidget(this);

    // connect color widgets
    connect(m_pRGBWidget, SIGNAL(colorChanged(const KisColor &)), this,
		  SLOT(slotRGBWidgetChanged(const KisColor &)));

    //connect(m_pGreyWidget, SIGNAL(colorChanged(const KisColor &)), this,
    //		  SLOT(slotGreyWidgetChanged(const KisColor &)));

    // show RGB as default 
    slotShowRGB();
}

void KisColorChooser::slotRGBWidgetChanged(const KisColor& c)
{
    if( m_active == ac_Foreground )
	    m_pColorFrame->slotSetColor1(c.color());
    else
	    m_pColorFrame->slotSetColor2(c.color());

    emit colorChanged(c);
    m_pGreyWidget->slotSetColor(c);
}

void KisColorChooser::slotGreyWidgetChanged(const KisColor& c)
{
    if( m_active == ac_Foreground )
	    m_pColorFrame->slotSetColor1(c.color());
    else
	    m_pColorFrame->slotSetColor2(c.color());

    emit colorChanged(c);
    m_pRGBWidget->slotSetColor(c);
}

void KisColorChooser::slotColorFrameChanged(const QColor& qc)
{
    KisColor c(qc);

    emit colorChanged(c);

    m_pGreyWidget->slotSetColor(c);
    m_pRGBWidget->slotSetColor(c);
}

void KisColorChooser::slotSetFGColor(const KisColor& c)
{
    m_fg = c;

    if (m_active == ac_Foreground)
	{
	    m_pColorFrame->slotSetColor1(c.color());
  	    m_pRGBWidget->slotSetColor(c);
	    m_pGreyWidget->slotSetColor(c);
	}
}

void KisColorChooser::slotSetBGColor(const KisColor& c)
{
    m_bg = c;

    if (m_active == ac_Background)
	{
	    m_pColorFrame->slotSetColor1(c.color());
	    m_pRGBWidget->slotSetColor(c);
	    m_pGreyWidget->slotSetColor(c);
	}
}

void KisColorChooser::slotShowGrey()
{
    m_pRGBWidget->hide();
    m_pGreyWidget->show();
}

void KisColorChooser::slotShowRGB()
{
    m_pGreyWidget->hide();
    m_pRGBWidget->show();
}

void KisColorChooser::slotShowHSB()
{
    m_pGreyWidget->hide();
    m_pRGBWidget->hide();
}

void KisColorChooser::slotShowCMYK()
{
    m_pGreyWidget->hide();
    m_pRGBWidget->hide();
}

void KisColorChooser::slotShowLAB()
{
    m_pGreyWidget->hide();
    m_pRGBWidget->hide();
}

void KisColorChooser::resizeEvent(QResizeEvent *)
{
    int w = width();
    int h = height();

    m_pColorFrame->setGeometry(2, h-22, w-4, 20);

    m_pRGBWidget->setGeometry(2, 2, w-4, h-24);
    m_pGreyWidget->setGeometry(2, 2, w-4, h-24);
}

RGBWidget::RGBWidget(QWidget *parent) : QWidget(parent)
{
    // init with defaults
    m_c = KisColor::white();

    // setup color sliders
    m_pRSlider = new ColorSlider(this);
    m_pRSlider->setMaximumHeight(20);
    m_pRSlider->slotSetRange(0, 255);

    m_pGSlider = new ColorSlider(this);
    m_pGSlider->setMaximumHeight(20);
    m_pGSlider->slotSetRange(0, 255);

    m_pBSlider = new ColorSlider(this);
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

void RGBWidget::slotSetColor(const KisColor&c)
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

  m_c = KisColor( r, g, b, cs_RGB );

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

    m_c = KisColor( r, g, b, cs_RGB );

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

  m_c = KisColor( r, g, b, cs_RGB );

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

  m_c = KisColor( r, g, b, cs_RGB );

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

  m_c = KisColor( r, g, b, cs_RGB );

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

  m_c = KisColor( r, g, b, cs_RGB );

  m_pRSlider->slotSetColor1( QColor( 0, g, b ) );
  m_pRSlider->slotSetColor2( QColor( 255, g, b ) );

  m_pGSlider->slotSetColor1( QColor( r, 0, b ) );
  m_pGSlider->slotSetColor2( QColor( r, 255, b ) );

  m_pBSlider->slotSetValue( b );

  emit colorChanged(m_c);
}

GreyWidget::GreyWidget(QWidget *parent) : QWidget(parent)
{
  // init with defaults
  m_c = KisColor::white();

  // setup slider
  m_pVSlider = new ColorSlider(this);
  m_pVSlider->setMaximumHeight(20);
  m_pVSlider->slotSetRange(0, 255);
  m_pVSlider->slotSetColor1(QColor(255, 255, 255));
  m_pVSlider->slotSetColor2(QColor(0, 0, 0));
 
  // setup slider label
  m_pVLabel = new QLabel("K", this);
  m_pVLabel->setFixedWidth(16);
  m_pVLabel->setFixedHeight(20);
  
  // setup spin box
  m_pVIn = new QSpinBox(0, 255, 1, this);
  m_pVIn->setFixedWidth(42);
  m_pVIn->setFixedHeight(20);

  // connect color slider
  connect(m_pVSlider, SIGNAL(valueChanged(int)), this,
  		  SLOT(slotVSliderChanged(int)));

  // connect spin box
  connect(m_pVIn, SIGNAL(valueChanged(int)), this,
  		  SLOT(slotVInChanged(int)));
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

void GreyWidget::slotSetColor(const KisColor&c)
{
  m_c = c;
  
  float v = c.R() + c.G() + c.B();
  v /= 3;
  v = 255 - v;
  m_pVIn->setValue(static_cast<int>(v));
  m_pVSlider->slotSetValue(static_cast<int>(v));
}
  
void GreyWidget::slotVSliderChanged(int v)
{
  m_pVIn->setValue(v);
  v = 255 - v;
  emit colorChanged( KisColor(v,v,v, cs_RGB));
}

void GreyWidget::slotVInChanged(int v)
{
  m_pVSlider->slotSetValue(v);
  v = 255 - v;
  emit colorChanged(KisColor(v,v,v, cs_RGB));
}

#include "kis_colorchooser.moc"
