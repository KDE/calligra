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

#include "kolinestyleaction.h"

#include <qpainter.h>
#include <qpixmap.h>
#include <qbitmap.h>
#include <qwhatsthis.h>
#include <qmenubar.h>

#include <kpopupmenu.h>
#include <kapplication.h>
#include <kdebug.h>
#include <ktoolbar.h>
#include <ktoolbarbutton.h>
#include <kiconloader.h>
#include <klocale.h>

KoLineStyleAction::KoLineStyleAction(const QString &text, const QString& icon,
  QObject* parent, const char* name) : KActionMenu(text, icon, parent, name)
{
  m_popup = new KPopupMenu(0L,"KoLineStyleAction::popup");
  m_currentStyle = Qt::SolidLine;
  
  createMenu();
}

KoLineStyleAction::KoLineStyleAction(const QString &text, const QString& icon, const QObject* receiver,
  const char* slot, QObject* parent, const char* name) : KActionMenu(text, icon, parent, name)
{
  m_popup = new KPopupMenu(0L,"KoLineStyleAction::popup");
  m_currentStyle = Qt::SolidLine;
  
  createMenu();
  
  connect(this, SIGNAL(newLineStyle(int)), receiver, slot);
  connect(popupMenu(), SIGNAL(activated(int)), this, SLOT(execute(int)));
}

KoLineStyleAction::~KoLineStyleAction()
{
  delete m_popup;
  m_popup = 0;
}

void KoLineStyleAction::createMenu()
{
  KPopupMenu* popup = popupMenu();
  QBitmap mask;
  QPixmap pix(70, 21);
  QPainter p(&pix, popup);
  int cindex = 0;
  QPen pen;
  pen.setWidth(2);

  for(int i = 0; i < 6; i++) {
    pix.fill(white);
    pen.setStyle(static_cast<Qt::PenStyle>(i));
    p.setPen(pen);
    p.drawLine(0, 10, pix.width(), 10);
    mask = pix;
    pix.setMask(mask);
    popup->insertItem(pix,cindex++);
  }
}

KPopupMenu* KoLineStyleAction::popupMenu() const
{
  return m_popup;
}

void KoLineStyleAction::popup(const QPoint& global)
{
  popupMenu()->popup(global);
}

int KoLineStyleAction::plug(QWidget* widget, int index)
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

void KoLineStyleAction::execute(int index)
{
  setCurrentStyle(index);
  emit newLineStyle(m_currentStyle);
}

void KoLineStyleAction::setCurrentStyle(int style)
{
  popupMenu()->setItemChecked(m_currentStyle, false);
  popupMenu()->setItemChecked(style, true);
  m_currentStyle = style;
}

#include "kolinestyleaction.moc"
