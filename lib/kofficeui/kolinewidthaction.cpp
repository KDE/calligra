/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2004 Peter Simonsson
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "kolinewidthaction.h"

#include <qpainter.h>
#include <qpixmap.h>
#include <qbitmap.h>
#include <qwhatsthis.h>
#include <qmenubar.h>
#include <qlayout.h>
#include <qlabel.h>

#include <kpopupmenu.h>
#include <kapplication.h>
#include <kdebug.h>
#include <ktoolbar.h>
#include <ktoolbarbutton.h>
#include <kiconloader.h>
#include <klocale.h>

#include <kozoomhandler.h>
#include <koUnitWidgets.h>

KoLineWidthAction::KoLineWidthAction(const QString &text, const QString& icon,
  QObject* parent, const char* name) : KActionMenu(text, icon, parent, name)
{
  m_popup = new KPopupMenu(0L,"KivioArrowHeadAction::popup");
  m_currentIndex = 0;
  m_currentWidth = 1.0;
  m_unit = KoUnit::U_PT;
  
  createMenu();
}

KoLineWidthAction::KoLineWidthAction(const QString &text, const QString& icon, const QObject* receiver,
  const char* slot, QObject* parent, const char* name) : KActionMenu(text, icon, parent, name)
{
  m_popup = new KPopupMenu(0L,"KivioArrowHeadAction::popup");
  m_currentIndex = 0;
  m_currentWidth = 1.0;
  
  createMenu();
  
  connect(this, SIGNAL(newLineWidth(double)), receiver, slot);
  connect(popupMenu(), SIGNAL(activated(int)), this, SLOT(execute(int)));
}

KoLineWidthAction::~KoLineWidthAction()
{
  delete m_popup;
  m_popup = 0;
}

void KoLineWidthAction::createMenu()
{
  KPopupMenu* popup = popupMenu();
  KoZoomHandler zoom;
  zoom.setZoomAndResolution(100, QPaintDevice::x11AppDpiX(), QPaintDevice::x11AppDpiY());
  QBitmap mask;
  QPixmap pix(70, 21);
  QPainter p(&pix, popup);
  int cindex = 0;
  QPen pen;

  for(int i = 1; i <= 10; i++) {
    pix.fill(white);
    pen.setWidth(zoom.zoomItY(i));
    p.setPen(pen);
    p.drawLine(0, 10, pix.width(), 10);
    mask = pix;
    pix.setMask(mask);
    popup->insertItem(pix,cindex++);
  }
  
  popup->insertSeparator(cindex++);
  popup->insertItem(i18n("&Custom..."), cindex++);
}

KPopupMenu* KoLineWidthAction::popupMenu() const
{
  return m_popup;
}

void KoLineWidthAction::popup(const QPoint& global)
{
  popupMenu()->popup(global);
}

