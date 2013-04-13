/* This file is part of the KDE project
 *
 * Copyright (C) 2013 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

// Own
#include "AlterFormulaCommand.h"

#include "FormulaShape.h"
#include "FormulaCursor.h"
#include "FormulaDocument.h"
#include <qtmmlwidget/qtmmlwidget.h>


AlterFormulaCommand::AlterFormulaCommand(FormulaCursor *cursor, const QString &newText,
                                         int newIndex, KUndo2Command *parent)
  : KUndo2Command(parent)
  , m_document(cursor->m_document)
  , m_newText(newText)
  , m_newIndex(newIndex)
  , m_currentIndex(0)
  , m_currentText("")
{
    Q_ASSERT(cursor);
    Q_ASSERT(dynamic_cast<MmlTextNode *>(cursor->m_node));
    m_textNode = static_cast<MmlTextNode *>(cursor->m_node);
    setText(i18nc("(qtundo-format)", "Modify formula"));
    m_currentIndex = m_textNode->m_cursorIndex;
    m_currentText = m_textNode->text();
}

AlterFormulaCommand::~AlterFormulaCommand()
{
}

void AlterFormulaCommand::redo()
{
    m_textNode->m_cursorIndex = m_newIndex;
    m_textNode->setText(m_newText);
    
    m_document->layout();
}

void AlterFormulaCommand::undo()
{
    m_textNode->m_cursorIndex = m_currentIndex;
    m_textNode->setText(m_currentText);
    
    m_document->layout();
}
 
