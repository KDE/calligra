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
class OutlineItem;
class ThumbBar;
class ThumbToolTip;
class OutlineToolTip;

class ThumbBar : public KIconView
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

private:
  QPixmap getSlideThumb(int slideNr) const;

  KPresenterDoc *doc;
  KPresenterView *view;
  ThumbToolTip *thumbTip;
  int offsetX;
  int offsetY;
};



class Outline: public KListView
{
    Q_OBJECT

public:
    Outline( QWidget *parent, KPresenterDoc *d, KPresenterView *v );
    ~Outline();
    void setCurrentPage( int pg );
    void setOn( int pg, bool on );
    QSize sizeHint() const { return QSize( 145, KListView::sizeHint().height() ); }
    void updateItem( int pagenr );
    void addItem( int pos );
    void moveItem( int oldPos, int newPos );
    void removeItem( int pos );
    // Called by OutlineItem
    void itemStateChange( OutlineItem * item, bool state );
    QRect tip(const QPoint &pos, QString &title);

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
    OutlineToolTip *outlineTip;
};



class SideBar: public QTabWidget
{
  Q_OBJECT

public:
  SideBar(QWidget *parent, KPresenterDoc *d, KPresenterView *v);
  void setCurrentPage( int pg ) {
      _outline->setCurrentPage(pg);
      _thb->setCurrentPage(pg);
  };
  void setOn( int pg, bool on ) { _outline->setOn(pg, on); };
  //QSize sizeHint() const { return QSize( 120, QTabWidget::sizeHint().height() ); };
  void updateItem( int pagenr, bool sticky = false);
  void addItem( int pos );
  void moveItem( int oldPos, int newPos );
  void removeItem( int pos );

  Outline *outline() const { return _outline; };
  ThumbBar *thumbBar() const { return _thb; };

signals: // all page numbers 0-based
  void showPage( int i );
  void movePage( int from, int to );
  void selectPage( int i, bool );

public slots:
  //void rebuildItems() { _outline->rebuildItems(); _thb->rebuildItems();};
  void renamePageTitle() { _outline->renamePageTitle(); };
  void currentChanged(QWidget *tab);

private:
  Outline *_outline;
  ThumbBar *_thb;

  KPresenterDoc *doc;
  KPresenterView *view;
};

#endif
