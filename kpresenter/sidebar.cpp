#include "sidebar.h"
#include "kpresenter_doc.h"
#include <qheader.h>
#include <qtimer.h>

SideBar::SideBar( QWidget *parent, KPresenterDoc *d )
    : KListView( parent ), doc( d )
{
    rebuildItems();
    setSorting( -1 );
    header()->hide();
    addColumn( i18n( "Slide" ) );
    addColumn( i18n( "Number" ) );
    setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Expanding ) );
    connect( this, SIGNAL( currentChanged( QListViewItem * ) ), this, SLOT( itemClicked( QListViewItem * ) ) );
    connect( this, SIGNAL( moved( QListViewItem *, QListViewItem *, QListViewItem * ) ),
	     this, SLOT( movedItems( QListViewItem *, QListViewItem *, QListViewItem * ) ) );
    setAcceptDrops( TRUE );
    setDropVisualizer( TRUE );
    setDragEnabled( TRUE );
}

void SideBar::rebuildItems()
{
    QMap< QString, bool > checkedMap;
    QListViewItemIterator it( this );
    for ( ; it.current(); ++it )
	checkedMap.insert( it.current()->text( 0 ), ( (QCheckListItem*)it.current() )->isOn() );

    clear();
    for ( int i = doc->getPageNums() - 1; i >= 0; --i ) {
	QCheckListItem *item = new QCheckListItem( this, "", QCheckListItem::CheckBox );
	QString title = doc->getPageTitle( i, i18n( "Slide %1" ).arg( i + 1 ) );
	QMap< QString, bool >::Iterator bit;
	item->setOn( ( bit = checkedMap.find( title ) ) != checkedMap.end() ? *bit : TRUE );
	item->setText( 1, QString( "%1" ).arg( i + 1 ) );
	item->setText( 0, title );
    }
    setCurrentItem( firstChild() );
    setSelected( firstChild(), TRUE );
}

void SideBar::itemClicked( QListViewItem *i )
{
    if ( !i )
	return;
    emit showPage( i->text( 1 ).toInt() - 1 );
}


void SideBar::setCurrentPage( int pg )
{
    QListViewItemIterator it( this );
    for ( ; it.current(); ++it ) {
	if ( it.current()->text( 1 ).toInt() - 1 == pg ) {
	    setCurrentItem( it.current() );
	    setSelected( it.current(), TRUE );
	}
    }
}

void SideBar::setOn( int pg, bool on )
{
    QListViewItemIterator it( this );
    for ( ; it.current(); ++it ) {
	if ( it.current()->text( 1 ).toInt() - 1 == pg ) {
	    ( (QCheckListItem*)it.current() )->setOn( on );
	    return;
	}
    }
}

void SideBar::contentsDropEvent( QDropEvent *e )
{
    disconnect( this, SIGNAL( currentChanged( QListViewItem * ) ), this, SLOT( itemClicked( QListViewItem * ) ) );
    KListView::contentsDropEvent( e );
    connect( this, SIGNAL( currentChanged( QListViewItem * ) ), this, SLOT( itemClicked( QListViewItem * ) ) );
}

void SideBar::movedItems( QListViewItem *i, QListViewItem *, QListViewItem *newAfter )
{
    movedItem = i;
    movedAfter = newAfter;
    QTimer::singleShot( 300, this, SLOT( doMoveItems() ) );
		 
}

void SideBar::doMoveItems()
{
    int num = movedItem->text( 1 ).toInt() - 1;
    int numNow;
    if ( !movedAfter ) {
	numNow = 0;
    } else { 
	numNow = movedAfter->text( 1 ).toInt();
	if ( numNow > num )
	    numNow--;
    }
    emit movePage( num, numNow );
}
