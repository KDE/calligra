/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
                      Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>
                 2006 Martin Pfeiffer <hubipete@gmx.net>
                 2009 Jeremias Epperlein <jeeree@web.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "FormulaCommand.h"
#include "KoFormulaShape.h"

FormulaCommand::FormulaCommand(KoFormulaShape* formulaShape,
                               QString mathML, QFont defaultFont,
                               QColor foregroundColor, QColor backgroundColor,
                               KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_shape(formulaShape)
    , m_newMathML(mathML)
    , m_newDefaultFont(defaultFont)
    , m_newForegroundColor(foregroundColor)
    , m_newBackgroundColor(backgroundColor)
{
    Q_ASSERT(m_shape);
    setText(kundo2_i18n("Update Formula"));

    m_oldMathML = m_shape->content();
    m_oldDefaultFont = m_shape->font();
    m_oldForegroundColor = m_shape->foregroundColor();
    m_oldBackgroundColor = m_shape->backgroundColor();
}


void FormulaCommand::redo() {
    KUndo2Command::redo();

    if (m_oldMathML != m_newMathML)
        m_shape->setContent(m_newMathML);
    if (m_oldDefaultFont != m_newDefaultFont)
        m_shape->setFont(m_newDefaultFont);
    if (m_oldForegroundColor != m_newForegroundColor)
        m_shape->setForegroundColor(m_newForegroundColor);
    if (m_oldBackgroundColor != m_newBackgroundColor)
        m_shape->setBackgroundColor(m_newBackgroundColor);

    m_shape->update();
}

void FormulaCommand::undo() {
    KUndo2Command::undo();

    if (m_oldMathML != m_newMathML)
        m_shape->setContent(m_oldMathML);
    if (m_oldDefaultFont != m_newDefaultFont)
        m_shape->setFont(m_oldDefaultFont);
    if (m_oldForegroundColor != m_newForegroundColor)
        m_shape->setForegroundColor(m_oldForegroundColor);
    if (m_oldBackgroundColor != m_newBackgroundColor)
        m_shape->setBackgroundColor(m_oldBackgroundColor);

    m_shape->update();
}
