/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000 theKompany.com
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include "kivio_icon_view.h"

#include "kivio_config.h"
#include "kivio_stencil_spawner.h"
#include "kivio_stencil_spawner_set.h"
#include "kivio_stencil_spawner_info.h"
#include "kivio_spawner_drag.h"
#include "kivio_icon_view.h"

#include <qbrush.h>
#include <qpalette.h>


KivioStencilSpawner* KivioIconView::m_pCurDrag = 0L;
/**********************************************************************
 *
 * KivioIconViewItem
 *
 **********************************************************************/
KivioIconViewItem::KivioIconViewItem( QIconView *parent )
    : QIconViewItem( parent )
{
    m_pSpawner = NULL;
    setText("stencil");
}

KivioIconViewItem::~KivioIconViewItem()
{
    m_pSpawner = NULL;
}

void KivioIconViewItem::setStencilSpawner( KivioStencilSpawner *pSpawn )
{
    KivioStencilSpawnerInfo *pInfo;

    m_pSpawner = pSpawn;

    if( !m_pSpawner )
    {
        setText( "Untitled" );
    }
    else
    {
        pInfo = m_pSpawner->info();
        setText( pInfo->title() );
        setPixmap( *(m_pSpawner->icon()) );
    }

}

bool KivioIconViewItem::acceptDrop( const QMimeSource * ) const
{
    return false;
}


/**********************************************************************
 *
 * KivioIconView
 *
 **********************************************************************/
KivioIconView::KivioIconView( QWidget *parent, const char *name )
    : QIconView( parent, name )
{
    m_pSpawnerSet = NULL;
    m_pCurDrag = NULL;

    m_bgType = KivioConfig::config()->stencilBGType();
    m_bgColor = KivioConfig::config()->stencilBGColor();
    m_pBackground = KivioConfig::config()->stencilBGPixmap();

    setGridX( 64 );
    setGridY( 64 );
    setResizeMode( Adjust );
    setWordWrapIconText(true);
    setHScrollBarMode( AlwaysOff );
    setVScrollBarMode( Auto );
    setAutoArrange(true);
    setSorting(true);

    setItemsMovable(false);

    setArrangement(LeftToRight);
    setAcceptDrops(false);
    viewport()->setAcceptDrops(false);
    
    QObject::connect( this, SIGNAL(doubleClicked(QIconViewItem *)), this, SLOT(slotDoubleClicked(QIconViewItem*)) );
}

KivioIconView::~KivioIconView()
{
    m_pCurDrag = NULL;

    m_pBackground = NULL;
}

void KivioIconView::setStencilSpawnerSet( KivioStencilSpawnerSet *pSet )
{
    m_pSpawnerSet = pSet;
    m_pCurDrag = NULL;

    KivioStencilSpawner *pSpawner;
    KivioIconViewItem *pItem;

    pSpawner = pSet->spawners()->first();
    while( pSpawner )
    {
        pItem = new KivioIconViewItem( this );
        pItem->setKey(pSpawner->info()->title());
        pItem->setStencilSpawner(pSpawner);

        qDebug("* %s", pSpawner->info()->title().ascii());

        pSpawner = pSet->spawners()->next();

    }
    qDebug("Leaving");
}

void KivioIconView::drawBackground( QPainter *p, const QRect &r )
{
    QBrush b;
    p->setBrushOrigin(-contentsX(),-contentsY());

    if( m_bgType == KivioConfig::sbgtColor )
    {
        b.setColor( m_bgColor );
        b.setStyle( QBrush::SolidPattern );
    }
    else if( m_bgType == KivioConfig::sbgtPixmap )
    {
        b.setPixmap(*m_pBackground);
    }
    p->fillRect( r, b );
}

QDragObject *KivioIconView::dragObject()
{
    if( !currentItem() )
        return 0;

    QPoint orig = viewportToContents( viewport()->mapFromGlobal( QCursor::pos() ) );
    KivioSpawnerDrag *drag = new KivioSpawnerDrag( this, viewport() );

    const char*null_pix[]={
    "1 1 1 1",
    "# c None",
    "#"};
    drag->setPixmap(null_pix);

    KivioIconViewItem *item = (KivioIconViewItem *)currentItem();

    QIconDragItem id;
    QString full;
    full = item->spawner()->set()->dir() + "/" + item->spawner()->info()->title();
    id.setData( QCString(full.latin1())); // ### unicode fixme !!! (Simon)
    drag->append( id,
        QRect( item->pixmapRect(FALSE).x() - orig.x(),
               item->pixmapRect(FALSE).y() - orig.y(),
               item->pixmapRect().width(),
               item->pixmapRect().height() ),
        QRect( item->textRect(FALSE).x() - orig.x(),
               item->textRect(FALSE).y() - orig.y(),
               item->textRect().width(),
               item->textRect().height() ),
        *(item->spawner()) );


    // Set the current dragged KivioStencilSpawner for use
    // when the drop occurs.  I don't actually encode the
    // data because it's a pain in the ass and I don't understand
    // how to do it.  So I store a pointer here and clear
    // it on the drop.
    m_pCurDrag = item->spawner();

    return drag;
}

void KivioIconView::clearCurrentDrag()
{
    m_pCurDrag = NULL;
}


void KivioIconView::slotDoubleClicked( QIconViewItem *pQtItem )
{
    KivioStencilSpawner *pSpawner;
    
    KivioIconViewItem *pItem = dynamic_cast<KivioIconViewItem *>(pQtItem);
    
    if( !pItem )
    {
        qDebug("KivioIconView::slotDoubleClicked() - Clicked item is not a KivioIconViewItem!");
        return;
    }
    
    pSpawner = pItem->spawner();

    emit createNewStencil( pSpawner );
}
