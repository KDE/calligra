#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <klistview.h>

class QListViewItem;
class KPresenterDoc;
class KPresenterView;
class QDropEvent;
class QPopupMenu;

class SideBar : public KListView
{
    Q_OBJECT

public:
    SideBar( QWidget *parent, KPresenterDoc *d, KPresenterView *v );
    void setCurrentPage( int pg );
    void setOn( int pg, bool on );
    QSize sizeHint() const { return QSize( 120, KListView::sizeHint().height() ); }

protected:
    void contentsDropEvent( QDropEvent *e );

signals:
    void showPage( int i );
    void movePage( int from, int to );

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
    KPresenterDoc *doc;
    KPresenterView *view;
    QListViewItem *movedItem, *movedAfter;
    QPopupMenu *pageMenu;
};

#endif
