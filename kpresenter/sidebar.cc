// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2001 Lukas Tinkl <lukas@kde.org>
   Copyright (C) 2002 Ariya Hidayat <ariya@kde.org>

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

#include <qheader.h>
#include <qtimer.h>
#include <qpopupmenu.h>
#include <qimage.h>
#include <qtabwidget.h>
#include <qtooltip.h>

#include <kwordwrap.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kinputdialog.h>
#include <knotifyclient.h>
#include <kiconview.h>
#include <kdebug.h>

#include "sidebar.h"
#include "kpresenter_view.h"
#include "kpresenter_doc.h"
#include "kprcanvas.h"
#include "kprpage.h"
#include "kpobject.h"
#include <qapplication.h>
#include "kprcommand.h"
#include <qvalidator.h>

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
        Outline* outline = dynamic_cast<Outline*>(parentWidget());
        QRect r( outline->tip(pos, title) );
        if (!r.isValid())
            return;
        tip(r, title);
    }
};

class OutlineSlideItem: public KListViewItem
{
public:
    OutlineSlideItem( KListView * parent, KPrPage* page );
    OutlineSlideItem( KListView * parent, OutlineSlideItem *after, KPrPage* page );

    KPrPage* page(){ return m_page; }

    void setPage( KPrPage* p );

    void update();

private:
    KPrPage* m_page;
};

class OutlineObjectItem: public KListViewItem
{
public:
    OutlineObjectItem( OutlineSlideItem * parent, KPObject* object,
                       bool sticky, const QString& name = QString::null );

    KPObject* object(){ return m_object; }

    void setObject( KPObject* o );

private:
    KPObject* m_object;
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
    addTab(_thb,i18n("Preview"));


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
    _outline->updateItem( pos, sticky );
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
            blockSignals( true );
            setCurrentItem( it );
            setSelected( it, FALSE ); // to avoid the blue "selected"-mark
            ensureItemVisible(it);
            refreshItems();
            blockSignals( false );
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
    title = doc->pageList().at(pagenr)->pageTitle();

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
        // calculate the size of the thumb
        QRect rect = view->kPresenterDoc()->pageList().at(i)->getZoomPageRect( );

        int w = rect.width();
        int h = rect.height();
        if ( w > h ) {
            w = 130;
            float diff = (float)rect.width() / (float)w;
            h = (int) (rect.height() / diff);
            if ( h > 120 ) {
                h = 120;
                float diff = (float)rect.height() / (float)h;
                w = (int) (rect.width() / diff);
            }
        }
        else if ( w < h ) {
            h = 130;
            float diff = (float)rect.height() / (float)h;
            w = (int) (rect.width() / diff);
            if ( w > 120 ) {
                w = 120;
                float diff = (float)rect.width() / (float)w;
                h = (int) (rect.height() / diff);
            }
        }
        else if ( w == h ) {
            w = 130;
            h = 130;
        }

        // draw an empty thumb
        QPixmap pix(w, h);
        pix.fill( Qt::white );

        QPainter p(&pix);
        p.setPen(Qt::black);
        p.drawRect(pix.rect());

        ThumbItem *item = new ThumbItem(static_cast<QIconView *>(this), QString::number(i+1), pix);
        item->setUptodate( false );
        item->setDragEnabled(false);  //no dragging for now
    }

    QTimer::singleShot( 10, this, SLOT( slotRefreshItems() ) );

    uptodate = true;

    QApplication::restoreOverrideCursor();
}

void ThumbBar::refreshItems(bool offset)
{
    QRect vRect = visibleRect();
    if ( offset )
        vRect.moveBy( offsetX, offsetY );
    else
        vRect.moveBy( contentsX(), contentsY() );

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
        if ( it == findFirstVisibleItem( vRect ) ) {
            do
            {
                if ( sticky || it->text().toInt() == pagenr + 1 ) {
                    it->setPixmap(getSlideThumb( pagecnt ));
                    static_cast<ThumbItem *>(it)->setUptodate( true );

                    if ( !sticky )
                        return;
                }
                if ( it == findLastVisibleItem( vRect ) )
                    break;
                pagecnt++;
                it = it->nextItem();
            } while ( true );
        }
        else if ( sticky || it->text().toInt() == pagenr + 1 ) {
            static_cast<ThumbItem *>(it)->setUptodate( false );
            if ( !sticky )
                return;
        }
        pagecnt++;
        it = it->nextItem();
    } while ( it );

    if ( ! sticky )
        kdWarning(33001) << "Item for page " << pagenr << " not found" << endl;
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
        if ( page >= pos )
            it->setText( QString::number(page+2) );
        page++;
    }
}

