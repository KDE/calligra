/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2001 Lukas Tinkl <lukas@kde.org>

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

#include <qheader.h>
#include <qtimer.h>
#include <qpopupmenu.h>
#include <qimage.h>
#include <qtabwidget.h>
#include <qtooltip.h>

#include <kmessagebox.h>
#include <klocale.h>
#include <klineeditdlg.h>
#include <knotifyclient.h>
#include <kiconview.h>
#include <kdebug.h>

#include "sidebar.h"
#include "kpresenter_view.h"
#include "kprcanvas.h"
#include <qapplication.h>
#include "kprcommand.h"

class ThumbToolTip : public QToolTip
{
public:
    ThumbToolTip(QWidget *parent)
     : QToolTip(parent)
     {}

protected:
    void maybeTip(const QPoint &pos)
    {
        QString title;
        QRect r(((ThumbBar*)parentWidget())->tip(pos, title));
        if (!r.isValid())
            return;

        tip(r, title);
    }
};

class OutlineToolTip : public QToolTip
{
public:
    OutlineToolTip(QWidget *parent)
     : QToolTip(parent)
     {}

protected:
    void maybeTip( const QPoint &pos )
    {
        QString title;
        QRect r(((Outline*)parentWidget())->tip(pos, title));
        if (!r.isValid())
            return;

        tip(r, title);
    }
};

class OutlineItem: public QCheckListItem
{
public:
    OutlineItem( QListView * parent )
     : QCheckListItem( parent, QString::null, QCheckListItem::CheckBox )
     {}

    virtual void stateChange(bool b)
    {
        static_cast<Outline*>(listView())->itemStateChange( this, b );
    }
};

class ThumbItem : public QIconViewItem
{
public:
    ThumbItem( QIconView *parent, const QString & text, const QPixmap & icon )
     : QIconViewItem( parent, text, icon )
     { uptodate = true; }
    ThumbItem( QIconView *parent, QIconViewItem *after, const QString & text, const QPixmap & icon )
     : QIconViewItem( parent, after, text, icon )
     { uptodate = true; }

    virtual bool isUptodate() { return uptodate; };
    virtual void setUptodate( bool _uptodate) { uptodate = _uptodate; };

private:
    bool uptodate;
};

SideBar::SideBar(QWidget *parent, KPresenterDoc *d, KPresenterView *v)
  :QTabWidget(parent), doc(d), view(v)
{
  setTabPosition(QTabWidget::Top);
  setTabShape(QTabWidget::Triangular);

  _outline = new Outline(this, doc, view);
  addTab(_outline, i18n("Outline"));

  _thb = new ThumbBar(this, doc, view);
  addTab(_thb,i18n("Thumbs"));


  //TODO find a better way
  connect(_outline, SIGNAL(showPage(int)),
          this, SIGNAL(showPage(int)));

  connect(_thb, SIGNAL(showPage(int)),
          this, SIGNAL(showPage(int)));

  connect(_outline, SIGNAL(movePage(int,int)),
          this, SIGNAL(movePage(int,int)));

  connect(_outline, SIGNAL(selectPage(int,bool)),
          this, SIGNAL(selectPage(int,bool)));

  connect(this, SIGNAL(currentChanged(QWidget *)),
          this, SLOT(currentChanged(QWidget *)));

}

void SideBar::currentChanged(QWidget *tab)
{
    if (tab == _thb) {
        if (!_thb->uptodate && _thb->isVisible())
            _thb->rebuildItems();
        else
            _thb->refreshItems();
    }
}

void SideBar::addItem( int pos )
{
    _outline->addItem( pos );
    _thb->addItem( pos );
}

void SideBar::moveItem( int oldPos, int newPos )
{
    _outline->moveItem( oldPos, newPos );
    _thb->moveItem( oldPos, newPos );
}

void SideBar::removeItem( int pos )
{
    _outline->removeItem( pos );
    _thb->removeItem( pos );
}

void SideBar::updateItem( int pos, bool sticky )
{
    //sticky page pos = -1
    if ( pos >= 0)
        _outline->updateItem( pos );
    _thb->updateItem( pos, sticky );
}

