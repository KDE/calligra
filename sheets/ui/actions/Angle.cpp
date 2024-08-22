/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "Angle.h"
#include "Actions.h"
#include "ui/actions/dialogs/AngleDialog.h"

#include <KLocalizedString>

#include "core/Cell.h"
#include "core/Style.h"
#include "ui/Selection.h"
#include "ui/commands/RowColumnManipulators.h"
#include "ui/commands/StyleCommand.h"

#include "KoCanvasBase.h"

using namespace Calligra::Sheets;

Angle::Angle(Actions *actions)
    : DialogCellAction(actions, "changeAngle", i18n("Change Angle..."), QIcon(), i18n("Change the angle that cell contents are printed"))
{
}

Angle::~Angle() = default;

QAction *Angle::createAction()
{
    QAction *res = CellAction::createAction();
    res->setIconText(i18n("Angle"));
    return res;
}

ActionDialog *Angle::createDialog(QWidget *canvasWidget)
{
    AngleDialog *dlg = new AngleDialog(canvasWidget, 0);
    connect(dlg, &AngleDialog::adjustAngle, this, &Angle::adjustAngle);
    return dlg;
}

void Angle::onSelectionChanged()
{
    Cell cell = activeCell();
    int angle = -1 * cell.style().angle();
    AngleDialog *dlg = dynamic_cast<AngleDialog *>(m_dlg);
    dlg->setAngle(angle);
}

void Angle::adjustAngle(int angle)
{
    Sheet *sheet = m_selection->activeSheet();
    KUndo2Command *macroCommand = new KUndo2Command(kundo2_i18n("Change Angle"));

    StyleCommand *manipulator = new StyleCommand(macroCommand);
    manipulator->setSheet(sheet);
    Style s;
    s.setAngle(-1 * angle);
    manipulator->setStyle(s);
    manipulator->add(*m_selection);

    AdjustColumnRowManipulator *manipulator2 = new AdjustColumnRowManipulator(macroCommand);
    manipulator2->setSheet(sheet);
    manipulator2->setAdjustColumn(true);
    manipulator2->setAdjustRow(true);
    manipulator2->add(*m_selection);

    m_selection->canvas()->addCommand(macroCommand);
}
