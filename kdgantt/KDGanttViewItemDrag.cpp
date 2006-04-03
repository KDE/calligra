

/****************************************************************************
 ** Copyright (C)  2002-2004 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KDGantt library.
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.
 **
 ** Licensees holding valid commercial KDGantt licenses may use this file in
 ** accordance with the KDGantt Commercial License Agreement provided with
 ** the Software.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** See http://www.klaralvdalens-datakonsult.se/Public/products/ for
 **   information about KDGantt Commercial License Agreements.
 **
 ** Contact info@klaralvdalens-datakonsult.se if any conditions of this
 ** licensing are not clear to you.
 **
 ** As a special exception, permission is given to link this program
 ** with any edition of Qt, and distribute the resulting executable,
 ** without including the source code for Qt in the source distribution.
 **
 **********************************************************************/


#include <KDGanttViewItemDrag.h>
#include <KDGanttViewItem.h>
#include <qpixmap.h>
#include <KDGanttView.h>

/*!
  \class KDGanttViewItemDrag KDGanttViewItemDrag.h
  \brief Drag and drop of KD Gantt items.

  This class implements drag and drop of KD Gantt items within a Gantt
  chart. It is mainly used for internal purposes, but made a part of
  the public API nevertheless, as you may want to subclass it for some
  specialized functionality.
*/


/*!
  The constructor. Creates a KDGanttViewItemDrag object and
  initializes the drag data in the form of an XML document.

  \param item the item that is dragged
  \param source the source widget
  \param name the internal object name
*/
KDGanttViewItemDrag::KDGanttViewItemDrag( KDGanttViewItem* item , QWidget *source,  const char * name  ) : QStoredDrag("x-application/x-KDGanttViewItemDrag", source,  name )
{
    myItem = item;

    QPixmap pix;
    if (item->pixmap() )
        pix = *(item->pixmap()) ;
    else {
        KDGanttViewItem::Shape start,  middle, end;
        item->shapes( start, middle, end );
        QColor st, mi, en;
        item->colors( st, mi, en );
        pix =item->myGanttView->getPixmap( start, st, item->myGanttView->lvBackgroundColor(), 11 );
    }
    setPixmap( pix , QPoint( -10,-10 ));
    QDomDocument doc( "GanttView" );
    QString docstart = "<GanttView/>";
    doc.setContent( docstart );
    QDomElement itemsElement = doc.createElement( "Items" );
    doc.documentElement().appendChild( itemsElement );
    item->createNode( doc, itemsElement );
    QDataStream s( array, IO_WriteOnly );
    s << doc.toString();
}


/*!
  Returns the encoded data of the drag object.

  \param c the format of the data
  \return the encoded data of the drag object
*/
QByteArray KDGanttViewItemDrag::encodedData( const char * c) const
{
    QString s ( c );
    if ( s == "x-application/x-KDGanttViewItemDrag" ) {
        return array;
    }
    return QByteArray();
}

/*!
  Returns the dragged item

  \return the dragged item
*/
KDGanttViewItem* KDGanttViewItemDrag::getItem()
{
    return myItem;
}


/*!
  Returns whether this drag object class can decode the data passed in \a e.

  \param e the mime source that has been dragged
  \return true if KDGanttViewItemDrag can decode the data in \a e.
*/
bool KDGanttViewItemDrag::canDecode (  const QMimeSource * e )
{
    if ( QString( e->format() ) == "x-application/x-KDGanttViewItemDrag" )
        return true;

    return false;
}


/*!
  Decodes the data passed in \a e into an XML string that is written
  into \a string.

  \param e the data to decode
  \param string the resulting XML string
  \return true if the operation succeeded
*/
bool KDGanttViewItemDrag::decode (  const QMimeSource * e , QString &  string)
{
    QByteArray arr;
    arr = e->encodedData( "x-application/x-KDGanttViewItemDrag");
    QDataStream s( arr, IO_ReadOnly );
    s >> string;
    return true;
}