ThumbBar::ThumbBar(QWidget *parent, KPresenterDoc *d, KPresenterView *v)
  :KIconView(parent), doc(d), view(v)
{
  uptodate = false;
  offsetX = 0;
  offsetY = 0;

  setArrangement(QIconView::LeftToRight);
  setAutoArrange(true);
  setSorting(false);
  setItemsMovable(false);
  setResizeMode(QIconView::Adjust);

  thumbTip = new ThumbToolTip(this);

  connect(this, SIGNAL(currentChanged(QIconViewItem *)),
          this, SLOT(itemClicked(QIconViewItem *)));
  connect(this, SIGNAL(contentsMoving(int, int)),
          this, SLOT(slotContentsMoving(int, int)));
}

ThumbBar::~ThumbBar()
{
    delete thumbTip;
}

void ThumbBar::setCurrentPage( int pg )
{
    for ( QIconViewItem *it = firstItem(); it; it = it->nextItem() )
    {
        if ( it->text().toInt() - 1 == pg ) {
            setCurrentItem( it );
            setSelected( it, FALSE ); // to avoid the blue "selected"-mark
            ensureItemVisible(it);
            refreshItems();
            return;
        }
    }

}

QRect ThumbBar::tip(const QPoint &pos, QString &title)
{
    QIconViewItem *item = findItem(viewportToContents(pos));
    if (!item)
        return QRect(0, 0, -1, -1);

    int pagenr =  item->index();
    title = doc->pageList().at(pagenr)->pageTitle(i18n("Slide %1").arg(pagenr + 1));

    QRect r = item->pixmapRect(FALSE);
    r = QRect(contentsToViewport(QPoint(r.x(), r.y())), QSize(r.width(), r.height()));
    return r;
}

void ThumbBar::rebuildItems()
{
    if( !isVisible())
        return;
    kdDebug(33001) << "ThumbBar::rebuildItems" << endl;

    QApplication::setOverrideCursor( Qt::waitCursor );

    clear();
    for ( unsigned int i = 0; i < doc->getPageNums(); i++ ) {
        ThumbItem *item = new ThumbItem(static_cast<QIconView *>(this), QString::number(i+1), getSlideThumb(i));
        item->setDragEnabled(false);  //no dragging for now
    }

    uptodate = true;

    QApplication::restoreOverrideCursor();
}

void ThumbBar::refreshItems(bool offset)
{
    QRect vRect = visibleRect();
    if ( offset ) {
        vRect.moveBy( offsetX, offsetY );
    }
    else {
        vRect.moveBy( contentsX(), contentsY() );
    }

    QIconViewItem *it = findFirstVisibleItem( vRect );
    while ( it )
    {
kdDebug(33001) << "visible page = " << it->text().toInt() << endl;
        if ( ! dynamic_cast<ThumbItem *>(it)->isUptodate( ) ){
            //todo refresh picture
            it->setPixmap( getSlideThumb( it->text().toInt() - 1 ) );
            static_cast<ThumbItem *>(it)->setUptodate( true );
        }

        if ( it == findLastVisibleItem( vRect ) )
            break;
        it = it->nextItem();
    }

    offsetX = 0;
    offsetY = 0;
}

void ThumbBar::updateItem( int pagenr /* 0-based */, bool sticky )
{
    if ( !uptodate )
        return;
    int pagecnt = 0;
    // calculate rect of visible objects
    QRect vRect = visibleRect();
    vRect.moveBy( contentsX(), contentsY() );

    // Find icon
    QIconViewItem *it = firstItem();
    do
    {
        //Commented out until only the visible objects are updated
        if ( sticky ) {
            if ( it == findFirstVisibleItem( vRect ) ) {
                //bool cont = true;
                do
                {
                    it->setPixmap(getSlideThumb( pagecnt ));
                    static_cast<ThumbItem *>(it)->setUptodate( true );
                    if ( it == findLastVisibleItem( vRect ) )
                        break;
                    pagecnt++;
                    it = it->nextItem();
                } while ( true );
            }
            else {
                static_cast<ThumbItem *>(it)->setUptodate( false );
            }
            pagecnt++;
        }
        else {
            if ( it->text().toInt() == pagenr + 1 )
            {
                it->setPixmap(getSlideThumb( pagenr ));
                static_cast<ThumbItem *>(it)->setUptodate( true );
                return;
            }
        }
        it = it->nextItem();
    } while ( it );
    if ( ! sticky )
        kdWarning() << "Item for page " << pagenr << " not found" << endl;
}

