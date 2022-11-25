/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "Comment.h"
#include "Actions.h"
#include "ui/actions/dialogs/CommentDialog.h"

#include <KLocalizedString>

#include "engine/CellBaseStorage.h"
#include "core/Sheet.h"



using namespace Calligra::Sheets;


Comment::Comment(Actions *actions)
    : CellAction(actions, "comment", i18n("Comment..."), koIcon("edit-comment"), i18n("Edit a comment for this cell"))
    , m_dlg(nullptr)
{
    m_closeEditor = true;
}

Comment::~Comment()
{
    if (m_dlg) delete m_dlg;
}


void Comment::execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget)
{
    m_selection = selection;

    m_dlg = new CommentDialog(canvasWidget);
    const QString comment = Cell(sheet, selection->marker()).comment();
    m_dlg->setComment(comment);

    if (m_dlg->exec() == QDialog::Accepted) {
        CommentCommand* command = new CommentCommand();
        command->setSheet(sheet);
        command->setComment(m_dlg->comment());
        command->add(*selection);
        command->execute(selection->canvas());
    }

    delete m_dlg;
    m_dlg = nullptr;
}


ClearComment::ClearComment(Actions *actions)
    : CellAction(actions, "clearComment", i18n("Clear Comment"), koIcon("delete-comment"), i18n("Remove this cell's comment"))
{
}

ClearComment::~ClearComment()
{
}

QAction *ClearComment::createAction() {
    QAction *res = CellAction::createAction();
    res->setIconText(i18n("Remove Comment"));
    return res;
}

// We're just keeping this enabled for everything, no need to check every selection change
void ClearComment::execute(Selection *selection, Sheet *sheet, QWidget *)
{
    CommentCommand* command = new CommentCommand();
    command->setSheet(sheet);
    command->setComment(QString());
    command->add(*selection);
    command->execute(selection->canvas());
}



CommentCommand::CommentCommand(KUndo2Command* parent)
        : AbstractRegionCommand(parent)
{
}

bool CommentCommand::process(Element* element)
{
    m_sheet->cellStorage()->setComment(Region(element->rect()), m_comment);
    return true;
}

void CommentCommand::setComment(const QString& comment)
{
    m_comment = comment;
    if (m_comment.isEmpty())
        setText(kundo2_i18n("Remove Comment"));
    else
        setText(kundo2_i18n("Add Comment"));
}

