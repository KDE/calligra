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
#include "ui/commands/StyleCommand.h"
#include "ui/commands/RowColumnManipulators.h"

#include "KoCanvasBase.h"



using namespace Calligra::Sheets;


Angle::Angle(Actions *actions)
    : CellAction(actions, "changeAngle", i18n("Change Angle..."), QIcon(), i18n("Change the angle that cell contents are printed"))
    , m_dlg(nullptr)
{
}

Angle::~Angle()
{
    if (m_dlg) delete m_dlg;
}

QAction *Angle::createAction() {
    QAction *res = CellAction::createAction();
    res->setIconText(i18n("Angle"));
    return res;
}



void Angle::execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget)
{
    QPoint marker(selection->marker());
    Cell cell(sheet, marker);
    int angle = -1 * cell.style().angle();
    m_dlg = new AngleDialog(canvasWidget, angle);

    if (m_dlg->exec() == QDialog::Accepted) {
        int angle = m_dlg->angle();

        KUndo2Command* macroCommand = new KUndo2Command(kundo2_i18n("Change Angle"));

        StyleCommand* manipulator = new StyleCommand(macroCommand);
        manipulator->setSheet(sheet);
        Style s;
        s.setAngle(-1 * angle);
        manipulator->setStyle(s);
        manipulator->add(*selection);

        AdjustColumnRowManipulator* manipulator2 = new AdjustColumnRowManipulator(macroCommand);
        manipulator2->setSheet(sheet);
        manipulator2->setAdjustColumn(true);
        manipulator2->setAdjustRow(true);
        manipulator2->add(*selection);

        selection->canvas()->addCommand (macroCommand);
    }    

    delete m_dlg;
    m_dlg = nullptr;
}




