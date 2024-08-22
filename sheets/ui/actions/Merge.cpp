/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "Merge.h"
#include "Actions.h"
#include "ui/commands/MergeCommand.h"

#include <KLocalizedString>

using namespace Calligra::Sheets;

Merge::Merge(Actions *actions, bool horizontal, bool vertical)
    : CellAction(actions,
                 horizontal ? "mergeCellsHorizontal" : (vertical ? "mergeCellsVertical" : "mergeCells"),
                 horizontal ? i18n("Merge Cells Horizontally") : (vertical ? i18n("Merge Cells Vertically") : i18n("Merge Cells")),
                 horizontal ? koIcon("mergecell-horizontal") : (vertical ? koIcon("mergecell-vertical") : koIcon("mergecell")),
                 QString())
    , m_horizontal(horizontal)
    , m_vertical(vertical)
{
}

Merge::~Merge() = default;

QAction *Merge::createAction()
{
    QAction *res = CellAction::createAction();
    QString tooltip;
    if (m_horizontal)
        tooltip = i18n("Merge the selected region horizontally");
    else if (m_vertical)
        tooltip = i18n("Merge the selected region vertically");
    else
        tooltip = i18n("Merge the selected region");
    res->setToolTip(tooltip);
    return res;
}

void Merge::execute(Selection *selection, Sheet *sheet, QWidget *)
{
    MergeCommand *const command = new MergeCommand();
    command->setSheet(sheet);
    command->setHorizontalMerge(m_horizontal);
    command->setVerticalMerge(m_vertical);
    command->setSelection(selection);
    command->add(*selection);
    command->execute(selection->canvas());
}

bool Merge::enabledForSelection(Selection *selection, const Cell &)
{
    if (selection->isSingular())
        return false;
    if (selection->isColumnSelected())
        return false;
    if (selection->isRowSelected())
        return false;
    return true;
}

RemoveMerge::RemoveMerge(Actions *actions)
    : CellAction(actions, "dissociateCells", i18n("Dissociate Cells"), koIcon("dissociatecell"), i18n("Unmerge the selected region"))
{
}

RemoveMerge::~RemoveMerge() = default;

void RemoveMerge::execute(Selection *selection, Sheet *sheet, QWidget *)
{
    MergeCommand *const command = new MergeCommand();
    command->setSheet(sheet);
    command->setMerge(false);
    command->setSelection(selection);
    command->add(*selection);
    command->execute(selection->canvas());
}
