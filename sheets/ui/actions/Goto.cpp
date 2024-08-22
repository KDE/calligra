/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "Goto.h"
#include "Actions.h"
#include "ui/CellToolBase.h"
#include "ui/actions/dialogs/GotoDialog.h"

#include <QAction>

#include <KLocalizedString>
#include <KMessageBox>

#include "core/Sheet.h"
#include "engine/MapBase.h"
#include "engine/NamedAreaManager.h"

using namespace Calligra::Sheets;

Goto::Goto(Actions *actions)
    : DialogCellAction(actions, "gotoCell", i18n("Goto Cell..."), koIcon("go-jump"), i18n("Move to a particular cell"))
{
}

Goto::~Goto() = default;

QAction *Goto::createAction()
{
    QAction *res = CellAction::createAction();
    res->setIconText(i18n("Goto"));
    res->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_G));
    return res;
}

void Goto::gotoCell(const QString &name)
{
    Sheet *cursheet = m_selection->activeSheet();
    Region region = cursheet->map()->regionFromName(name, cursheet);
    if (!region.isValid()) {
        KMessageBox::error(m_canvasWidget, i18n("That is not a valid cell."));
        return;
    }

    Sheet *firstSheet = dynamic_cast<Sheet *>(region.firstSheet());
    if (firstSheet != cursheet)
        m_selection->emitVisibleSheetRequested(firstSheet);
    m_selection->initialize(region);
    m_actions->tool()->scrollToCell(m_selection->cursor());
}

ActionDialog *Goto::createDialog(QWidget *canvasWidget)
{
    m_canvasWidget = canvasWidget;
    NamedAreaManager *manager = m_selection->activeSheet()->map()->namedAreaManager();
    GotoDialog *dlg = new GotoDialog(canvasWidget, manager->areaNames());
    connect(dlg, &GotoDialog::gotoCell, this, &Goto::gotoCell);
    return dlg;
}
