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
    ThumbToolTip( ThumbBar *parent )
        : QToolTip( parent->viewport() )
        , m_thumbBar( parent )
        {}

protected:
    void maybeTip(const QPoint &pos)
    {
        QString title;
        QRect r( m_thumbBar->tip( pos, title ) );
        if (!r.isValid())
            return;

        tip(r, title);
    }
private:
    ThumbBar *m_thumbBar;
};


class OutlineSlideItem: public KListViewItem
{
public:
    OutlineSlideItem( KListView * parent, KPrPage* page, bool _masterPage );
    OutlineSlideItem( KListView * parent, OutlineSlideItem *after, KPrPage* page, bool _masterPage );

    KPrPage* page() const { return m_page; }

    void setPage( KPrPage* p );

    void update();
    void updateTitle();

private:
    KPrPage* m_page;
    bool m_masterPage;
};

class OutlineObjectItem: public KListViewItem
{
public:
    OutlineObjectItem( OutlineSlideItem * parent, KPObject* object,
                       bool sticky, const QString& name = QString::null );

    KPObject* object() const { return m_object; }

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
    :QTabWidget(parent), m_doc(d), m_view(v)
{
    setTabPosition(QTabWidget::Top);
    setTabShape(QTabWidget::Triangular);

    m_outline = new Outline(this, m_doc, m_view);
    addTab(m_outline, i18n("Outline"));

    m_thb = new ThumbBar(this, m_doc, m_view);
    addTab(m_thb,i18n("Preview"));


    //TODO find a better way
    connect(m_outline, SIGNAL(showPage(int)),
            this, SIGNAL(showPage(int)));

    connect(m_thb, SIGNAL(showPage(int)),
            this, SIGNAL(showPage(int)));

    connect(m_outline, SIGNAL(movePage(int,int)),
            this, SIGNAL(movePage(int,int)));

    connect(m_outline, SIGNAL(selectPage(int,bool)),
            this, SIGNAL(selectPage(int,bool)));

    connect(this, SIGNAL(currentChanged(QWidget *)),
            this, SLOT(currentChanged(QWidget *)));

}

void SideBar::currentChanged(QWidget *tab)
{
    if (tab == m_thb) {
        if (!m_thb->uptodate && m_thb->isVisible())
            m_thb->rebuildItems();
        else
            m_thb->refreshItems();
    }
}

void SideBar::addItem( int pos )
{
    m_outline->addItem( pos );
    m_thb->addItem( pos );
}

void SideBar::moveItem( int oldPos, int newPos )
{
    m_outline->moveItem( oldPos, newPos );
    m_thb->moveItem( oldPos, newPos );
}

void SideBar::removeItem( int pos )
{
    m_outline->removeItem( pos );
    m_thb->removeItem( pos );
}

void SideBar::updateItem( KPrPage *page )
{
    bool sticky = false;
    int pos = 0;
    if ( page == m_doc->masterPage() )
    {
        pos = -1;
        sticky = true;
    }
    else
    {
        pos = m_doc->pageList().findRef( page );
    }

    m_outline->updateItem( pos, sticky );
    m_thb->updateItem( pos, sticky );
}

void SideBar::setViewMasterPage( bool _masterPage )
{
    m_outline->setViewMasterPage( _masterPage );
    m_thb->setViewMasterPage( _masterPage );
    m_outline->rebuildItems();
    m_thb->rebuildItems();
}

SideBarBase::SideBarBase(KPresenterDoc *_doc, KPresenterView *_view)
    : m_doc( _doc ), m_view( _view ), m_viewMasterPage( false )
{
}

void SideBarBase::setViewMasterPage( bool _b )
{
    m_viewMasterPage = _b;
}

ThumbBar::ThumbBar(QWidget *parent, KPresenterDoc *d, KPresenterView *v)
    :KIconView(parent), SideBarBase( d,v)
{
    uptodate = false;
    m_offsetX = 0;
    m_offsetY = 0;

    setArrangement(QIconView::LeftToRight);
    setAutoArrange(true);
    setSorting(false);
    setItemsMovable(false);
    setResizeMode(QIconView::Adjust);

    m_thumbTip = new ThumbToolTip(this);

    connect(this, SIGNAL(currentChanged(QIconViewItem *)),
            this, SLOT(itemClicked(QIconViewItem *)));
    connect(this, SIGNAL(contentsMoving(int, int)),
            this, SLOT(slotContentsMoving(int, int)));
}

ThumbBar::~ThumbBar()
{
    delete m_thumbTip;
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
    title = m_doc->pageList().at(pagenr)->pageTitle();

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
    if ( m_viewMasterPage )
    {
    }
    else
    {
        for ( unsigned int i = 0; i < m_doc->getPageNums(); i++ ) {
            // calculate the size of the thumb
            QRect rect = m_doc->pageList().at(i)->getZoomPageRect( );

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
    }
    uptodate = true;

    QApplication::restoreOverrideCursor();
}

void ThumbBar::refreshItems(bool offset)
{
    QRect vRect = visibleRect();
    if ( offset )
        vRect.moveBy( m_offsetX, m_offsetY );
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

    m_offsetX = 0;
    m_offsetY = 0;
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

    m_view->getCanvas()->drawPageInPix( pix, slideNr, 60 );

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
    m_offsetX = x;
    m_offsetY = y;
    kdDebug(33001) << "offset x,y = " << x << ", " << y << endl;
    refreshItems( true );
}

void ThumbBar::slotRefreshItems()
{
    refreshItems();
}

OutlineSlideItem::OutlineSlideItem( KListView* parent, KPrPage* _page, bool _masterPage )
    : KListViewItem( parent ), m_page( _page ), m_masterPage( _masterPage )
{
    setDragEnabled(true);
    setPage( _page );
    setPixmap( 0, KPBarIcon( "newslide" ) );
}

OutlineSlideItem::OutlineSlideItem( KListView* parent, OutlineSlideItem * after, KPrPage* _page, bool _masterPage )
    : KListViewItem( parent, after ), m_page( _page ), m_masterPage( _masterPage )
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
    KPresenterDoc *doc = m_page->kPresenterDoc();
    updateTitle();

    // add all objects
    OutlineObjectItem *ooi = 0;
    while ( ( ooi = dynamic_cast<OutlineObjectItem*>( this->firstChild() ) ) )
        delete ooi;

    // keep selected object
    ooi = 0;

    QPtrListIterator<KPObject> it( m_page->objectList() );

    if ( !m_masterPage )
    {
        for ( ; it.current(); ++it ) {
            OutlineObjectItem *item = new OutlineObjectItem( this, it.current(),
                                                             it.current()->isSticky() );
            item->setDragEnabled( false );
            if ( it.current()->isSelected() )
                ooi = item;
        }
    }
    KPObject* header = 0;
    KPObject* footer = 0;

    // add sticky objects, exclude header and footer
    it = doc->masterPage()->objectList();
    for ( ; it.current() ; ++it )
    {
        KPObject* object = it.current();

        if( m_page->hasHeader() && doc->isHeader( object ) )
            header = object;
        else if( m_page->hasFooter() && doc->isFooter( object ) )
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

void OutlineSlideItem::updateTitle()
{
    QString title = m_page->pageTitle();
    if ( ! m_page->isSlideSelected() )
        title = i18n( "(%1)" ).arg( title );
    setText( 0, title );
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
    : KListView( parent ), SideBarBase( d, v)
{
    rebuildItems();
    setSorting( -1 );
    header()->hide();
    addColumn( i18n( "Slide" ) );
    setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Expanding ) );

    connect( this, SIGNAL( currentChanged( QListViewItem * ) ), this, SLOT( itemClicked( QListViewItem * ) ) );
    connect( this, SIGNAL( rightButtonPressed( QListViewItem *, const QPoint &, int ) ),
             this, SLOT( rightButtonPressed( QListViewItem *, const QPoint &, int ) ) );

    connect( this, SIGNAL( doubleClicked ( QListViewItem * )),
             this, SLOT(renamePageTitle()));
    connect( this, SIGNAL( dropped( QDropEvent*, QListViewItem*, QListViewItem* ) ),
             this, SLOT( slotDropped( QDropEvent*, QListViewItem*, QListViewItem*  ) ));

    setItemsMovable( false );
    setDragEnabled( true );
    setAcceptDrops( true );
    setDropVisualizer( true );
    setFullWidth( true );
    this->setRootIsDecorated( true );
}

Outline::~Outline()
{
}

void Outline::rebuildItems()
{
    clear();
    if ( m_viewMasterPage )
    {
        KPrPage *page=m_doc->masterPage();
        new OutlineSlideItem( this, page, true );
    }
    else
    {
        // Rebuild all the items
        for ( int i = m_doc->getPageNums() - 1; i >= 0; --i ) {
            KPrPage *page=m_doc->pageList().at( i );
            new OutlineSlideItem( this, page, false );
        }
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

    KPrPage *page=m_doc->pageList().at( pos );
    OutlineSlideItem *item;
    if ( pos == 0 ) {
        item = new OutlineSlideItem( this, page,m_viewMasterPage );
    }
    else {
        OutlineSlideItem *after = slideItem( pos - 1 );
        item = new OutlineSlideItem( this, after, page,m_viewMasterPage );
    }

    item = dynamic_cast<OutlineSlideItem*>( item->nextSibling() );
    // update title
    for( ; item; item = dynamic_cast<OutlineSlideItem*>( item->nextSibling() ) )
        item->updateTitle();
}

// move an Outline Item so that not the hole list has to be recreated
void Outline::moveItem( int oldPos, int newPos )
{
    kdDebug(33001)<< "Outline::moveItem " << oldPos << " to " << newPos << endl;

    int lowPage = oldPos > newPos ? newPos : oldPos;
    int highPage = oldPos < newPos ? newPos : oldPos;

    OutlineSlideItem *item = dynamic_cast<OutlineSlideItem*>( firstChild() );
    QListViewItem *itemToMove = 0;
    QListViewItem *itemAfter = 0;

    // moving backwards
    if ( newPos < oldPos )
        newPos--;

    for ( int index = 0; item; ++index, item = dynamic_cast<OutlineSlideItem*>( item->nextSibling() ) )
    {
        if ( index == oldPos )
            itemToMove = item;
        if ( index == newPos )
            itemAfter = item;
        if ( index >= lowPage && index <= highPage )
            item->updateTitle();
    }

    KListView::moveItem( itemToMove, 0, itemAfter );
}

void Outline::removeItem( int pos )
{
    kdDebug(33001)<< "Outline::removeItem" << endl;

    OutlineSlideItem* item = slideItem( pos );
    if( !item ) return;
    OutlineSlideItem* temp = dynamic_cast<OutlineSlideItem*>(item->nextSibling());

    delete item;

    for ( item = temp; item; item = dynamic_cast<OutlineSlideItem*>( item->nextSibling() ) )
        item->updateTitle();
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
        emit showPage( m_doc->pageList().findRef( page ) );
    }

    // check if we need to show chosen object
    OutlineObjectItem* objectItem = dynamic_cast<OutlineObjectItem*>(item);
    if( objectItem )
    {
        KPObject *object = objectItem->object();
        if( !object ) return;

        // ensure the owner slide is shown first
        OutlineSlideItem* slideItem = dynamic_cast<OutlineSlideItem*>(objectItem->parent());
        if( slideItem && m_doc->activePage() != slideItem->page() )
        {
            KPrPage* page = slideItem->page();
            if( !page ) return;
            emit showPage( m_doc->pageList().findRef( page ) );
        }

        // select the object, make sure it's visible
        m_doc->deSelectAllObj();
        m_view->getCanvas()->selectObj( object );
        QRect rect( m_doc->zoomHandler()->zoomRect( object->getBoundingRect() ) );
        rect.setLeft( rect.left() - 20 );
        rect.setTop( rect.top() - 20 );
        rect.setRight( rect.right() + 20 );
        rect.setBottom( rect.bottom() + 20 );
        m_view->makeRectVisible( rect );
        m_doc->repaint( false );
    }
}

/**
 * The listview no longer moves the item by itself. It just calls m_doc->movePage
 * which then moves the item. At the moment the method only works as long as
 * only one object is moves.
 * When an item is about to move (using drag-and-drop), it makes shure that
 * it's not moved right after an object.
 */
void Outline::slotDropped( QDropEvent * /* e */, QListViewItem *parent, QListViewItem *target )
{
    kdDebug(33001) << "slotDropped" << endl;
    /* slide doesn't have parent (always 0)
     * Only slides can move at the moment, objects can't. */
    if ( parent )
        return;

    // This code is taken from KListView
    for (QListViewItem *i = firstChild(), *iNext = 0; i != 0; i = iNext)
    {
        iNext = i->itemBelow();
        if ( !i->isSelected() )
            continue;

        // don't drop an item after itself, or else
        // it moves to the top of the list
        if ( i == target )
            continue;

        i->setSelected( false );

        // don't move the item as it is allready
        moveItem(i, parent, target );

        // Only one item can be moved
        break;
    }
}

// We have to overwrite this method as it checks if an item is movable
// and we have disabled it.
bool Outline::acceptDrag( QDropEvent* e ) const
{
    return acceptDrops() && (e->source()==viewport());
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

void Outline::moveItem( QListViewItem *i, QListViewItem *, QListViewItem *newAfter )
{
    OutlineSlideItem* srcItem = dynamic_cast<OutlineSlideItem*>( i );
    if ( !srcItem )
        return;

    int num = m_doc->pageList().findRef( srcItem->page() );

    int numNow = 0;
    if ( newAfter )
    {
        OutlineSlideItem* dstItem = dynamic_cast<OutlineSlideItem*>( newAfter );
        if( !dstItem )
            return;

        numNow = m_doc->pageList().findRef( dstItem->page() );
        if ( numNow < num )
            numNow++;
    }

    if ( num!=numNow )
        m_doc->movePage( num, numNow );
}

void Outline::rightButtonPressed( QListViewItem *, const QPoint &pnt, int )
{
    if ( !m_doc->isReadWrite() || m_viewMasterPage ) return;

    QListViewItem *item = QListView::selectedItem();
    if( !item ) return;

    OutlineSlideItem* slideItem = dynamic_cast<OutlineSlideItem*>(item);
    if( !slideItem ) return;

    m_view->openPopupMenuSideBar(pnt);
}

void Outline::renamePageTitle()
{
    QListViewItem *item = QListView::selectedItem();
    if( !item || m_viewMasterPage) return;

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
            KPrChangeTitlePageNameCommand *cmd=new KPrChangeTitlePageNameCommand( i18n("Rename Slide"),
                                                                                  m_doc, activeTitle, newTitle, page  );
            cmd->execute();
            m_doc->addCommand(cmd);
        }
    }
}


#include "sidebar.moc"
