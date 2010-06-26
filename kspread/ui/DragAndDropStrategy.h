/* This file is part of the KDE project
   Copyright 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_DRAG_AND_DROP_STRATEGY
#define KSPREAD_DRAG_AND_DROP_STRATEGY

#include "AbstractSelectionStrategy.h"
#include "kspread_export.h"

namespace KSpread
{

/**
 * A strategy for dragging cells.
 *
 * The drag starts, if the mouse leaves the cell, where the mouse press occurred.
 * If this cell is not left, the cell becomes selected.
 */
class KSPREAD_EXPORT DragAndDropStrategy : public AbstractSelectionStrategy
{
public:
    /**
     * Constructor.
     */
    DragAndDropStrategy(CellToolBase *cellTool, const QPointF position, Qt::KeyboardModifiers modifiers);

    /**
     * Destructor.
     */
    virtual ~DragAndDropStrategy();

    virtual void handleMouseMove(const QPointF& mouseLocation, Qt::KeyboardModifiers modifiers);
    virtual QUndoCommand* createCommand();

private:
    class Private;
    Private * const d;
};

} // namespace KSpread

#endif // KSPREAD_DRAG_AND_DROP_STRATEGY
