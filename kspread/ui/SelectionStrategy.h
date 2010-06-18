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

#ifndef KSPREAD_SELECTION_STRATEGY
#define KSPREAD_SELECTION_STRATEGY

#include "AbstractSelectionStrategy.h"
#include "kspread_export.h"

namespace KSpread
{
class Selection;

/**
 * A strategy for selecting cell ranges.
 *
 * On creation the cell the mouse press occurred in is selected.
 *
 * For the case, that you are only interested in updating the selection on mouse movements, derive
 * directly from AbstractSelectionStrategy.
 *
 * \see AbstractSelectionStrategy
 */
class KSPREAD_EXPORT SelectionStrategy : public AbstractSelectionStrategy
{
public:
    /**
     * Constructor.
     */
    SelectionStrategy(KoToolBase *parent, Selection *selection,
                      const QPointF position, Qt::KeyboardModifiers modifiers);

    /**
     * Destructor.
     */
    virtual ~SelectionStrategy();

    virtual void handleMouseMove(const QPointF &position, Qt::KeyboardModifiers modifiers);

private:
    class Private;
    Private * const d;
};

} // namespace KSpread

#endif // KSPREAD_SELECTION_STRATEGY
