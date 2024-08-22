/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "Comment.h"
#include "Actions.h"
#include "ui/actions/dialogs/CommentDialog.h"

#include <KLocalizedString>

#include "core/Sheet.h"
#include "engine/CellBaseStorage.h"

using namespace Calligra::Sheets;

Comment::Comment(Actions *actions)
    : DialogCellAction(actions, "comment", i18n("Comment..."), koIcon("edit-comment"), i18n("Edit a comment for this cell"))
{
    m_closeEditor = true;
}

Comment::~Comment() = default;

ActionDialog *Comment::createDialog(QWidget *canvasWidget)
{
    CommentDialog *dlg = new CommentDialog(canvasWidget);
    connect(dlg, &CommentDialog::changeComment, this, &Comment::changeComment);
    return dlg;
}

void Comment::onSelectionChanged()
{
    CommentDialog *dlg = dynamic_cast<CommentDialog *>(m_dlg);
    const QString comment = activeCell().comment();
    dlg->setComment(comment);
}

void Comment::changeComment(const QString &comment)
{
    CommentCommand *command = new CommentCommand();
    command->setSheet(m_selection->activeSheet());
    command->setComment(comment);
    command->add(*m_selection);
    command->execute(m_selection->canvas());
}

ClearComment::ClearComment(Actions *actions)
    : CellAction(actions, "clearComment", i18n("Clear Comment"), koIcon("delete-comment"), i18n("Remove this cell's comment"))
{
}

ClearComment::~ClearComment() = default;

QAction *ClearComment::createAction()
{
    QAction *res = CellAction::createAction();
    res->setIconText(i18n("Remove Comment"));
    return res;
}

// We're just keeping this enabled for everything, no need to check every selection change
void ClearComment::execute(Selection *selection, Sheet *sheet, QWidget *)
{
    CommentCommand *command = new CommentCommand();
    command->setSheet(sheet);
    command->setComment(QString());
    command->add(*selection);
    command->execute(selection->canvas());
}

CommentCommand::CommentCommand(KUndo2Command *parent)
    : AbstractRegionCommand(parent)
{
}

bool CommentCommand::process(Element *element)
{
    m_sheet->cellStorage()->setComment(element->rect().left(), element->rect().top(), m_comment);
    return true;
}

void CommentCommand::setComment(const QString &comment)
{
    m_comment = comment;
    if (m_comment.isEmpty())
        setText(kundo2_i18n("Remove Comment"));
    else
        setText(kundo2_i18n("Add Comment"));
}
