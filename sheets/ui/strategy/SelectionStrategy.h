/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_SELECTION_STRATEGY
#define CALLIGRA_SHEETS_SELECTION_STRATEGY

#include "../AbstractSelectionStrategy.h"
#include "../sheets_ui_export.h"

namespace Calligra
{
namespace Sheets
{

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
class CALLIGRA_SHEETS_UI_EXPORT SelectionStrategy : public AbstractSelectionStrategy
{
public:
    /**
     * Constructor.
     */
    SelectionStrategy(CellToolBase *cellTool, const QPointF &position, Qt::KeyboardModifiers modifiers);

    /**
     * Destructor.
     */
    ~SelectionStrategy() override;

    void handleMouseMove(const QPointF &position, Qt::KeyboardModifiers modifiers) override;

private:
    class Private;
    Private *const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_SELECTION_STRATEGY
