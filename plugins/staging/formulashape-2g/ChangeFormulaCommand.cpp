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

#include "ChangeFormulaCommand.h"

#include "FormulaShape.h"
#include "FormulaCursor.h"
#include "FormulaDocument.h"


ChangeFormulaCommand::ChangeFormulaCommand(FormulaCursor *cursor, KUndo2Command *parent)
  : KUndo2Command(parent)
  , m_node(cursor->m_node)
  , m_document(cursor->m_document)
  , m_cursor(cursor)
  , m_isLastCommandRedo(false)
{
    Q_ASSERT(cursor);
    setText(i18nc("(qtundo-format)", "Delete formula element"));
}

ChangeFormulaCommand::~ChangeFormulaCommand()
{
    if (m_isLastCommandRedo) {
        delete m_node;
    }
}

void ChangeFormulaCommand::redo()
{
    m_isLastCommandRedo = true;
    
    //set the cursor to the parent of the node being deleted
    m_cursor->setNode(m_node->parent());
    m_document->deleteNode(m_node);
    
    m_document->layout();
}

void ChangeFormulaCommand::undo()
{
    m_isLastCommandRedo = true;

    m_document->insertSibling(m_node, m_node->parent(), m_node->previousSibling(), m_node->nextSibling());
    //set the cursor to the deleted node
    m_cursor->setNode(m_node);
    
    m_document->layout();
}
