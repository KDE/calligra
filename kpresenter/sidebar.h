/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <klistview.h>

class QListViewItem;
class KPresenterDoc;
class KPresenterView;
class QDropEvent;
class QPopupMenu;
class SideBarItem;

class SideBar : public KListView
{
    Q_OBJECT

public:
    SideBar( QWidget *parent, KPresenterDoc *d, KPresenterView *v );
    void setCurrentPage( int pg );
    void setOn( int pg, bool on );
    QSize sizeHint() const { return QSize( 120, KListView::sizeHint().height() ); }
    void updateItem( int pagenr );
    // Called by SideBarItem
    void itemStateChange( SideBarItem * item, bool state );

protected:
    void contentsDropEvent( QDropEvent *e );

signals: // all page numbers 0-based
    void showPage( int i );
    void movePage( int from, int to );
    void selectPage( int i, bool );

public slots:
    void rebuildItems();
    void renamePageTitle();	
private slots:
    void itemClicked( QListViewItem *i );
    void rightButtonPressed( QListViewItem *i, const QPoint &pnt, int c );
    void movedItems( QListViewItem *i, QListViewItem *firstAfter, QListViewItem *newAfter );
    void doMoveItems();

private:
    int delPageId;
    KPresenterDoc *doc;
    KPresenterView *view;
    QListViewItem *movedItem, *movedAfter;
};

#endif
