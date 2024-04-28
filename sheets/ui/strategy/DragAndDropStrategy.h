/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_DRAG_AND_DROP_STRATEGY
#define CALLIGRA_SHEETS_DRAG_AND_DROP_STRATEGY

#include "../AbstractSelectionStrategy.h"
#include "../sheets_ui_export.h"

namespace Calligra
{
namespace Sheets
{

/**
 * A strategy for dragging cells.
 *
 * The drag starts, if the mouse leaves the cell, where the mouse press occurred.
 * If this cell is not left, the cell becomes selected.
 */
class CALLIGRA_SHEETS_UI_EXPORT DragAndDropStrategy : public AbstractSelectionStrategy
{
public:
    /**
     * Constructor.
     */
    DragAndDropStrategy(CellToolBase *cellTool, const QPointF &position, Qt::KeyboardModifiers modifiers);

    /**
     * Destructor.
     */
    ~DragAndDropStrategy() override;

    void handleMouseMove(const QPointF &mouseLocation, Qt::KeyboardModifiers modifiers) override;
    KUndo2Command *createCommand() override;

    bool dragStarted() const;

private:
    class Private;
    Private *const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_DRAG_AND_DROP_STRATEGY
