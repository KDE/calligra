/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_PASTE_STRATEGY
#define CALLIGRA_SHEETS_PASTE_STRATEGY

#include "../sheets_ui_export.h"
#include "SelectionStrategy.h"

namespace Calligra
{
namespace Sheets
{

/**
 * A strategy for pasting the global mouse selection.
 *
 * On creation, the cell in which the mouse press occurred is selected.
 *
 * \see QClipboard::Selection
 */
class CALLIGRA_SHEETS_UI_EXPORT PasteStrategy : public SelectionStrategy
{
public:
    /**
     * Constructor.
     */
    PasteStrategy(CellToolBase *cellTool, const QPointF &position, Qt::KeyboardModifiers modifiers);

    /**
     * Destructor.
     */
    ~PasteStrategy() override;

    KUndo2Command *createCommand() override;

private:
    class Private;
    Private *const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_PASTE_STRATEGY
