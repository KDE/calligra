/****************************************************************************
 ** Copyright (C) 2001-2006 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KD Gantt library.
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.
 **
 ** Licensees holding valid commercial KD Gantt licenses may use this file in
 ** accordance with the KD Gantt Commercial License Agreement provided with
 ** the Software.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** See http://www.kdab.net/kdgantt for
 **   information about KD Gantt Commercial License Agreements.
 **
 ** Contact info@kdab.net if any conditions of this
 ** licensing are not clear to you.
 **
 **********************************************************************/
#include "kdganttstyleoptionganttitem.h"

using namespace KDGantt;

/*!\class KDGantt::StyleOptionGanttItem kdganttstyleoptionganttitem.h KDGanttStyleOptionGanttItem
 * \ingroup KDGantt
 * \brief QStyleOption subclass for gantt items.
 */

typedef QStyleOptionViewItem BASE;

/*! Constructor. Sets grid to 0. */
StyleOptionGanttItem::StyleOptionGanttItem()
    : BASE(),
      grid( 0 )
{
    type = QStyleOption::SO_CustomBase+89;
    version = 1;
}

/*! Copy constructor. Creates a copy of \a other */
StyleOptionGanttItem::StyleOptionGanttItem( const StyleOptionGanttItem& other )
    : BASE(other)
{
    operator=( other );
}

/*! Assignment operator */
StyleOptionGanttItem& StyleOptionGanttItem::operator=( const StyleOptionGanttItem& other )
{
    BASE::operator=( other );
    boundingRect = other.boundingRect;
    itemRect = other.itemRect;
    displayPosition = other.displayPosition;
    grid = other.grid;
    text = other.text;
    return *this;
}

#ifndef QT_NO_DEBUG_STREAM

QDebug operator<<( QDebug dbg, const KDGantt::StyleOptionGanttItem& s )
{
    dbg << "KDGantt::StyleOptionGanttItem[ boundingRect="<<s.boundingRect
        <<", itemRect="<<s.itemRect
        <<", displayPosition="<<s.displayPosition
        <<", grid="<<s.grid
        <<", text="<<s.text
        <<"]";
    return dbg;
}

#endif /* QT_NO_DEBUG_STREAM */


/*!\enum KDGantt::StyleOptionGanttItem::Position
 * This enum is used to describe where the Qt::DisplayRole
 * (the label) should be located relative to the item itself.
 */

/*!\var StyleOptionGanttItem::boundingRect
 * Contains the bounding rectangle for the item
 */

/*!\var StyleOptionGanttItem::itemRect
 * Contains the "active" item rectangle that corresponds
 * to the values from the model.
 */

/*!\var StyleOptionGanttItem::displayPosition
 * \see StyleOptionGanttItem::Position.
 */

/*!\var StyleOptionGanttItem::grid
 * Contains a pointer to the AbstractGrid used by the view
 */

/*!\var StyleOptionGanttItem::text
 * Contains a string printed to the item
 */
