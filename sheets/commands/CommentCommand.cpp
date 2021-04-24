/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005, 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "CommentCommand.h"


#include <KLocalizedString>

#include "CellStorage.h"
#include "Sheet.h"
#include "RectStorage.h"

using namespace Calligra::Sheets;

CommentCommand::CommentCommand(KUndo2Command* parent)
        : AbstractRegionCommand(parent)
{
}

bool CommentCommand::process(Element* element)
{
    if (!m_reverse) {
        // create undo
        if (m_firstrun)
            m_undoData += m_sheet->commentStorage()->undoData(Region(element->rect()));
        m_sheet->cellStorage()->setComment(Region(element->rect()), m_comment);
    }
    return true;
}

bool CommentCommand::mainProcessing()
{
    if (m_reverse) {
        m_sheet->cellStorage()->setComment(*this, QString());
        for (int i = 0; i < m_undoData.count(); ++i)
            m_sheet->cellStorage()->setComment(Region(m_undoData[i].first.toRect()), m_undoData[i].second);
    }
    return AbstractRegionCommand::mainProcessing();
}

void CommentCommand::setComment(const QString& comment)
{
    m_comment = comment;
    if (m_comment.isEmpty())
        setText(kundo2_i18n("Remove Comment"));
    else
        setText(kundo2_i18n("Add Comment"));
}
