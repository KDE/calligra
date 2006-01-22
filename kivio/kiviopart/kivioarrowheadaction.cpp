/* This file is part of the KDE project
   Copyright (C) 2003 Peter Simonsson <psn@linux.se>,
   theKompany.com & Dave Marotti

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "kivioarrowheadaction.h"
#include "kivioglobal.h"

#include <qbitmap.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qpainter.h>
#include <qmenubar.h>
#include <qwhatsthis.h>

#include <kpopupmenu.h>
#include <ktoolbar.h>
#include <klocale.h>
#include <kdebug.h>
#include <kapplication.h>
#include <ktoolbarbutton.h>
#include <kstandarddirs.h>
#include <kiconloader.h>

KivioArrowHeadAction::KivioArrowHeadAction(const QString &text, const QString &pix,
  QObject* parent, const char *name)
  : KActionMenu(text, pix, parent, name)
{
  m_emitSignals = true;
  setShortcutConfigurable( false );
  m_popup = new KPopupMenu(0L,"KivioArrowHeadAction::popup");
  m_startPopup = new KPopupMenu;
  m_endPopup = new KPopupMenu;
  m_startPopup->setCheckable(true);
  m_endPopup->setCheckable(true);
  m_popup->insertItem(SmallIconSet("start_arrowhead", 16), i18n("Arrowhead at Origin"), m_startPopup);
  m_popup->insertItem(SmallIconSet("end_arrowhead", 16), i18n("Arrowhead at End"), m_endPopup);
  loadArrowHeads(m_startPopup);
  loadArrowHeads(m_endPopup);
  m_currentStart = m_currentEnd = 0;
  m_startPopup->setItemChecked(0, true);
  m_endPopup->setItemChecked(0, true);
  connect(m_startPopup, SIGNAL(activated(int)), SLOT(setCurrentStartArrow(int)));
  connect(m_endPopup, SIGNAL(activated(int)), SLOT(setCurrentEndArrow(int)));
}

KivioArrowHeadAction::~KivioArrowHeadAction()
{
  delete m_startPopup;
  m_startPopup = 0;
  delete m_endPopup;
  m_endPopup = 0;
  delete m_popup;
  m_popup = 0;
}

KPopupMenu* KivioArrowHeadAction::popupMenu() const
{
  return m_popup;
}

void KivioArrowHeadAction::popup( const QPoint& global )
{
  popupMenu()->popup(global);
}

int KivioArrowHeadAction::plug( QWidget* widget, int index)
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

void KivioArrowHeadAction::loadArrowHeads(KPopupMenu* popup)
{
  QBitmap mask;
  QPixmap pixAll = Kivio::arrowHeadPixmap();  
  QPixmap pix(pixAll.width(), 17);
  QPainter p(&pix, popup);
  int cindex = 0;

  // insert item "None"
  popup->insertItem(i18n("no line end", "None"),cindex++);

  for (int y = 0; y < pixAll.height(); y += 17 ) {
    pix.fill(white);
    p.drawPixmap(0, 0, pixAll, 0, y, pix.width(), pix.height());
    popup->insertItem(pix, cindex++);
  }

  p.end();
}

int KivioArrowHeadAction::currentStartArrow()
{
  return m_currentStart;
}

int KivioArrowHeadAction::currentEndArrow()
{
  return m_currentEnd;
}

void KivioArrowHeadAction::setCurrentStartArrow(int c)
{
  m_startPopup->setItemChecked(m_currentStart, false);
  m_currentStart = c;
  m_startPopup->setItemChecked(m_currentStart, true);

  if(m_emitSignals) {
    emit startChanged(m_currentStart);
  }
}

void KivioArrowHeadAction::setCurrentEndArrow(int c)
{
  m_endPopup->setItemChecked(m_currentEnd, false);
  m_currentEnd = c;
  m_endPopup->setItemChecked(m_currentEnd, true);

  if(m_emitSignals) {
    emit endChanged(m_currentEnd);
  }
}

#include "kivioarrowheadaction.moc"
