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

#include <kglobalsettings.h>
#include <kdualcolorbtn.h>
#include <kdebug.h> //jwc

#include "kis_sidebar.h"
#include "kis_brushwidget.h"
#include "kis_patternwidget.h"
#include "kis_colorchooser.h"
#include "kis_framebutton.h"

KisSideBar::KisSideBar( QWidget* parent, const char* name ) : QWidget( parent, name )
{
  kdDebug() << "KisSideBar::KisSideBar" << endl; //jwc

  m_pTopFrame = new TopFrame(this);
  m_pChooserFrame = new ChooserFrame(this);
  m_pControlFrame = new ControlFrame(this);
  m_pDockFrame = new DockFrame(this);

  setFixedWidth( 200 );

  // connect chooser frame
  connect(m_pChooserFrame, SIGNAL(colorChanged(const KisColor &)), this,
	  SLOT(slotChooserColorSelected(const KisColor &)));

  // connect top frame
  connect(m_pTopFrame, SIGNAL(greyClicked()), m_pChooserFrame,
		  SLOT(slotShowGrey()));
  connect(m_pTopFrame, SIGNAL(rgbClicked()), m_pChooserFrame,
		  SLOT(slotShowRGB()));
  connect(m_pTopFrame, SIGNAL(hsbClicked()), m_pChooserFrame,
		  SLOT(slotShowHSB()));
  connect(m_pTopFrame, SIGNAL(cmykClicked()), m_pChooserFrame,
		  SLOT(slotShowCMYK()));
  connect(m_pTopFrame, SIGNAL(labClicked()), m_pChooserFrame,
		  SLOT(slotShowLAB()));

  // connect control frame
  connect(m_pControlFrame, SIGNAL(fgColorChanged(const KisColor &)), this,
	  SLOT(slotControlFGColorSelected(const KisColor &)));
  connect(m_pControlFrame, SIGNAL(bgColorChanged(const KisColor &)), this,
	  SLOT(slotControlBGColorSelected(const KisColor &)));

  connect(m_pControlFrame, SIGNAL(activeColorChanged(ActiveColor)), this,
	  SLOT(slotControlActiveColorChanged(ActiveColor)));

  kdDebug() << "KisSideBar::KisSideBar leaving" << endl; //jwc          
}

void KisSideBar::resizeEvent ( QResizeEvent * )
{
  m_pTopFrame->setGeometry( 0, 0, width(), 20 );
  m_pChooserFrame->setGeometry( 0, 20, width(), 138 );
  m_pControlFrame->setGeometry( 0, 158, width(), 42);
  m_pDockFrame->setGeometry( 0, 200, width(), height() - 200 );
}

void KisSideBar::slotSetFGColor(const KisColor& c)
{
  m_pChooserFrame->slotSetFGColor( c );
  m_pControlFrame->slotSetFGColor( c );
}

void KisSideBar::slotSetBGColor(const KisColor& c)
{
  m_pChooserFrame->slotSetBGColor( c );
  m_pControlFrame->slotSetBGColor( c );
}

void KisSideBar::slotChooserColorSelected(const KisColor& c)
{
  if (m_pControlFrame->activeColor() == ac_Foreground)
	{
	  m_pControlFrame->slotSetFGColor(c);
	  emit fgColorChanged( c );
	}
  else
	{
	  m_pControlFrame->slotSetBGColor(c);
	  emit bgColorChanged( c );
	}
}

void KisSideBar::slotControlActiveColorChanged(ActiveColor s)
{
  m_pChooserFrame->slotSetActiveColor(s);
}

void KisSideBar::slotControlFGColorSelected(const KisColor& c)
{
  m_pChooserFrame->slotSetFGColor(c);
  emit fgColorChanged( c );
}

void KisSideBar::slotControlBGColorSelected(const KisColor& c)
{
  m_pChooserFrame->slotSetBGColor(c);
  emit bgColorChanged( c );
}

void KisSideBar::slotSetBrush( const KisBrush& b )
{
  m_pControlFrame->slotSetBrush(b);
}

