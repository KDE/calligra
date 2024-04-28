/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_AUTOFILL_STRATEGY
#define CALLIGRA_SHEETS_AUTOFILL_STRATEGY

#include "../AbstractSelectionStrategy.h"
#include "../sheets_ui_export.h"

namespace Calligra
{
namespace Sheets
{

/**
 * A strategy for automatic filling values into selected cells.
 *
 * Created, when the user presses the left mouse button on the selection handle.
 * Nothing happens, if the selection did not change.
 */
class CALLIGRA_SHEETS_UI_EXPORT AutoFillStrategy : public AbstractSelectionStrategy
{
public:
    /**
     * Constructor.
     */
    AutoFillStrategy(CellToolBase *cellTool, const QPointF &position, Qt::KeyboardModifiers modifiers);

    /**
     * Destructor.
     */
    ~AutoFillStrategy() override;

    KUndo2Command *createCommand() override;

private:
    class Private;
    Private *const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_AUTOFILL_STRATEGY