// moves a item without recreating all pages
void ThumbBar::moveItem( int oldPos, int newPos )
{
    kdDebug(33001)<< "ThumbBar::moveItem " << oldPos << " to " << newPos << endl;
    int page = 0;
    QIconViewItem *after = 0;
    QIconViewItem *take = 0;
    for ( QIconViewItem *it = firstItem(); it; it = it->nextItem() ) {
        // find page which should move
        if ( page == oldPos )
            take = it;
        // find position where page should be insert
        // as a page can not be insert at the beginning get the first one
        // the page to get depends on if a page is moved forward / backwards
        if ( page == newPos )
            after = page == 0 ? it : newPos > oldPos ? it : it->prevItem();
        page++;
    }

    if ( ! take )
        return;

    // workaround for a bug in qt 3.1.1 insertItem dose not work.
    // TODO remove workaround when qt 3.1.2 comes out tz
    //takeItem( take );
    //insertItem( take, after);
    ThumbItem *item = new ThumbItem( static_cast<QIconView *>(this), after, QString::number( newPos ), *(take->pixmap()) );
    item->setDragEnabled(false);  //no dragging for now
    delete take;
    // update the thumbs if new pos was 0
    // because it was insert after the first one
    if ( newPos == 0 ) {
        //todo do not recreate the pics
        after->setPixmap(getSlideThumb( 0 ));
        //take->setPixmap(getSlideThumb( 1 ));
        item->setPixmap(getSlideThumb( 1 ));
    }

    //write the new page numbers
    int lowPage = oldPos > newPos ? newPos : oldPos;
    int highPage = oldPos < newPos ? newPos : oldPos;
    page = 0;
    for ( QIconViewItem *it = firstItem(); it; it = it->nextItem() ) {
        if ( page >= lowPage && page <= highPage)
            it->setText( QString::number(page+1) );
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
        if ( change )
            it->setText( QString::number( page + 1 ) );
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
    kdDebug(33001) << "offset x,y = " << x << ", " << y << endl;
    refreshItems( true );
}

void ThumbBar::slotRefreshItems()
{
    refreshItems();
}

OutlineSlideItem::OutlineSlideItem( KListView* parent, KPrPage* _page )
    : KListViewItem( parent ), m_page( _page )
{
    setDragEnabled(true);
    setPage( _page );
    setPixmap( 0, KPBarIcon( "newslide" ) );
}

OutlineSlideItem::OutlineSlideItem( KListView* parent, OutlineSlideItem * after, KPrPage* _page )
    : KListViewItem( parent, after ), m_page( _page )
{
    setDragEnabled(true);
    setPage( _page );
    setPixmap( 0, KPBarIcon( "newslide" ) );
}

void OutlineSlideItem::setPage( KPrPage* p )
{
    if( !p ) return;
    m_page = p;
    update();
}

void OutlineSlideItem::update()
{
    if( !m_page ) return;
    int index = m_page->kPresenterDoc()->pageList().findRef( m_page );
    QString title = m_page->pageTitle();
    if ( !m_page->kPresenterDoc()->isSlideSelected( index ) )
        title = i18n( "(%1)" ).arg( title );
    setText( 0, title );

    // add all objects
    OutlineObjectItem *ooi = 0;
    while ( ( ooi = dynamic_cast<OutlineObjectItem*>( this->firstChild() ) ) )
        delete ooi;

    // keep selected object
    ooi = 0;

    QPtrListIterator<KPObject> it( m_page->objectList() );

    for ( ; it.current(); ++it ) {
        OutlineObjectItem *item = new OutlineObjectItem( this, it.current(),
                                                         it.current()->isSticky() );
        item->setDragEnabled( false );
        if ( it.current()->isSelected() )
            ooi = item;
    }

    KPObject* header = 0;
    KPObject* footer = 0;

    KPresenterDoc *doc = m_page->kPresenterDoc();
    // add sticky objects, exclude header and footer
    it = doc->masterPage()->objectList();
    for ( ; it.current() ; ++it )
    {
        KPObject* object = it.current();

        if( doc->hasHeader() && doc->isHeader( object ) )
            header = object;
        else if( doc->hasFooter() && doc->isFooter( object ) )
            footer = object;
        else if( !doc->isHeader( object ) && !doc->isFooter( object ) ) {
            OutlineObjectItem *item = new OutlineObjectItem( this, object, true );
            if ( object->isSelected() )
                ooi = item;
        }

    }

    // add header and footer (if any)
    if ( footer ) {
        OutlineObjectItem *item = new OutlineObjectItem( this, footer, true, i18n("Footer") );
        if ( footer->isSelected() )
            ooi = item;
    }

    if ( header ) {
        OutlineObjectItem *item = new OutlineObjectItem( this, header, true, i18n("Header") );
        if ( header->isSelected() )
            ooi = item;
    }

    // select selected object the page is necessary that a
    // sticky object is selected on the active page
    if ( ooi && doc->activePage() == m_page )
        (ooi->listView())->setSelected( ooi, true );
}


OutlineObjectItem::OutlineObjectItem( OutlineSlideItem* parent, KPObject* _object,
                                      bool sticky, const QString& name )
    : KListViewItem( parent ), m_object( _object )
{
    setObject( m_object );

    QString objectName = name.isEmpty() ? m_object->getObjectName() : name;
    if( sticky ) objectName += i18n(" (Sticky)" );
    setText( 0, objectName );
}

void OutlineObjectItem::setObject( KPObject* object )
{
    if( !object ) return;
    m_object = object;

    switch ( m_object->getType() ) {
    case OT_PICTURE:
        setPixmap( 0, KPBarIcon( "frame_image" ) );
        break;
    case OT_LINE:
        setPixmap( 0, KPBarIcon( "mini_line" ) );
        break;
    case OT_RECT:
        setPixmap( 0, KPBarIcon( "mini_rect" ) );
        break;
    case OT_ELLIPSE:
        setPixmap( 0, KPBarIcon( "mini_circle" ) );
        break;
    case OT_TEXT:
        setPixmap( 0, KPBarIcon( "frame_text" ) );
        break;
    case OT_AUTOFORM:
        setPixmap( 0, KPBarIcon( "mini_autoform" ) );
        break;
    case OT_CLIPART:
        setPixmap( 0, KPBarIcon( "mini_clipart" ) );
        break;
    case OT_PIE:
        setPixmap( 0, KPBarIcon( "mini_pie" ) );
        break;
    case OT_PART:
        setPixmap( 0, KPBarIcon( "frame_query" ) );
        break;
    case OT_FREEHAND:
        setPixmap( 0, KPBarIcon( "freehand" ) );
        break;
    case OT_POLYLINE:
        setPixmap( 0, KPBarIcon( "polyline" ) );
        break;
    case OT_QUADRICBEZIERCURVE:
        setPixmap( 0, KPBarIcon( "quadricbeziercurve" ) );
        break;
    case OT_CUBICBEZIERCURVE:
        setPixmap( 0, KPBarIcon( "cubicbeziercurve" ) );
        break;
    case OT_POLYGON:
        setPixmap( 0, KPBarIcon( "mini_polygon" ) );
        break;
    case OT_CLOSED_LINE: {
        QString name = m_object->getTypeString();
        if ( name == i18n( "Closed Freehand" ) )
            setPixmap( 0, KPBarIcon( "closed_freehand" ) );
        else if ( name == i18n( "Closed Polyline" ) )
            setPixmap( 0, KPBarIcon( "closed_polyline" ) );
        else if ( name == i18n( "Closed Quadric Bezier Curve" ) )
            setPixmap( 0, KPBarIcon( "closed_quadricbeziercurve" ) );
        else if ( name == i18n( "Closed Cubic Bezier Curve" ) )
            setPixmap( 0, KPBarIcon( "closed_cubicbeziercurve" ) );
    } break;
    case OT_GROUP:
        setPixmap( 0, KPBarIcon( "group" ) );
        break;
    default:
        break;
    }
}

Outline::Outline( QWidget *parent, KPresenterDoc *d, KPresenterView *v )
    : KListView( parent ), doc( d ), view( v )
{
    rebuildItems();
    setSorting( -1 );
    header()->hide();
    addColumn( i18n( "Slide" ) );
    setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Expanding ) );

    outlineTip = new OutlineToolTip(this);

    connect( this, SIGNAL( currentChanged( QListViewItem * ) ), this, SLOT( itemClicked( QListViewItem * ) ) );
    connect( this, SIGNAL( moved( QListViewItem *, QListViewItem *, QListViewItem * ) ),
             this, SLOT( movedItems( QListViewItem *, QListViewItem *, QListViewItem * ) ) );
    connect( this, SIGNAL( rightButtonPressed( QListViewItem *, const QPoint &, int ) ),
             this, SLOT( rightButtonPressed( QListViewItem *, const QPoint &, int ) ) );

    connect( this, SIGNAL( doubleClicked ( QListViewItem * )),
             this, SLOT(renamePageTitle()));

    setAcceptDrops( true );
    setDropVisualizer( true );
    setFullWidth( true );
    this->setRootIsDecorated( true );
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
        KPrPage *page=doc->pageList().at( i );
        new OutlineSlideItem( this, page );
    }
}

