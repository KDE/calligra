/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000 theKompany.com
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
#include "kivio_zoomaction.h"
#include "tkcombobox.h"

ZoomAction::ZoomAction(QObject* parent, const char* name)
: TKSelectAction(parent,name)
{
  setEditable(true);
  QStringList lst;
  lst << "50%";
  lst << "75%";
  lst << "100%";
  lst << "150%";
  lst << "200%";
  lst << "250%";
  lst << "350%";
  lst << "400%";
  lst << "450%";
  lst << "500%";
  setItems(lst);
}

ZoomAction::~ZoomAction()
{
}

void ZoomAction::slotActivated( const QString& text )
{
  QString zt(text);
  zt = zt.replace(QRegExp("%"),"");
  zt = zt.simplifyWhiteSpace();
  int zoom = QMIN(10000,QMAX(5,zt.toInt()));
  insertItem(zoom);

  emit zoomActivated(zoom);
}

void ZoomAction::insertItem( int item )
{
  QString zt = QString("%1%").arg(item);

  QStringList i = items();
  if (i.contains(zt)==0) {
    i.append(zt);

    // sort items
    QStringList::Iterator it;
    for( it = i.begin(); it != i.end(); ++it ) {
      QString base;
      base.fill(' ',7-(*it).length());
      (*it) = base + (*it);
    }
    i.sort();
    for( it = i.begin(); it != i.end(); ++it ) {
      (*it) = (*it).simplifyWhiteSpace();;
    }
  }
  setItems(i);
  setCurrentItem(i.findIndex(zt));
}

void ZoomAction::setEditZoom( int zoom )
{
  QString zt = QString("%1%").arg(zoom);
  setEditText(zt);
}
#include "kivio_zoomaction.moc"
