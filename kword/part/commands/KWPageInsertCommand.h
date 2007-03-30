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

#ifndef KWPAGEINSERTCOMMAND_H
#define KWPAGEINSERTCOMMAND_H

#include <QUndoCommand>

class KWPage;
class KWPageManager;
class KWDocument;
class KoShapeMoveCommand;

/// The undo / redo command for inserting a new page in a kword document.
class KWPageInsertCommand : public QUndoCommand
{
public:
    explicit KWPageInsertCommand( KWDocument *document, int afterPageNum, QUndoCommand *parent = 0 );
    ~KWPageInsertCommand();

    /// redo the command
    void redo();
    /// revert the actions done in redo
    void undo();

    /// return the page created.  Note that the result is 0 before the first redo()
    KWPage *page() const { return m_page; }

private:
    KWDocument *m_document;
    KWPage *m_page;
    bool m_deletePage;
    int m_afterPageNum;
    KoShapeMoveCommand *m_shapeMoveCommand;
};

#endif
