/* This file is part of the KDE project
 * Copyright (C) 2015-2016 MultiRacio Ltd. <multiracio@multiracio.com> (S.Schliszka, F.Novak, P.Rakyta)
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

#ifndef MCTUNDOCLASS_H
#define MCTUNDOCLASS_H

#include "MctChange.h"
#include "MctChangeset.h"
#include "MctDoBase.h"
#include "MctRedoGraph.h"

#include <QList>
/**
 * Do undo operation for all changes of a revision
 *
 * Undo class prepare and apply a change set before added to abstract graph as a new redo revision.
 * It behaves like a hub, gathers uncommited changes, decide the type of each change,
 * collect type depending information, APPLY the change and than add it to the abstract graph (redo).
 */
class MctUndoClass : MctDoBase
{
public:
    /// constructor
    MctUndoClass(KoTextDocument *kotextdoc);
    ~MctUndoClass();

    /**
     * add changeset to redo abstarct graph
     *
     * @param changesetNode
     * @param redoChangeList
     * @return
     */
    MctChangeset * addChangesetToRedo(MctChangeset * changesetNode, QList<MctChange*> * redoChangeList);

    /**
     * prepare changes into changeset object
     *
     * @param changesetNode
     * @param add2Graph
     */
    void undoChangeset(MctChangeset * changesetNode, bool add2Graph = true);

    /**
     * call type related private function (undo the change)
     *
     * @param changeNode individual change which is tested
     * @return properly configured change object
     */
    MctChange* undoChange(MctChange * changeNode);

private:
    /// undo added string change and commit to redo
    MctChange * undoAddedString(MctChange * changeNode);

    /// undo removed string change and commit to redo
    MctChange * undoRemovedString(MctChange * changeNode);

    /// undo moved string change and commit to redo
    MctChange * undoMovedString(MctChange * changeNode);

    /// undo added string (in table) change and commit to redo
    MctChange * undoAddedStringInTable(MctChange * changeNode);

    /// undo removed string (in table) change and commit to redo
    MctChange * undoRemovedStringInTable(MctChange * changeNode);

    /// undo moved string change (in table) and commit to redo
    MctChange * undoMovedStringInTable(MctChange * changeNode);

    /// undo added paragraph break change and commit to redo
    MctChange * undoParagraphBreak(MctChange * changeNode);

    /// undo deleted paragraph break change and commit to redo
    MctChange * undoDelParagraphBreak(MctChange * changeNode);

    /// undo added paragraph break change (in table) and commit to redo
    MctChange * undoParagraphBreakInTable(MctChange * changeNode);

    /// undo deleted paragraph break change (in table) and commit to redo
    MctChange * undoDelParagraphBreakInTable(MctChange * changeNode);

    /// undo style change and commit to redo
    MctChange * undoStyleChange(MctChange * changeNode);

    /// wrapper for different style type changes
    void undoPropsChange(QTextCursor *cursor, ChangeEventList * changeNode);

    /// undo style change (in table) and commit to redo
    MctChange * undoStyleChangeInTable(MctChange * changeNode);

    /// undo added object change and commit to redo
    MctChange * undoAddedTextGraphicObjects(MctChange * changeNode, bool withprops = true);

    /// undo removed object change and commit to redo
    MctChange * undoRemovedTextGraphicObjects(MctChange * changeNode, bool withprops = true);

    /// undo added onbject change (in table) and commit to redo
    MctChange * undoAddedTextGraphicObjectsInTable(MctChange * changeNode, bool withprops = true);

    /// undo removed object change (in table) and commit to redo
    MctChange * undoRemovedTextGraphicObjectsInTable(MctChange * changeNode, bool withprops = true);

    /// undo added text table change and commit to redo
    MctChange * undoAddedTextTable(MctChange * changeNode, bool withprops = true);

    /// undo removed text table change and commit to redo
    MctChange * undoRemovedTextTable(MctChange * changeNode);

    /// undo added text table change (in table) and commit to redo
    MctChange * undoAddedTextTableInTable(MctChange * changeNode, bool withprops = true);

    /// undo removed text table change (in table) and commit to redo
    MctChange * undoRemovedTextTableInTable(MctChange * changeNode);

    /// undo added row change in table and commit to redo
    MctChange * undoAddedRowInTable(MctChange * changeNode);

    /// undo removed row change in table and commit to redo
    MctChange * undoRemovedRowInTable(MctChange * changeNode);

    /// undo added col change in table and commit to redo
    MctChange * undoAddedColInTable(MctChange * changeNode);

    /// undo added col change in table  and commit to redo
    MctChange * undoRemovedColInTable(MctChange * changeNode);
};

#endif // MCTUNDOCLASS_H
