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

  m_pColorButton = new KDualColorButton(this);

  connect(m_pColorButton, SIGNAL(fgChanged(const QColor &)), this,
	  SLOT(slotFGColorSelected(const QColor &)));

  connect(m_pColorButton, SIGNAL(bgChanged(const QColor &)), this,
	  SLOT(slotBGColorSelected(const QColor &)));
}

ControlFrame::~ControlFrame()
{
  delete m_pColorButton;
}

void ControlFrame::resizeEvent ( QResizeEvent * )
{
  m_pColorButton->setGeometry( 5, 5, 40, 40 );
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