// given the page number (0-based), find associated slide item
// returns 0 upon stupid things (e.g. invalid page number)
OutlineSlideItem* Outline::slideItem( int pageNumber )
{
    QListViewItem* item = firstChild();
    for( int index = 0; item; ++index, item = item->nextSibling() ) {
        if( index == pageNumber )
            return dynamic_cast<OutlineSlideItem*>( item );
    }

    return 0;
}


// update the Outline item, the title may have changed
void Outline::updateItem( int pagenr /* 0-based */, bool sticky )
{
    if ( ! sticky ) {
        OutlineSlideItem *item = slideItem( pagenr );
        if( item ) {
            blockSignals(true);
            item->update();
            blockSignals(false);
        }
    } else {
        blockSignals(true);
        for( QListViewItem *item = this->firstChild(); item; item = item->nextSibling() )
            dynamic_cast<OutlineSlideItem*>(item)->update();
        blockSignals(false);
    }
}

void Outline::addItem( int pos )
{
    kdDebug(33001)<< "Outline::addItem" << endl;

    KPrPage *page=doc->pageList().at( pos );
    OutlineSlideItem *item;
    if ( pos == 0 ) {
        item = new OutlineSlideItem( this, page );
    }
    else {
        OutlineSlideItem *after = slideItem( pos - 1 );
        item = new OutlineSlideItem( this, after, page );
    }

    // update title
    for( ; item; item = dynamic_cast<OutlineSlideItem*>(item->nextSibling()) )
        item->update();
}

