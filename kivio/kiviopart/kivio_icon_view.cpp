/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2001 theKompany.com & Dave Marotti
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include "kivio_icon_view.h"

#include "kivio_common.h"
#include "kiviodragobject.h"
#include "object.h"
#include "kivioglobal.h"

#include <qcursor.h>
#include <klocale.h>
#include <kdebug.h>

/**********************************************************************
 *
 * KivioIconViewItem
 *
 **********************************************************************/
KivioIconViewItem::KivioIconViewItem( QIconView *parent )
    : QIconViewItem( parent )
{
  m_shape = 0;
  setText(i18n("untitled shape", "Untitled"));
}

KivioIconViewItem::~KivioIconViewItem()
{
  m_shape = 0;
}

void KivioIconViewItem::setShape(Kivio::Object* newShape)
{
  m_shape = newShape;

  if(!m_shape) {
    setText(i18n("untitled shape", "Untitled"));
  } else {
    setText(m_shape->name());
    setPixmap(Kivio::generatePixmapFromObject(32, 32, m_shape));
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
KivioIconView::KivioIconView( bool _readWrite,QWidget *parent, const char *name )
: QIconView( parent, name )
{
  m_shapeCollection = 0;
  isReadWrite = _readWrite;

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

  if(isReadWrite) {
    connect(this, SIGNAL(doubleClicked(QIconViewItem *)), this, SLOT(slotDoubleClicked(QIconViewItem*)));
  }
}

KivioIconView::~KivioIconView()
{
}

QDragObject *KivioIconView::dragObject()
{
  if( !currentItem() || !isReadWrite)
    return 0;

  QPoint orig = viewportToContents( viewport()->mapFromGlobal( QCursor::pos() ) );
  KivioDragObject* dragObject = new KivioDragObject(this, "shapeDrag");

  KivioIconViewItem *item = (KivioIconViewItem *)currentItem();
  dragObject->addObject(item->shape());


  return dragObject;
}

void KivioIconView::slotDoubleClicked( QIconViewItem *pQtItem )
{
  //FIXME We want to make this work as a tool when clicked / double clicked
/*  KivioStencilSpawner *pSpawner;

  KivioIconViewItem *pItem = dynamic_cast<KivioIconViewItem *>(pQtItem);

  if( !pItem )
  {
    kdDebug(43000) << "KivioIconView::slotDoubleClicked() - Clicked item is not a KivioIconViewItem!" << endl;
    return;
  }

  pSpawner = pItem->spawner();

  emit createNewStencil( pSpawner );*/
}

#include "kivio_icon_view.moc"