int KoLineWidthAction::plug(QWidget* widget, int index)
{
  // This function is copied from KActionMenu::plug
  if (kapp && !kapp->authorizeKAction(name()))
    return -1;
  kdDebug(129) << "KAction::plug( " << widget << ", " << index << " )" << endl; // remove -- ellis
  if ( widget->inherits("QPopupMenu") )
  {
    QPopupMenu* menu = static_cast<QPopupMenu*>( widget );
    int id;

    if ( hasIconSet() )
      id = menu->insertItem( iconSet(), text(), popupMenu(), -1, index );
    else
      id = menu->insertItem( kapp->iconLoader()->loadIcon(icon(), KIcon::Small),
        text(), popupMenu(), -1, index );

    if ( !isEnabled() )
      menu->setItemEnabled( id, false );

    addContainer( menu, id );
    connect( menu, SIGNAL( destroyed() ), this, SLOT( slotDestroyed() ) );

    return containerCount() - 1;
  }
  else if ( widget->inherits( "KToolBar" ) )
  {
    KToolBar *bar = static_cast<KToolBar *>( widget );

    int id_ = KAction::getToolButtonID();

    if ( icon().isEmpty() && !iconSet().isNull() )
      bar->insertButton( iconSet().pixmap(), id_, SIGNAL( clicked() ), this,
                          SLOT( slotActivated() ), isEnabled(), plainText(),
                          index );
    else
    {
      KInstance *instance;

      if ( m_parentCollection )
        instance = m_parentCollection->instance();
      else
        instance = KGlobal::instance();

      bar->insertButton( icon(), id_, SIGNAL( clicked() ), this,
                          SLOT( slotActivated() ), isEnabled(), plainText(),
                          index, instance );
    }

    addContainer( bar, id_ );

    if (!whatsThis().isEmpty())
      QWhatsThis::add( bar->getButton(id_), whatsThis() );

    connect( bar, SIGNAL( destroyed() ), this, SLOT( slotDestroyed() ) );

    bar->getButton(id_)->setPopup(popupMenu(), true );

    return containerCount() - 1;
  }
  else if ( widget->inherits( "QMenuBar" ) )
  {
    QMenuBar *bar = static_cast<QMenuBar *>( widget );

    int id;

    id = bar->insertItem( text(), popupMenu(), -1, index );

    if ( !isEnabled() )
      bar->setItemEnabled( id, false );

    addContainer( bar, id );
    connect( bar, SIGNAL( destroyed() ), this, SLOT( slotDestroyed() ) );

    return containerCount() - 1;
  }

  return -1;
}

void KoLineWidthAction::execute(int index)
{
  bool ok = false;
  
  if((index >= 0) && (index < 10)) {
    m_currentWidth = (double) index + 1.0;
    ok = true;
  } if(index == 11) { // Custom width dialog...
    KoLineWidthChooser dlg;
    
    if(dlg.exec()) {
      m_currentWidth = dlg.width();
      ok = true;
    }
  }

  if(ok) {
    popupMenu()->setItemChecked(m_currentIndex, false);
    popupMenu()->setItemChecked(index, true);
    m_currentIndex = index;
    emit newLineWidth(m_currentWidth);
  }
}

void KoLineWidthAction::setCurrentWidth(double width)
{
  m_currentWidth = width;
  
  // Check if it is a standard width...
  for(int i = 1; i <= 10; i++) {
    if(KoUnit::toPoint(width) == (double) i) {
      popupMenu()->setItemChecked(m_currentIndex, false);
      popupMenu()->setItemChecked(i - 1, true);
      m_currentIndex = i - 1;
      return;
    }
  }

  //Custom width...
  popupMenu()->setItemChecked(m_currentIndex, false);
  popupMenu()->setItemChecked(11, true);
  m_currentIndex = 11;
}

void KoLineWidthAction::setUnit(KoUnit::Unit unit)
{
  m_unit = unit;
}

//////////////////////////////////////////////////
//
// KoLineWidthChooser
//

KoLineWidthChooser::KoLineWidthChooser(QWidget* parent, const char* name)
 : KDialogBase(parent, name, true, i18n("Custom Line Width"), Ok|Cancel, Ok)
{
  m_unit = KoUnit::U_PT;

  // Create the ui
  QWidget* mainWidget = new QWidget(this);
  setMainWidget(mainWidget);
  QGridLayout* gl = new QGridLayout(mainWidget, 1, 2, KDialog::marginHint(), KDialog::spacingHint());
  QLabel* textLbl = new QLabel(i18n("Line Width:"), mainWidget);
  m_lineWidthUSBox = new KoUnitDoubleSpinBox(mainWidget, 0.0, 1000.0, 0.1, 1.0, m_unit, 2);
  gl->addWidget(textLbl, 0, 0);
  gl->addWidget(m_lineWidthUSBox, 0, 1);
}

double KoLineWidthChooser::width()
{
  return KoUnit::ptFromUnit(m_lineWidthUSBox->value(), m_unit);
}

void KoLineWidthChooser::setUnit(KoUnit::Unit unit)
{
  m_unit = unit;
  m_lineWidthUSBox->setUnit(unit);
}

void KoLineWidthChooser::setWidth(double width)
{
  m_lineWidthUSBox->changeValue(KoUnit::ptToUnit(width, m_unit));
}

#include "kolinewidthaction.moc"
