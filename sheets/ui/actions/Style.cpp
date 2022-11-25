/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "Style.h"
#include "Actions.h"

#include "ui/commands/StyleCommand.h"
#include "ui/CellEditorBase.h"
#include "ui/CellToolBase.h"

#include <KoCanvasBase.h>

#include <KLocalizedString>



using namespace Calligra::Sheets;


Bold::Bold(Actions *actions)
    : ToggleableCellAction(actions, "bold", i18n("Bold"), koIcon("format-text-bold"), i18n("Bold"))
{
}

Bold::~Bold()
{
}

QAction *Bold::createAction() {
    QAction *res = ToggleableCellAction::createAction();
    res->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_B));
    return res;
}

void Bold::executeToggled(bool selected, Selection *selection, Sheet *sheet, QWidget *)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(sheet);
    command->setText(kundo2_i18n("Change Font"));
    Style s;
    s.setFontBold(selected);
    command->setStyle(s);
    command->add(*selection);
    command->execute(selection->canvas());
    if (m_actions->tool()->editor()) {
        const Cell cell = Cell(sheet, selection->marker());
        m_actions->tool()->editor()->setEditorFont(cell.style().font(), true, selection->canvas()->viewConverter());
    }
}

bool Bold::checkedForSelection(Selection *, const Cell &activeCell)
{
    const Style style = activeCell.style();
    return style.bold();
}