// add a thumb item without recreating all thumbs
void ThumbBar::addItem( int pos )
{
    kdDebug(33001)<< "ThumbBar::addItem" << endl;
    int page = 0;
    for ( QIconViewItem *it = firstItem(); it; it = it->nextItem() ) {
        // find page which should move
        // do stuff because a item can not be insert at the beginning
        if ( pos == 0 && page == pos ){
            ThumbItem *item = new ThumbItem(static_cast<QIconView *>(this), it, QString::number(2), getSlideThumb(1));
            item->setDragEnabled(false);  //no dragging for now
            it->setPixmap(getSlideThumb( 0 ));
            // move on to next item as we have inserted one
            it = it->nextItem();
        }
        else if ( (page + 1) == pos ) {
            ThumbItem *item = new ThumbItem(static_cast<QIconView *>(this), it, QString::number(pos+1), getSlideThumb(pos));
            item->setDragEnabled(false);  //no dragging for now
            it = it->nextItem();
        }
        // update page numbers
        if ( page >= pos ) {
            it->setText( QString::number(page+2) );
        }
        page++;
    }
}

// moves a item without recreating all pages
void ThumbBar::moveItem( int oldPos, int newPos )
{
    int page = 0;
    QIconViewItem *after = 0;
    QIconViewItem *take = 0;
    for ( QIconViewItem *it = firstItem(); it; it = it->nextItem() ) {
        // find page which should move
        if ( page == oldPos ) {
            take = it;
        }
        // find position where page should be insert
        // as a page can not be insert at the beginning get the first one
        // the page to get depends on if a page is moved forward / backwards
        if ( page == newPos ) {
            after = page == 0 ? it : newPos > oldPos ? it : it->prevItem();
        }
        page++;
    }

    if ( ! take )
        return;

    takeItem( take );
    insertItem( take, after);
    // update the thumbs if new pos was 0
    // because it was insert after the first one
    if ( newPos == 0 ) {
        //todo do not recreate the pics
        after->setPixmap(getSlideThumb( 0 ));
        take->setPixmap(getSlideThumb( 1 ));
    }

    //write the new page numbers
    int lowPage = oldPos > newPos ? newPos : oldPos;
    int highPage = oldPos < newPos ? newPos : oldPos;
    page = 0;
    for ( QIconViewItem *it = firstItem(); it; it = it->nextItem() ) {
        if ( page >= lowPage && page <= highPage) {
            it->setText( QString::number(page+1) );
        }
        page++;
    }
}

void ThumbBar::removeItem( int pos )
{
    kdDebug(33001)<< "ThumbBar::removeItem" << endl;
    int page = 0;
    bool change = false;
    QIconViewItem *itemToDelete = 0;

    for ( QIconViewItem *it = firstItem(); it; it = it->nextItem() ) {
        if ( page == pos ) {
            itemToDelete = it;
            if ( it->nextItem() )
                it = it->nextItem();
                change = true;
        }
        if ( change ) {
            it->setText( QString::number( page + 1 ) );
        }
        page++;
    }
    delete itemToDelete;
}

QPixmap ThumbBar::getSlideThumb(int slideNr) const
{
  //kdDebug(33001) << "ThumbBar::getSlideThumb: " << slideNr << endl;
  QPixmap pix( 10, 10 );

  view->getCanvas()->drawPageInPix( pix, slideNr, 60 );

  int w = pix.width();
  int h = pix.height();

  if ( w > h ) {
      w = 130;
      h = 120;
  }
  else if ( w < h ) {
      w = 120;
      h = 130;
  }
  else if ( w == h ) {
      w = 130;
      h = 130;
  }

  const QImage img(pix.convertToImage().smoothScale( w, h, QImage::ScaleMin ));
  pix.convertFromImage(img);

  // draw a frame around the thumb to show its size
  QPainter p(&pix);
  p.setPen(Qt::black);
  p.drawRect(pix.rect());

  return pix;
}

void ThumbBar::itemClicked(QIconViewItem *i)
{
  if ( !i )
    return;
  emit showPage( i->index() );
}

void ThumbBar::slotContentsMoving(int x, int y)
{
    offsetX = x;
    offsetY = y;
kdDebug() << "offset x,y = " << x << ", " << y << endl;
    refreshItems( true );
}

