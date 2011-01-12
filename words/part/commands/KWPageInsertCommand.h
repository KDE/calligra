/* This file is part of the KDE project
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
 * Copyright (C) 2008 Pierre Ducroquet <pinaraf@pinaraf.info>
 * Copyright (C) 2008 Sebastian Sauer <mail@dipe.org>
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

#include "../kword_export.h"
#include "../KWPage.h"

#include <QUndoCommand>

class KWPage;
class KWPageManager;
class KWDocument;
class KoShapeMoveCommand;

/// The undo / redo command for inserting a new page in a kword document.
class KWORD_TEST_EXPORT KWPageInsertCommand : public QUndoCommand
{
public:
    /**
     * The constructor for a command to insert a new page.
     * @param document the document that gets a new page.
     * @param afterPageNum we will insert a new page after the page indicated with pagenumber afterPageNum
     * @param parent the parent for command macros
     * @param masterPageName the master page name for the new page
     */
    explicit KWPageInsertCommand(KWDocument *document, int afterPageNum, const QString &masterPageName = QString(), QUndoCommand *parent = 0);
    ~KWPageInsertCommand();

    /// redo the command
    virtual void redo();
    /// revert the actions done in redo
    virtual void undo();

    /// return the page created.
    KWPage page() const;

private:
    // private class is not exactly needed since this class is not exported.
    // but since we use private API in the cpp file I decided to use a d-pointer
    // to avoid including private headers in a public header.
    class Private;
    Private * const d;
};

#endif
