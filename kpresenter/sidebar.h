#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <klistview.h>

class QListViewItem;
class KPresenterDoc;

class SideBar : public KListView
{
    Q_OBJECT
    
public:
    SideBar( QWidget *parent, KPresenterDoc *d );
    void setCurrentPage( int pg );
    void setOn( int pg, bool on );
    QSize sizeHint() const { return QSize( 120, KListView::sizeHint().height() ); }
    
signals:
    void showPage( int i );
    
public slots:
    void rebuildItems();

private slots:
    void itemClicked( QListViewItem *i );
    
private:
    KPresenterDoc *doc;
    
};

#endif