Outline::Outline( QWidget *parent, KPresenterDoc *d, KPresenterView *v )
    : KListView( parent ), doc( d ), view( v )
{
    rebuildItems();
    setSorting( -1 );
    header()->hide();
    addColumn( i18n( "Slide" ) );
    addColumn( i18n( "Number" ) );
    setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Expanding ) );

    outlineTip = new OutlineToolTip(this);

    connect( this, SIGNAL( currentChanged( QListViewItem * ) ), this, SLOT( itemClicked( QListViewItem * ) ) );
    connect( this, SIGNAL( moved( QListViewItem *, QListViewItem *, QListViewItem * ) ),
             this, SLOT( movedItems( QListViewItem *, QListViewItem *, QListViewItem * ) ) );
    connect( this, SIGNAL( rightButtonPressed( QListViewItem *, const QPoint &, int ) ),
             this, SLOT( rightButtonPressed( QListViewItem *, const QPoint &, int ) ) );

    connect( this, SIGNAL( doubleClicked ( QListViewItem * )),
             this, SLOT(renamePageTitle()));

    setAcceptDrops( TRUE );
    setDropVisualizer( TRUE );
    setDragEnabled( TRUE );

}

Outline::~Outline()
{
    delete outlineTip;
}

void Outline::rebuildItems()
{
    clear();
    // Rebuild all the items
    for ( int i = doc->getPageNums() - 1; i >= 0; --i ) {
        QCheckListItem *item = new OutlineItem( this );
        QString title = doc->pageList().at(i)->pageTitle( i18n( "Slide %1" ).arg( i + 1 ) );
        //kdDebug(33001) << "Outline::rebuildItems slide " << i+1 << " selected:" << doc->isSlideSelected( i ) << endl;
        item->setOn( doc->isSlideSelected( i ) ); // calls itemStateChange !
        item->setText( 1, QString::number( i + 1 ) ); // page number
        if (title.length() > 12) // restrict to a maximum of 12 characters
            item->setText(0, title.left(5) + "..." + title.right(4));
        else
            item->setText( 0, title );
    }
}

// update the Outline item, the title my have changed
void Outline::updateItem( int pagenr /* 0-based */)
{
    // Find item
    QListViewItemIterator it( this );
    for ( ; it.current(); ++it )
    {
        if ( it.current()->text(1).toInt() == pagenr+1 )
        {
            QString title = doc->pageList().at(pagenr)->pageTitle( i18n( "Slide %1" ).arg( pagenr + 1 ) );
            if (title.length() > 12) // restrict to a maximum of 12 characters
                it.current()->setText( 0, title.left(5) + "..." + title.right(4));
            else
                it.current()->setText( 0, title );

            it.current()->setText( 1, QString::null ); // hack, to make itemStateChange do nothing
            static_cast<OutlineItem*>(it.current())->setOn( doc->isSlideSelected( pagenr ) );
            it.current()->setText( 1, QString::number( pagenr + 1 ) ); // page number
            return;
        }
    }
    kdWarning() << "Item for page " << pagenr << " not found" << endl;
}

void Outline::addItem( int /*pos*/ )
{
    kdDebug(33001)<< "Outline::addItem" << endl;
    // still use rebuildItems as I had no good idea to do it :-) tz
    rebuildItems();
}

// move an Outline Item so that not the hole list has to be recerated
void Outline::moveItem( int oldPos, int newPos )
{
    int page = 0;

    int lowPage = oldPos > newPos ? newPos : oldPos;
    int highPage = oldPos < newPos ? newPos : oldPos;
    QListViewItemIterator it( this );
    // Recreate all Titles and pagenumbers between lowPage & highPage
    for ( ; it.current(); ++it ) {
        if ( page >= lowPage && page <= highPage) {
            QString title = doc->pageList().at(page)->pageTitle( i18n( "Slide %1" ).arg( page + 1 ) );
            if (title.length() > 12) // restrict to a maximum of 12 characters
                it.current()->setText( 0, title.left(5) + "..." + title.right(4));
            else
                it.current()->setText( 0, title );

            it.current()->setText( 1, QString::null ); // hack, to make itemStateChange do nothing
            static_cast<OutlineItem*>(it.current())->setOn( doc->isSlideSelected( page ) );
            it.current()->setText( 1, QString::number( page + 1 ) ); // page number
            if ( page == highPage )
                return;
        }
        page++;
    }
}