TopFrame::TopFrame( QWidget* parent, const char* name ) : QFrame( parent, name )
{
  setFrameStyle(Panel | Raised);
  setLineWidth(1);

  // setup buttons
  m_pHideButton = new KisFrameButton(this);
  m_pGreyButton = new KisFrameButton("Grey", this);
  m_pRGBButton = new KisFrameButton("RGB", this);
  m_pHSBButton = new KisFrameButton("HSB", this);
  m_pCMYKButton = new KisFrameButton("CMYK", this);
  m_pLABButton = new KisFrameButton("LAB", this);

  m_pEmptyFrame = new QFrame(this);
  m_pEmptyFrame->setFrameStyle(Panel | Raised);
  m_pEmptyFrame->setLineWidth(1);

  QFont font = KGlobalSettings::generalFont();
  font.setPointSize( 8 );

  m_pGreyButton->setFont(font);
  m_pRGBButton->setFont(font);
  m_pHSBButton->setFont(font);
  m_pCMYKButton->setFont(font);
  m_pLABButton->setFont(font);

  m_pGreyButton->setToggleButton(true);
  m_pRGBButton->setToggleButton(true);
  m_pHSBButton->setToggleButton(true);
  m_pCMYKButton->setToggleButton(true);
  m_pLABButton->setToggleButton(true);

  // connect buttons
  connect(m_pHideButton, SIGNAL(clicked()), this,
		  SLOT(slotHideClicked()));
  connect(m_pGreyButton, SIGNAL(clicked()), this,
		  SLOT(slotGreyClicked()));
  connect(m_pRGBButton, SIGNAL(clicked()), this,
		  SLOT(slotRGBClicked()));
  connect(m_pHSBButton, SIGNAL(clicked()), this,
		  SLOT(slotHSBClicked()));
  connect(m_pCMYKButton, SIGNAL(clicked()), this,
		  SLOT(slotCMYKClicked()));
  connect(m_pLABButton, SIGNAL(clicked()), this,
		  SLOT(slotLABClicked()));

  setFrameStyle( Panel | Raised );

  // RGB is default
  m_pRGBButton->setOn(true);
}

void TopFrame::resizeEvent ( QResizeEvent * )
{
  int w = width();

  m_pHideButton->setGeometry(w-20, 0, 20, 20);

  m_pLABButton->setGeometry(0, 0, 28, 20);
  m_pCMYKButton->setGeometry(28, 0, 36, 20);
  m_pHSBButton->setGeometry(64, 0, 28, 20);
  m_pRGBButton->setGeometry(92, 0, 28, 20);
  m_pGreyButton->setGeometry(120, 0, 28, 20);
  m_pEmptyFrame->setGeometry(148, 0, w-168, 20);
}

void TopFrame::slotHideClicked()
{
  emit hideClicked();
}

void TopFrame::slotGreyClicked()
{
  m_pCMYKButton->setOn(false);
  m_pGreyButton->setOn(true);
  m_pRGBButton->setOn(false);
  m_pHSBButton->setOn(false);
  m_pLABButton->setOn(false);
  
  emit greyClicked();
}

void TopFrame::slotRGBClicked()
{
  m_pCMYKButton->setOn(false);
  m_pGreyButton->setOn(false);
  m_pRGBButton->setOn(true);
  m_pHSBButton->setOn(false);
  m_pLABButton->setOn(false);

  emit rgbClicked();
}

void TopFrame::slotHSBClicked()
{
  m_pCMYKButton->setOn(false);
  m_pGreyButton->setOn(false);
  m_pRGBButton->setOn(false);
  m_pHSBButton->setOn(true);
  m_pLABButton->setOn(false);

  emit hsbClicked();
}

void TopFrame::slotCMYKClicked()
{
  m_pCMYKButton->setOn(true);
  m_pGreyButton->setOn(false);
  m_pRGBButton->setOn(false);
  m_pHSBButton->setOn(false);
  m_pLABButton->setOn(false);

  emit cmykClicked();
}

void TopFrame::slotLABClicked()
{
  m_pCMYKButton->setOn(false);
  m_pGreyButton->setOn(false);
  m_pRGBButton->setOn(false);
  m_pHSBButton->setOn(false);
  m_pLABButton->setOn(true);

  emit labClicked();
}

DockFrame::DockFrame( QWidget* parent, const char* name ) : QFrame( parent, name )
{
  setFrameStyle(Panel | Raised);
  setLineWidth(1);
  m_wlst.setAutoDelete(true);
  m_blst.setAutoDelete(true);
}

void DockFrame::plug (QWidget* w)
{
  if (!w)
	return;

  QString name = w->caption();

  m_wlst.append(w);
  w->reparent ( this, QPoint(0, 0), true );

  KisFrameButton* btn = new KisFrameButton(this);
  btn->setToggleButton(true);

  QFont font = KGlobalSettings::generalFont();
  font.setPointSize( 8 );

  btn->setFont(font);
  btn->setText(name);
  btn->setGeometry(0, 0, btn->width(), 18);

  // connect button
  connect(btn, SIGNAL(clicked(const QString&)), this,
		  SLOT(slotActivateTab(const QString&)));

  m_blst.append(btn);
  slotActivateTab(name);
}

void DockFrame::unplug (QWidget* w)
{
  if (!w)
	return;

  KisFrameButton *b;
  
  for ( b = m_blst.first(); b != 0; b = m_blst.next() )
	{
	  if (b->text() == w->caption())
		{
		  m_blst.remove(b);
		  break;
		}
	}		

  m_wlst.remove(w);
  w->reparent ( 0L, QPoint(0, 0), false );
}

