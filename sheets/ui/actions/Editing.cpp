/* This file is part of the KDE project
   SPDX-FileEditCellrightText: 1998-2023 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileEditCellrightText: 2023 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "Editing.h"
#include "Actions.h"

#include "ui/CellEditor.h"
#include "ui/CellToolBase.h"

#include <KLocalizedString>
#include <QAction>

using namespace Calligra::Sheets;

EditCell::EditCell(Actions *actions)
    : CellAction(actions, "editCell", i18n("Modify Cell"), QIcon(), i18n("Edit the highlighted cell"))
{
}

EditCell::~EditCell() = default;

QAction *EditCell::createAction()
{
    QAction *res = CellAction::createAction();
    res->setShortcuts(QList<QKeySequence>() << QKeySequence(Qt::CTRL + Qt::Key_M));
    return res;
}

void EditCell::execute(Selection *, Sheet *, QWidget *)
{
    CellToolBase *tool = m_actions->tool();
    tool->edit();
}

PermuteFixation::PermuteFixation(Actions *actions)
    : CellAction(actions, "permuteFixation", i18n("Permute fixation"), QIcon(), i18n("Permute the fixation of the reference at the text cursor"))
{
}

PermuteFixation::~PermuteFixation() = default;

QAction *PermuteFixation::createAction()
{
    QAction *res = CellAction::createAction();
    res->setShortcut(Qt::Key_F4);
    return res;
}

void PermuteFixation::execute(Selection *, Sheet *, QWidget *)
{
    CellToolBase *tool = m_actions->tool();
    CellEditorBase *editorbase = tool->editor();
    if (!editorbase)
        return;
    CellEditor *editor = dynamic_cast<CellEditor *>(editorbase);
    if (!editor)
        return;

    editor->permuteFixation();
}