void Outline::removeItem( int pos )
{
    kdDebug(33001)<< "Outline::removeItem" << endl;
    int page = 0;
    bool updatePageNum = false;

    QListViewItemIterator it( this );
    for ( ; it.current(); ++it ) {
        if ( page == pos ) {
            kdDebug(33001) << "Page " << it.current()->text(0) << " removed" << endl;
            if ( it.current()->nextSibling())
                updatePageNum = true;
            delete it.current();
        }
        if ( updatePageNum ) {
            QString title = doc->pageList().at(page)->pageTitle( i18n( "Slide %1" ).arg( page + 1 ) );
            if (title.length() > 12) // restrict to a maximum of 12 characters
                it.current()->setText( 0, title.left(5) + "..." + title.right(4));
            else
                it.current()->setText( 0, title );

            it.current()->setText( 1, QString::number( page + 1 ) ); // page number
        }
        page++;
    }
}

void Outline::itemStateChange( OutlineItem * item, bool state )
{
    QString text = item->text( 1 );
    if ( !text.isEmpty() ) // empty if we are called from rebuildItems
        emit selectPage( text.toInt() - 1, state );
}

QRect Outline::tip(const QPoint &pos, QString &title)
{
    QListViewItem *item = itemAt( pos );
    if (!item)
        return QRect(0, 0, -1, -1);

    int pagenr = item->text(1).toInt() - 1;
    title = doc->pageList().at(pagenr)->pageTitle(i18n("Slide %1").arg(pagenr + 1));

    return itemRect(item);
}

void Outline::itemClicked( QListViewItem *i )
{
    if ( !i )
        return;
    emit showPage( i->text( 1 ).toInt() - 1 );
}


void Outline::setCurrentPage( int pg )
{
    QListViewItemIterator it( this );
    for ( ; it.current(); ++it ) {
        if ( it.current()->text( 1 ).toInt() - 1 == pg ) {
            setCurrentItem( it.current() );
            setSelected( it.current(), TRUE );
            ensureItemVisible(it.current());
        }
    }
}

void Outline::setOn( int pg, bool on )
{
    QListViewItemIterator it( this );
    for ( ; it.current(); ++it ) {
        if ( it.current()->text( 1 ).toInt() - 1 == pg ) {
            ( (QCheckListItem*)it.current() )->setOn( on );
            return;
        }
    }
}

void Outline::contentsDropEvent( QDropEvent *e )
{
    disconnect( this, SIGNAL( currentChanged( QListViewItem * ) ), this, SLOT( itemClicked( QListViewItem * ) ) );
    KListView::contentsDropEvent( e );
    connect( this, SIGNAL( currentChanged( QListViewItem * ) ), this, SLOT( itemClicked( QListViewItem * ) ) );
}

void Outline::movedItems( QListViewItem *i, QListViewItem *, QListViewItem *newAfter )
{
    movedItem = i;
    movedAfter = newAfter;
    QTimer::singleShot( 300, this, SLOT( doMoveItems() ) );

}

void Outline::doMoveItems()
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
    if(num!=numNow) {
        emit movePage( num, numNow );
        // this has to be done because moving a page is take + insert the page
        setSelected( movedItem, true );
    }
}

void Outline::rightButtonPressed( QListViewItem *, const QPoint &pnt, int )
{
    if ( !selectedItem() || !doc->isReadWrite())
        return;
    view->openPopupMenuSideBar(pnt);
}

void Outline::renamePageTitle()
{
    QListViewItem *item = QListView::selectedItem();
    if ( !item )
        return;

    int pageNumber = item->text( 1 ).toInt() - 1;
    bool ok;
    QString activeTitle = doc->pageList().at( pageNumber )->pageTitle( i18n( "Slide %1" ).arg( pageNumber + 1 ) );
    QString newTitle = KLineEditDlg::getText( i18n("Rename Page"),i18n("Page title:"), activeTitle, &ok, this );

    // Have a different name ?
    if ( ok ) { // User pushed an OK button.
        if ( (newTitle.stripWhiteSpace()).isEmpty() ) { // Title is empty.
            KNotifyClient::beep();
            KMessageBox::information( this, i18n("Page title cannot be empty."), i18n("Rename Page") );
            // Recursion
            renamePageTitle();
        }
        else if ( newTitle != activeTitle ) { // Title changed.
            KPresenterDoc *doc=view->kPresenterDoc();
            KPrChangeTitlePageNameCommand *cmd=new KPrChangeTitlePageNameCommand( i18n("Rename Page"),doc, activeTitle, newTitle,doc->pageList().at(pageNumber)  );
            cmd->execute();
            doc->addCommand(cmd);
        }
    }
}


#include <sidebar.moc>
