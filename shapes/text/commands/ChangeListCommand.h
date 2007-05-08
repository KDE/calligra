/* This file is part of the KDE project
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
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

#ifndef CHANGELISTCOMMAND
#define CHANGELISTCOMMAND

#include <KoListStyle.h>

#include <QUndoCommand>
#include <QTextBlock>

class ChangeListCommand : public QUndoCommand
{
public:
    /**
     * Change the list property of 'block'.
     * @param block the paragraph to change the list property of
     * @param style indicates which style to use.
     * @param parent the parent undo command for macro functionality
     */
    ChangeListCommand(const QTextBlock &block, KoListStyle::Style style, QUndoCommand *parent = 0);

    /**
     * Change the list property of 'block'.
     * @param block the paragraph to change the list property of
     * @param style the style to apply
     * @param exact if true then the actual style 'style' should be set, if false we possibly  merge with another similar style that is near the block
     * @param parent the parent undo command for macro functionality
     */
    ChangeListCommand(const QTextBlock &block, KoListStyle style, bool exact = true, QUndoCommand *parent = 0);
    ~ChangeListCommand();

    /// redo the command
    virtual void redo();
    /// revert the actions done in redo
    virtual void undo();

private:
    QTextBlock m_block;
    KoListStyle *m_listStyle;
};

#endif
