/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "Cells.h"
#include "Actions.h"

#include "ui/actions/dialogs/InsertDialog.h"
#include "ui/commands/DataManipulators.h"
#include "ui/commands/RowColumnManipulators.h"

#include <KLocalizedString>

using namespace Calligra::Sheets;

CellsInsert::CellsInsert(Actions *actions)
    : CellAction(actions, "insertCell", i18n("Insert Cells..."), koIcon("insertcell"), i18n("Insert a blank cell into the spreadsheet"))
    , m_dlg(nullptr)
{
}

CellsInsert::~CellsInsert()
{
    if (m_dlg)
        delete m_dlg;
}

QAction *CellsInsert::createAction()
{
    QAction *res = CellAction::createAction();
    res->setIconText(i18n("Insert Cells..."));
    return res;
}

void CellsInsert::execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget)
{
    m_dlg = new InsertDialog(canvasWidget, InsertDialog::Insert);

    if (m_dlg->exec() == QDialog::Accepted) {
        AbstractRegionCommand *cmd = nullptr;
        if (m_dlg->moveRowsChecked()) {
            ShiftManipulator *manipulator = new ShiftManipulator();
            cmd = manipulator;
            manipulator->setDirection(ShiftManipulator::ShiftRight);
        } else if (m_dlg->moveColumnsChecked()) {
            ShiftManipulator *manipulator = new ShiftManipulator();
            cmd = manipulator;
            manipulator->setDirection(ShiftManipulator::ShiftBottom);
        } else if (m_dlg->insertRowsChecked()) {
            InsertDeleteRowManipulator *manipulator = new InsertDeleteRowManipulator();
            cmd = manipulator;
        } else if (m_dlg->insertColumnsChecked()) {
            InsertDeleteColumnManipulator *manipulator = new InsertDeleteColumnManipulator();
            cmd = manipulator;
        }
        if (cmd) {
            cmd->setSheet(sheet);
            cmd->add(*selection);
            cmd->execute(selection->canvas());
        }
    }

    delete m_dlg;
    m_dlg = nullptr;
}

CellsRemove::CellsRemove(Actions *actions)
    : CellAction(actions, "deleteCell", i18n("Remove Cells..."), koIcon("removecell"), i18n("Removes the cells from the spreadsheet"))
    , m_dlg(nullptr)
{
}

CellsRemove::~CellsRemove()
{
    if (m_dlg)
        delete m_dlg;
}

QAction *CellsRemove::createAction()
{
    QAction *res = CellAction::createAction();
    res->setIconText(i18n("Remove Cells..."));
    return res;
}

void CellsRemove::execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget)
{
    m_dlg = new InsertDialog(canvasWidget, InsertDialog::Remove);

    if (m_dlg->exec() == QDialog::Accepted) {
        AbstractRegionCommand *cmd = nullptr;
        if (m_dlg->moveRowsChecked()) {
            ShiftManipulator *manipulator = new ShiftManipulator();
            cmd = manipulator;
            manipulator->setDirection(ShiftManipulator::ShiftRight);
            manipulator->setRemove(true);
        } else if (m_dlg->moveColumnsChecked()) {
            ShiftManipulator *manipulator = new ShiftManipulator();
            cmd = manipulator;
            manipulator->setDirection(ShiftManipulator::ShiftBottom);
            manipulator->setRemove(true);
        } else if (m_dlg->insertRowsChecked()) {
            InsertDeleteRowManipulator *manipulator = new InsertDeleteRowManipulator();
            cmd = manipulator;
            manipulator->setDelete(true);
        } else if (m_dlg->insertColumnsChecked()) {
            InsertDeleteColumnManipulator *manipulator = new InsertDeleteColumnManipulator();
            cmd = manipulator;
            manipulator->setDelete(true);
        }
        if (cmd) {
            cmd->setSheet(sheet);
            cmd->add(*selection);
            cmd->execute(selection->canvas());
        }
    }

    delete m_dlg;
    m_dlg = nullptr;
}
