/*
 *  kis_sidebar.cc - part of KImageShop
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
 *  GNU General Public License for more details.g
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <kdualcolorbtn.h>

#include "kis_sidebar.h"
#include "kis_brushwidget.h"

KisSideBar::KisSideBar( QWidget* parent, const char* name ) : QWidget( parent, name )
{
  m_pTopFrame = new TopFrame(this);
  m_pControlFrame = new ControlFrame(this);
  
  setFixedWidth( 50 );

  connect(m_pControlFrame, SIGNAL(fgColorChanged(const KisColor &)), this,
	  SLOT(slotFGColorSelected(const KisColor &)));

  connect(m_pControlFrame, SIGNAL(bgColorChanged(const KisColor &)), this,
	  SLOT(slotBGColorSelected(const KisColor &)));
}

KisSideBar::~KisSideBar()
{
  delete m_pTopFrame;
  delete m_pControlFrame;
}

void KisSideBar::resizeEvent ( QResizeEvent * )
{
  m_pTopFrame->setGeometry( 0, 0, width(), 20 );
  m_pControlFrame->setGeometry( 0, 20, width(), height() - 20 );
}

void KisSideBar::slotSetFGColor(const KisColor& c)
{
  m_pControlFrame->setFGColor( c );
}

void KisSideBar::slotSetBGColor(const KisColor& c)
{
  m_pControlFrame->setBGColor( c );
}

void KisSideBar::slotFGColorSelected(const KisColor& c)
{
  emit fgColorChanged( c );
}

void KisSideBar::slotBGColorSelected(const KisColor& c)
{
  emit bgColorChanged( c );
}

void KisSideBar::slotSetPosition( const QPoint& p )
{
  QString s;
  
  if ( p.x() == -1 )
    s = "#";
  else
    s.setNum( p.x() );

  m_pControlFrame->setXValue(s);

  if ( p.y() == -1 )
    s = "#";
  else
    s.setNum( p.y() );
  m_pControlFrame->setYValue(s);
}

void KisSideBar::slotSetColor(const KisColor& c)
{
  // FIXME : non - RGB color spaces
  QString s;

  s.setNum(c.R());
  m_pControlFrame->setC1Value(s);

  s.setNum(c.G());
  m_pControlFrame->setC2Value(s);

  s.setNum(c.B());
  m_pControlFrame->setC3Value(s);

  m_pControlFrame->setColorFrame(c.color());
}

void KisSideBar::slotSetBrush( const KisBrush& b )
{
  m_pControlFrame->setBrush(b);
}

TopFrame::TopFrame( QWidget* parent, const char* name ) : QFrame( parent, name )
{
  setFrameStyle( Panel | Raised );
}

void TopFrame::drawContents ( QPainter * )
{
  // TODO
}

ControlFrame::ControlFrame( QWidget* parent, const char* name ) : QFrame( parent, name )
{
  setFrameStyle( Panel | Raised );

  QFont f("Helvetica", 10);
  setFont(f);

  m_pColorButton = new KDualColorButton(this);
  m_pC1Label = new QLabel( "R:", this );
  m_pC1Label->setFont( f );
  m_pC2Label = new QLabel("G:", this);
  m_pC2Label->setFont(f);
  m_pC3Label = new QLabel("B:", this);
  m_pC3Label->setFont(f);
  m_pC4Label = new QLabel("", this);
  m_pC4Label->setFont(f);
  m_pALabel = new QLabel("A:", this);
  m_pALabel->setFont(f);
  m_pXLabel = new QLabel("X:", this);
  m_pXLabel->setFont(f);
  m_pYLabel = new QLabel("Y:", this);
  m_pYLabel->setFont(f);
  m_pC1Value = new QLabel("255", this);
  m_pC1Value->setAlignment( AlignRight );
  m_pC1Value->setFont(f);
  m_pC2Value = new QLabel("255", this);
  m_pC2Value->setAlignment( AlignRight );
  m_pC2Value->setFont(f);
  m_pC3Value = new QLabel("255", this);
  m_pC3Value->setAlignment( AlignRight );
  m_pC3Value->setFont(f);
  m_pC4Value = new QLabel("", this);
  m_pC4Value->setAlignment( AlignRight );
  m_pC4Value->setFont(f);
  m_pAValue = new QLabel("255", this);
  m_pAValue->setAlignment( AlignRight );
  m_pAValue->setFont(f);
  m_pXValue = new QLabel("32000", this);
  m_pXValue->setAlignment( AlignRight );
  m_pXValue->setFont(f);
  m_pYValue = new QLabel("32000", this);
  m_pYValue->setAlignment( AlignRight );
  m_pYValue->setFont(f);
  m_pColorFrame = new QFrame(this);

  m_pColorFrame->setBackgroundColor(white);
  m_pColorFrame->setFrameStyle( Panel | Sunken );

  m_pBrushWidget = new KisBrushWidget(this);

  connect(m_pColorButton, SIGNAL(fgChanged(const QColor &)), this,
	  SLOT(slotFGColorSelected(const QColor &)));

  connect(m_pColorButton, SIGNAL(bgChanged(const QColor &)), this,
	  SLOT(slotBGColorSelected(const QColor &)));
}

ControlFrame::~ControlFrame()
{
  delete m_pColorButton;
  delete m_pC1Label;
  delete m_pC2Label;
  delete m_pC3Label;
  delete m_pC4Label;
  delete m_pALabel;
  delete m_pXLabel;
  delete m_pYLabel;
  delete m_pC1Value;
  delete m_pC2Value;
  delete m_pC3Value;
  delete m_pC4Value;
  delete m_pAValue;
  delete m_pXValue;
  delete m_pYValue;
}


void ControlFrame::setBrush(const KisBrush& b)
{
  m_pBrushWidget->slotSetBrush(b);
}

void ControlFrame::resizeEvent ( QResizeEvent * )
{
  m_pColorButton->setGeometry( 5, 5, 40, 40 );

  m_pC1Label->setGeometry( 5, 50, 10, 14 );
  m_pC2Label->setGeometry( 5, 64, 10, 14 );
  m_pC3Label->setGeometry( 5, 78, 10, 14 );
  m_pC4Label->setGeometry( 5, 92, 10, 14 );
  m_pALabel->setGeometry( 5, 106, 10, 14 );
  m_pC1Value->setGeometry( 15, 50, 30, 14 );
  m_pC2Value->setGeometry( 15, 64, 30, 14 );
  m_pC3Value->setGeometry( 15, 78, 30, 14 );
  m_pC4Value->setGeometry( 15, 92, 30, 14 );
  m_pAValue->setGeometry( 15, 106, 30, 14 );

  m_pColorFrame->setGeometry( 25, 120, 20, 20 );

  m_pXValue->setGeometry( 15, 150, 30, 14 );
  m_pYValue->setGeometry( 15, 164, 30, 14 );
  m_pXLabel->setGeometry( 5, 150, 10, 14 );
  m_pYLabel->setGeometry( 5, 164, 10, 14 );

  m_pBrushWidget->setGeometry( 5, 183, 40, 40 );
}

void ControlFrame::setFGColor(const KisColor& c)
{
  m_pColorButton->slotSetForeground( c.color() );
}

void ControlFrame::setBGColor(const KisColor& c)
{
  m_pColorButton->slotSetBackground( c.color() );
}

void ControlFrame::slotFGColorSelected(const QColor& c)
{
  emit fgColorChanged( KisColor(c) );
}

void ControlFrame::slotBGColorSelected(const QColor& c)
{
  emit bgColorChanged( KisColor(c) );
}




#include "kis_sidebar.moc"