// move an Outline Item so that not the hole list has to be recreated
void Outline::moveItem( int oldPos, int newPos )
{
    kdDebug(33001)<< "Outline::moveItem " << oldPos << " to " << newPos << endl;

    int lowPage = oldPos > newPos ? newPos : oldPos;
    int highPage = oldPos < newPos ? newPos : oldPos;

    // update, for all between lowPage & highPage
    int page = lowPage;
    OutlineSlideItem* item = slideItem( page );
    for( ; item ; ++page ) {
        KPrPage* newPage = doc->pageList().at( page );
        item->setPage( newPage );
        item = dynamic_cast<OutlineSlideItem*>( item->nextSibling() );
        if ( page == highPage ) break;
    }
}

void Outline::removeItem( int pos )
{
    kdDebug(33001)<< "Outline::removeItem" << endl;

    OutlineSlideItem* item = slideItem( pos );
    if( !item ) return;
    OutlineSlideItem* temp = dynamic_cast<OutlineSlideItem*>(item->nextSibling());

    delete item;

    for( item = temp; item; ++pos ) {
        KPrPage* newPage = doc->pageList().at( pos );
        item->setPage( newPage );
        item = dynamic_cast<OutlineSlideItem*>( item->nextSibling() );
    }
}

QRect Outline::tip(const QPoint &pos, QString &title)
{
    QListViewItem *item = itemAt( pos );
    if (!item)
        return QRect(0, 0, -1, -1);

    OutlineSlideItem* slideItem = dynamic_cast<OutlineSlideItem*>(item);
    if( !slideItem )
        return QRect(0, 0, -1, -1);
    title = slideItem->text( 0 );

    return itemRect(item);
}