void DockFrame::slotActivateTab(const QString& tab)
{
  QWidget *w;
  for ( w = m_wlst.first(); w != 0; w = m_wlst.next() )
	{
	  if (w->caption() == tab)
		w->show();
	  else
		w->hide();
	}
  
  KisFrameButton *b;
  for ( b = m_blst.first(); b != 0; b = m_blst.next() )
	b->setOn(b->text() == tab);
}

void DockFrame::resizeEvent ( QResizeEvent * )
{
  int bw = 0;
  int row = 0;

  KisFrameButton *b;
  
  for ( b = m_blst.first(); b != 0; b = m_blst.next() )
	{
	  if (bw >= width())
		{
		  bw = 0;
		  row++;
		}
	  b->move(bw, row*18);
	  bw += b->width();
	}
 
  QWidget *w;

  int xw = 18+ row*18;
  
  for ( w = m_wlst.first(); w != 0; w = m_wlst.next() )
	w->setGeometry(2, xw, width()-4, height()- xw-2);
}

ChooserFrame::ChooserFrame( QWidget* parent, const char* name ) : QFrame( parent, name )
{
  setFrameStyle(Panel | Raised);
  setLineWidth(1);

  m_pColorChooser = new KisColorChooser(this);

  connect(m_pColorChooser, SIGNAL(colorChanged(const KisColor &)), this,
		  SLOT(slotColorSelected(const KisColor &)));
}

void ChooserFrame::slotShowGrey()
{
  m_pColorChooser->slotShowGrey();
}

void ChooserFrame::slotShowRGB()
{
  m_pColorChooser->slotShowRGB();
}

void ChooserFrame::slotShowHSB()
{
  m_pColorChooser->slotShowHSB();
}
void ChooserFrame::slotShowCMYK()
{
  m_pColorChooser->slotShowCMYK();
}

void ChooserFrame::slotShowLAB()
{
  m_pColorChooser->slotShowLAB();
}

void ChooserFrame::slotSetActiveColor( ActiveColor a )
{
  m_pColorChooser->slotSetActiveColor(a);
}

void ChooserFrame::resizeEvent ( QResizeEvent * )
{
  m_pColorChooser->setGeometry ( 2, 2, width()-4, height()-4 );
}

void ChooserFrame::slotSetFGColor(const KisColor& c)
{
  m_pColorChooser->slotSetFGColor( c.color() );
}

void ChooserFrame::slotSetBGColor(const KisColor& c)
{
  m_pColorChooser->slotSetBGColor( c.color() );
}

void ChooserFrame::slotColorSelected(const KisColor& c)
{
  emit colorChanged( c );
}

ControlFrame::ControlFrame( QWidget* parent, const char* name ) : QFrame( parent, name )
{
  setFrameStyle(Panel | Raised);
  setLineWidth(1);

  m_pColorButton = new KDualColorButton(this);
  m_pBrushWidget = new KisBrushWidget(this);
  //m_pPatternWidget = new KisPatternWidget(this); //jwc

  connect(m_pColorButton, SIGNAL(fgChanged(const QColor &)), this,
	  SLOT(slotFGColorSelected(const QColor &)));

  connect(m_pColorButton, SIGNAL(bgChanged(const QColor &)), this,
	  SLOT(slotBGColorSelected(const QColor &)));
  
  connect(m_pColorButton, SIGNAL(currentChanged(KDualColorButton::DualColor)), this,
		  SLOT(slotActiveColorChanged(KDualColorButton::DualColor )));
}

ActiveColor ControlFrame::activeColor()
{
  if (m_pColorButton->current() == KDualColorButton::Foreground)
	return ac_Foreground;
  else
	return ac_Background;
}

void ControlFrame::slotActiveColorChanged(KDualColorButton::DualColor s)
{
  if (s == KDualColorButton::Foreground)
	emit activeColorChanged(ac_Foreground);
  else
	emit activeColorChanged(ac_Background);
}

void ControlFrame::slotSetBrush(const KisBrush& b)
{
  m_pBrushWidget->slotSetBrush(b);
}

void ControlFrame::slotSetPattern(const KisPattern& b)
{
  //m_pPatternWidget->slotSetPattern(b); //jwc
}

void ControlFrame::resizeEvent ( QResizeEvent * )
{
  m_pColorButton->setGeometry( 4, 4, 34, 34 );
  m_pBrushWidget->setGeometry( 42, 4, 34, 34 );
  //m_pPatternWidget->setGeometry( 80, 4, 34, 34 ); //jwc
}

void ControlFrame::slotSetFGColor(const KisColor& c)
{
  m_pColorButton->setForeground( c.color() );
}

void ControlFrame::slotSetBGColor(const KisColor& c)
{
  m_pColorButton->setBackground( c.color() );
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
