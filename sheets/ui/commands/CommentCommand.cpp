/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005, 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "CommentCommand.h"


#include "core/CellStorage.h"
#include "core/Sheet.h"

using namespace Calligra::Sheets;

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