void Outline::itemClicked( QListViewItem *item )
{
    if( !item ) return;

    // check if we need to show chosen slide
    OutlineSlideItem* slideItem = dynamic_cast<OutlineSlideItem*>(item);
    if( slideItem )
    {
        KPrPage* page = slideItem->page();
        if( !page ) return;
        emit showPage( page->kPresenterDoc()->pageList().findRef( page ) );
    }

    // check if we need to show chosen object
    OutlineObjectItem* objectItem = dynamic_cast<OutlineObjectItem*>(item);
    if( objectItem )
    {
        KPObject *object = objectItem->object();
        if( !object ) return;

        // ensure the owner slide is shown first
        OutlineSlideItem* slideItem = dynamic_cast<OutlineSlideItem*>(objectItem->parent());
        if( slideItem && doc->activePage() != slideItem->page() )
        {
            KPrPage* page = slideItem->page();
            if( !page ) return;
            emit showPage( page->kPresenterDoc()->pageList().findRef( page ) );
        }

        // select the object, make sure it's visible
        doc->deSelectAllObj();
        view->getCanvas()->selectObj( object );
        QRect rect( doc->zoomHandler()->zoomRect( object->getBoundingRect() ) );
        rect.setLeft( rect.left() - 20 );
        rect.setTop( rect.top() - 20 );
        rect.setRight( rect.right() + 20 );
        rect.setBottom( rect.bottom() + 20 );
        view->makeRectVisible( rect );
        doc->repaint( false );
    }
}

void Outline::setCurrentPage( int pg )
{
    OutlineSlideItem *item = slideItem( pg );
    if( item && ( item!=currentItem()->parent() ) )
    {
        blockSignals( true );
        setCurrentItem( item );
        setSelected( item, true );
        ensureItemVisible( item );
        blockSignals( false );
    }
}

void Outline::contentsDropEvent( QDropEvent *e )
{
    disconnect( this, SIGNAL( currentChanged( QListViewItem * ) ), this, SLOT( itemClicked( QListViewItem * ) ) );
    KListView::contentsDropEvent( e );
    connect( this, SIGNAL( currentChanged( QListViewItem * ) ), this, SLOT( itemClicked( QListViewItem * ) ) );
}

// when item is about to move (using drag-and-drop), make sure:
// it's not moved right after object. only slides can move, objects can't.
void Outline::movableDropEvent( QListViewItem* parent, QListViewItem* target )
{
    // slide doesn't have parent (always 0)
    if( parent ) return;

    KListView::movableDropEvent( parent, target );
}

void Outline::movedItems( QListViewItem *i, QListViewItem *, QListViewItem *newAfter )
{
    movedItem = i;
    movedAfter = newAfter;
    QTimer::singleShot( 300, this, SLOT( doMoveItems() ) );
}

void Outline::doMoveItems()
{
    OutlineSlideItem* srcItem = dynamic_cast<OutlineSlideItem*>(movedItem);
    if( !srcItem ) return;

    int num = doc->pageList().findRef( srcItem->page() );

    OutlineSlideItem* dstItem = dynamic_cast<OutlineSlideItem*>(movedAfter);
    if( movedAfter && !dstItem ) return;

    int numNow = movedAfter ? doc->pageList().findRef( dstItem->page() ) : -1;
    if ( numNow < num ) numNow++;

    if(num!=numNow) {
        emit movePage( num, numNow );
        // this has to be done because moving a page is take + insert the page
        setSelected( movedItem, true );
    }
}

void Outline::rightButtonPressed( QListViewItem *, const QPoint &pnt, int )
{
    if ( !doc->isReadWrite()) return;

    QListViewItem *item = QListView::selectedItem();
    if( !item ) return;

    OutlineSlideItem* slideItem = dynamic_cast<OutlineSlideItem*>(item);
    if( !slideItem ) return;

    view->openPopupMenuSideBar(pnt);
}

void Outline::renamePageTitle()
{
    QListViewItem *item = QListView::selectedItem();
    if( !item ) return;

    OutlineSlideItem* slideItem = dynamic_cast<OutlineSlideItem*>(item);
    if( !slideItem ) return;

    KPrPage* page = slideItem->page();
    if( !page ) return;

    bool ok = false;
    QString activeTitle = item->text( 0 );
    QRegExpValidator validator( QRegExp( ".*" ), 0 ); // we want to allow empty titles. Empty == automatic.
    QString newTitle = KInputDialog::getText( i18n("Rename Slide"),
                                              i18n("Slide title:"), activeTitle, &ok, this, 0,
                                              &validator );

    // Have a different name ?
    if ( ok ) { // User pushed an OK button.
        if ( newTitle != activeTitle ) { // Title changed.
            KPresenterDoc *doc=view->kPresenterDoc();
            KPrChangeTitlePageNameCommand *cmd=new KPrChangeTitlePageNameCommand( i18n("Rename Slide"),
                                                                                  doc, activeTitle, newTitle, page  );
            cmd->execute();
            doc->addCommand(cmd);
        }
    }
}


#include "sidebar.moc"
