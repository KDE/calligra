/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2023 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022-2023 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "DialogCellAction.h"
#include "dialogs/ActionDialog.h"

#include "core/Sheet.h"
#include "engine/Damages.h"
#include "engine/MapBase.h"
#include "ui/Selection.h"

using namespace Calligra::Sheets;

DialogCellAction::DialogCellAction(Actions *actions, const QString &actionName, const QString &caption, const QIcon &icon, const QString &tooltip)
    : CellAction(actions, actionName, caption, icon, tooltip)
    , m_dlg(nullptr)
    , m_selection(nullptr)
{
}

DialogCellAction::~DialogCellAction()
{
    if (m_dlg)
        delete m_dlg;
}

void DialogCellAction::execute(Selection *selection, Sheet *, QWidget *canvasWidget)
{
    if (!m_selection) {
        m_selection = selection;
        connect(m_selection, &Selection::activeSheetChanged, this, &DialogCellAction::activeSheetChanged);
        connect(m_selection, &Selection::changed, this, &DialogCellAction::selectionChanged);
        connect(m_selection->activeSheet()->map(), &MapBase::damagesFlushed, this, &DialogCellAction::handleDamages);
    }

    if (!m_dlg) {
        m_dlg = createDialog(canvasWidget);
        if (!m_dlg)
            return; // No dialog? Nothing to do.
        connect(m_dlg, &ActionDialog::finished, this, &DialogCellAction::onDialogClosed);
    }

    m_dlg->show();
    m_dlg->raise();
    m_dlg->activateWindow();

    // Update everything necessary.
    onSelectionChanged();
    m_dlg->onSelectionChanged(m_selection);
}

Cell DialogCellAction::activeCell() const
{
    return Cell(m_selection->activeSheet(), m_selection->cursor());
}

void DialogCellAction::activeSheetChanged(Sheet *)
{
    onSelectionChanged();
    if (m_dlg)
        m_dlg->onSelectionChanged(m_selection);
}

/** This ensures that the UI updates when we change cell content. */
void DialogCellAction::handleDamages()
{
    if (!m_dlg)
        return; // Only if the dialog is active.
    onSelectionChanged();
    m_dlg->onSelectionChanged(m_selection);
}

void DialogCellAction::selectionChanged(const Region &)
{
    if (!m_dlg)
        return; // Only if the dialog is active.
    onSelectionChanged();
    m_dlg->onSelectionChanged(m_selection);
}

void DialogCellAction::onDialogClosed()
{
    if (!m_dlg)
        return;
    disconnect(m_dlg, &ActionDialog::finished, this, &DialogCellAction::onDialogClosed);
    m_dlg->deleteLater();
    m_dlg = nullptr;
}
