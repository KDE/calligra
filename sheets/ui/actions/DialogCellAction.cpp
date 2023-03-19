/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2023 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022-2023 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "DialogCellAction.h"
#include "dialogs/ActionDialog.h"


using namespace Calligra::Sheets;

DialogCellAction::DialogCellAction(Actions *actions, const QString &actionName, const QString &caption, const QIcon &icon, const QString &tooltip)
    : CellAction(actions, actionName, caption, icon, tooltip)
    , m_dlg(nullptr)
{

}

DialogCellAction::~DialogCellAction()
{
    if (m_dlg) delete m_dlg;
}


void DialogCellAction::execute(Selection *selection, Sheet *, QWidget *canvasWidget)
{
    m_selection = selection;

    if (!m_dlg) {
        m_dlg = createDialog(canvasWidget);
        connect(m_dlg, &ActionDialog::finished, this, &DialogCellAction::onDialogClosed);
    }
    m_dlg->show();
    m_dlg->raise();
    m_dlg->activateWindow();
}

void DialogCellAction::onDialogClosed()
{
    if (!m_dlg) return;
    disconnect(m_dlg, &ActionDialog::finished, this, &DialogCellAction::onDialogClosed);
    m_dlg->deleteLater();
    m_dlg = nullptr;
}


