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

private slots:
    void itemClicked( QListViewItem *i );
    void rightButtonPressed( QListViewItem *i, const QPoint &pnt, int c );
    void movedItems( QListViewItem *i, QListViewItem *firstAfter, QListViewItem *newAfter );
    void doMoveItems();
    void pageDefaultTemplate();
    void duplicateCopy();
    void pageDelete();
    void pageInsert();

private:
    int delPageId;
    KPresenterDoc *doc;
    KPresenterView *view;
    QListViewItem *movedItem, *movedAfter;
    QPopupMenu *pageMenu;
};

#endif
