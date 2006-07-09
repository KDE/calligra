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

#ifndef KIVIOSTENCILSETACTION_H
#define KIVIOSTENCILSETACTION_H


#include <kaction.h>

#include <qptrlist.h>
#include <qvaluelist.h>
#include <qstringlist.h>

class KPopupMenu;
class QPixmap;

class KivioStencilSetAction : public KAction
{
  Q_OBJECT
  public:
    KivioStencilSetAction(const QString &text, const QString &pix,
      KActionCollection *parent, const char *name);

    virtual ~KivioStencilSetAction();

    KPopupMenu* popupMenu() const;
    void popup( const QPoint& global );

    virtual int plug( QWidget* widget, int index = -1 );

  public slots:
    void updateMenu();

  protected slots:
    void slotActivated(int);

  signals:
    void activated( const QString& );
    void showDialog();

  protected:
    QPixmap dirtPixmap( const QString& dir );
    void loadCollections( const QString& rootDirStr );
    void loadSet( KPopupMenu* menu, const QString& rootDirStr );
    void clearCollectionMenuList();

  protected:
    KPopupMenu* m_popup;
    uint m_ssId;
    QPtrList<QString> m_pathList;
    QStringList m_collectionIdList;
    QValueList<KPopupMenu*> m_collectionMenuList;
};

#endif
