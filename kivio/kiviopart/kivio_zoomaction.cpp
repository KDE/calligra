/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2001 theKompany.com & Dave Marotti
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include "kivio_zoomaction.h"
#include "tkcombobox.h"
#include <QRegExp>

#include <klocale.h>
#include <q3tl.h>

using namespace Kivio;

ZoomAction::ZoomAction(QObject* parent, const char* name)
: TKSelectAction(parent,name)
{
  setEditable(true);
  QStringList lst;
  lst << i18n("%1%").arg("33");
  lst << i18n("%1%").arg("50");
  lst << i18n("%1%").arg("75");
  lst << i18n("%1%").arg("100");
  lst << i18n("%1%").arg("125");
  lst << i18n("%1%").arg("150");
  lst << i18n("%1%").arg("200");
  lst << i18n("%1%").arg("250");
  lst << i18n("%1%").arg("350");
  lst << i18n("%1%").arg("400");
  lst << i18n("%1%").arg("450");
  lst << i18n("%1%").arg("500");
  setItems(lst);
}

ZoomAction::~ZoomAction()
{
}

void ZoomAction::slotActivated( const QString& text )
{
  QRegExp regexp("(\\d+)"); // "Captured" non-empty sequence of digits
  regexp.search(text);
  bool ok=false;
  // Use templates, not macro to avoid to call the functiosn many times.
  const int zoom=kMin(10000,qMax(10,regexp.cap(1).toInt(&ok)));
  insertItem(zoom);

  emit zoomActivated(zoom);
}

void ZoomAction::insertItem( int zoom )
{
  // This code is taken from KWords changeZoomMenu
  QValueList<int> list;
  bool ok;
  const QStringList itemsList(items());
  QRegExp regexp("(\\d+)"); // "Captured" non-empty sequence of digits

  for (QStringList::ConstIterator it = itemsList.begin() ; it != itemsList.end() ; ++it) {
    regexp.search(*it);
    const int val=regexp.cap(1).toInt(&ok);
    //zoom : limit inferior=10
    if(ok && val>9 && list.contains(val)==0)
      list.append( val );
  }
  //necessary at the beginning when we read config
  //this value is not in combo list
  if(list.contains(zoom)==0)
    list.append( zoom );

  qHeapSort( list );

  QStringList lst;
  for (QValueList<int>::Iterator it = list.begin() ; it != list.end() ; ++it)
    lst.append( i18n("%1%").arg(*it) );
  setItems(lst);
  setCurrentItem(lst.findIndex(i18n("%1%").arg(zoom)));
}

void ZoomAction::setEditZoom( int zoom )
{
  const QString zt(i18n("%1%").arg(zoom));
  setEditText(zt);
}
#include "kivio_zoomaction.moc"
