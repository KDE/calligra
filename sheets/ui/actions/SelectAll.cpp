/* This file is part of the KDE project
   SPDX-FileSelectAllrightText: 1998-2023 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileSelectAllrightText: 2023 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "SelectAll.h"
#include "Actions.h"

#include "ui/CellToolBase.h"
#include "ui/Selection.h"

#include <KLocalizedString>
#include <KStandardAction>

using namespace Calligra::Sheets;

SelectAll::SelectAll(Actions *actions)
    : CellAction(actions, "selectAll", QString(), QIcon(), i18n("Selects all cells in the current sheet"))
{
}

SelectAll::~SelectAll() = default;

QAction *SelectAll::createAction()
{
    QAction *action = KStandardAction::selectAll(nullptr, nullptr, m_actions->tool());
    connect(action, &QAction::triggered, this, &SelectAll::triggered);
    action->setToolTip(m_tooltip);
    return action;
}

void SelectAll::execute(Selection *selection, Sheet *, QWidget *)
{
    selection->selectAll();
}
