/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2023 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022-2023 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_DIALOG_CELL_ACTION
#define CALLIGRA_SHEETS_DIALOG_CELL_ACTION

#include "CellAction.h"

namespace Calligra
{
namespace Sheets
{

class ActionDialog;

/**
 * Subclass of CellAction and a base class of all the actions that use a non-modal dialog.
 */

class DialogCellAction : public CellAction
{
    Q_OBJECT
public:
    DialogCellAction(Actions *actions, const QString &actionName, const QString &caption, const QIcon &icon, const QString &tooltip);
    virtual ~DialogCellAction();

protected Q_SLOTS:
    void onDialogClosed();
    void activeSheetChanged(Sheet *sheet);
    void selectionChanged(const Region &region);
    void handleDamages();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;

    virtual ActionDialog *createDialog(QWidget *canvasWidget) = 0;
    /** Called when the selection changes. Also called when opening the dialog, to simplify the logic. */
    virtual void onSelectionChanged()
    {
    }

    Cell activeCell() const;

    ActionDialog *m_dlg;
    Selection *m_selection;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_DIALOG_CELL_ACTION
