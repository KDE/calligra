// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <qrect.h>
#include <qtabwidget.h>

#include <kiconview.h>
#include <klistview.h>

class QListViewItem;
class KPresenterDoc;
class KPresenterView;
class QDropEvent;
class QPopupMenu;
class Outline;
class OutlineSlideItem;
class ThumbBar;
class ThumbToolTip;
class KPrPage;

class SideBarBase
{
public:
    SideBarBase(KPresenterDoc *_doc, KPresenterView *_view);
    void setViewMasterPage( bool _b );
protected:
    KPresenterDoc *m_doc;
    KPresenterView *m_view;
    bool m_viewMasterPage;
};

class ThumbBar : public KIconView, public SideBarBase
{
    Q_OBJECT

public:
    ThumbBar(QWidget *parent, KPresenterDoc *d, KPresenterView *v);
    ~ThumbBar();
    void setCurrentPage( int pg );
    void updateItem( int pagenr, bool sticky = false );
    void addItem( int pos );
    void moveItem( int oldPos, int newPos );
    void removeItem( int pos );
    void refreshItems ( bool offset = false );
    QRect tip(const QPoint &pos, QString &title);

    bool uptodate;

signals:
    void showPage( int i );

public slots:
    void rebuildItems();

private slots:
    void itemClicked(QIconViewItem *i);
    void slotContentsMoving(int x, int y);
    void slotRefreshItems();

private:
    QPixmap getSlideThumb(int slideNr) const;

    ThumbToolTip *m_thumbTip;
    int m_offsetX;
    int m_offsetY;
};

class Outline: public KListView, public SideBarBase
{
    Q_OBJECT

public:
    Outline( QWidget *parent, KPresenterDoc *d, KPresenterView *v );
    ~Outline();
    void setCurrentPage( int pg );
    QSize sizeHint() const { return QSize( 145, KListView::sizeHint().height() ); }
    void updateItem( int pagenr, bool sticky = false);
    void addItem( int pos );
    void moveItem( int oldPos, int newPos );
    void removeItem( int pos );

protected:
    void contentsDropEvent( QDropEvent *e );
    void moveItem( QListViewItem *i, QListViewItem *firstAfter, QListViewItem *newAfter );
    OutlineSlideItem* slideItem( int pageNumber );
    bool acceptDrag( QDropEvent* e ) const;

    virtual QDragObject* dragObject();

signals: // all page numbers 0-based
    void showPage( int i );
    void movePage( int from, int to );
    void selectPage( int i, bool );

public slots:
    void rebuildItems();
    void renamePageTitle();

private slots:
    void itemClicked( QListViewItem *i );
    void slotDropped( QDropEvent *e, QListViewItem *parent, QListViewItem *target );
    void rightButtonPressed( QListViewItem *i, const QPoint &pnt, int c );

private:
    QListViewItem *m_movedItem, *m_movedAfter;
};

class SideBar: public QTabWidget
{
    Q_OBJECT

public:
    SideBar(QWidget *parent, KPresenterDoc *d, KPresenterView *v);
    void setCurrentPage( int pg ) {
        m_outline->setCurrentPage(pg);
        m_thb->setCurrentPage(pg);
    };
    void setOn( int , bool ) { };
    //QSize sizeHint() const { return QSize( 120, QTabWidget::sizeHint().height() ); };
    void updateItem( KPrPage *page );
    void addItem( int pos );
    void moveItem( int oldPos, int newPos );
    void removeItem( int pos );

    Outline *outline() const { return m_outline; };
    ThumbBar *thumbBar() const { return m_thb; };

    void setViewMasterPage( bool _masterPage );
signals: // all page numbers 0-based
    void showPage( int i );
    void movePage( int from, int to );
    void selectPage( int i, bool );

public slots:
    //void rebuildItems() { m_outline->rebuildItems(); m_thb->rebuildItems();};
    void renamePageTitle() { m_outline->renamePageTitle(); };
    void currentChanged(QWidget *tab);

private:
    Outline *m_outline;
    ThumbBar *m_thb;

    KPresenterDoc *m_doc;
    KPresenterView *m_view;
};

#endif
