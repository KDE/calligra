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

#include <sidebar.h>
#include <kpresenter_view.h>
#include <kmessagebox.h>
#include <klineeditdlg.h>
#include <knotifyclient.h>
#include <qheader.h>
#include <qtimer.h>
#include <qpopupmenu.h>
#include <kdebug.h>

class SideBarItem : public QCheckListItem
{
public:
    SideBarItem( QListView * parent )
     : QCheckListItem( parent, QString::null, QCheckListItem::CheckBox )
     {}

    virtual void stateChange(bool b)
    {
        static_cast<SideBar*>(listView())->itemStateChange( this, b );
    }
};

SideBar::SideBar( QWidget *parent, KPresenterDoc *d, KPresenterView *v )
    : KListView( parent ), doc( d ), view( v )
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
    connect( this, SIGNAL( rightButtonPressed( QListViewItem *, const QPoint &, int ) ),
             this, SLOT( rightButtonPressed( QListViewItem *, const QPoint &, int ) ) );
    setAcceptDrops( TRUE );
    setDropVisualizer( TRUE );
    setDragEnabled( TRUE );

    pageMenu = new QPopupMenu();
    CHECK_PTR( pageMenu );
    pageMenu->insertItem( i18n( "&Insert Page..." ), this, SLOT( pageInsert() ) );
    pageMenu->insertItem( i18n( "&Use current slide as default template" ), this, SLOT( pageDefaultTemplate() ) );
    pageMenu->insertItem( KPBarIcon( "newslide" ), i18n( "&Duplicate Page" ), this, SLOT( duplicateCopy() ) );
    delPageId = pageMenu->insertItem( KPBarIcon( "delslide" ), i18n( "D&elete Page..." ), this, SLOT( pageDelete() ) );
    pageMenu->insertItem( i18n( "&Rename Page" ), this, SLOT( renamePageTitle() ) );
    pageMenu->setMouseTracking( true );
}

void SideBar::rebuildItems()
{
    clear();
    // Rebuild all the items
    for ( int i = doc->getPageNums() - 1; i >= 0; --i ) {
        QCheckListItem *item = new SideBarItem( this );
        QString title = doc->getPageTitle( i, i18n( "Slide %1" ).arg( i + 1 ) );
        //kdDebug() << "SideBar::rebuildItems slide " << i+1 << " selected:" << doc->isSlideSelected( i ) << endl;
        item->setOn( doc->isSlideSelected( i ) ); // calls itemStateChange !
        item->setText( 1, QString::number( i + 1 ) ); // page number
        item->setText( 0, title );
    }
}

void SideBar::updateItem( int pagenr /* 0-based */)
{
    // Find item
    QListViewItemIterator it( this );
    for ( ; it.current(); ++it )
    {
        if ( it.current()->text(1).toInt() == pagenr+1 )
        {
            QString title = doc->getPageTitle( pagenr, i18n( "Slide %1" ).arg( pagenr + 1 ) );
            it.current()->setText( 0, title );
            it.current()->setText( 1, QString::null ); // hack, to make itemStateChange do nothing
            static_cast<SideBarItem*>(it.current())->setOn( doc->isSlideSelected( pagenr ) );
            it.current()->setText( 1, QString::number( pagenr + 1 ) ); // page number
            return;
        }
    }
    kdWarning() << "Item for page " << pagenr << " not found" << endl;
}

void SideBar::itemStateChange( SideBarItem * item, bool state )
{
    QString text = item->text( 1 );
    if ( !text.isEmpty() ) // empty if we are called from rebuildItems
        emit selectPage( text.toInt() - 1, state );
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

void SideBar::pageInsert()
{
    view->insertPage();
}

void SideBar::duplicateCopy()
{
    view->editDuplicatePage();
}

void SideBar::pageDelete()
{
    view->editDelPage();
}

void SideBar::pageDefaultTemplate()
{
    view->extraDefaultTemplate();
}

void SideBar::rightButtonPressed( QListViewItem *, const QPoint &pnt, int )
{
    if ( !selectedItem() || !doc->isReadWrite())
        return;

    pageMenu->setItemEnabled( delPageId, doc->getPageNums() > 1 );
    pageMenu->popup( pnt );
}

void SideBar::renamePageTitle()
{
    int pageNumber = QListView::selectedItem()->text( 1 ).toInt() - 1;
    bool ok;
    QString activeTitle = QListView::selectedItem()->text( 0 );
    QString newTitle = KLineEditDlg::getText( i18n("Page Title"), activeTitle, &ok, this );

    // Have a different name ?
    if ( ok ) { // User pushed an OK button.
        if ( (newTitle.stripWhiteSpace()).isEmpty() ) { // Title is empty.
            KNotifyClient::beep();
            KMessageBox::information( this, i18n("Page title cannot be empty."), i18n("Change page title") );
            // Recursion
            renamePageTitle();
        }
        else if ( newTitle != activeTitle ) { // Title changed.
            (*doc->manualTitleList.at(pageNumber)) = newTitle;
            updateItem(pageNumber);
            view->kPresenterDoc()->setModified( true );
        }
    }
}


#include <sidebar.moc>
