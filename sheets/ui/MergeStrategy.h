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

#ifndef CALLIGRA_SHEETS_MERGE_STRATEGY
#define CALLIGRA_SHEETS_MERGE_STRATEGY

#include "AbstractSelectionStrategy.h"
#include "sheets_common_export.h"

namespace Calligra
{
namespace Sheets
{

/**
 * A strategy for merging cells.
 *
 * Created, when the user presses the middle mouse button on the selection handle.
 * Nothing happens, if the selection did not change.
 */
class CALLIGRA_SHEETS_COMMON_EXPORT MergeStrategy : public AbstractSelectionStrategy
{
public:
    /**
     * Constructor.
     */
    MergeStrategy(CellToolBase *cellTool, const QPointF &position, Qt::KeyboardModifiers modifiers);

    /**
     * Destructor.
     */
    ~MergeStrategy() override;

    KUndo2Command* createCommand() override;

private:
    class Private;
    Private * const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_MERGE_STRATEGY
