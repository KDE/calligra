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

#ifndef MCTREDOCLASS_H
#define MCTREDOCLASS_H

#include "MctChange.h"
#include "MctChangeset.h"
#include "MctDoBase.h"
#include "MctChangeEntities.h"
#include "KoShape.h"

#include <QList>

/**
 * Do redo operation for all changes of a revision
 *
 * Redo class prepare and apply a change set before added to abstract graph as a new undo revision.
 * It behaves like a hub, gathers uncommited changes, decide the type of each change,
 * collect type depending information, APPLY the change and than add it to the abstract graph (undo).
 */
class MctRedoClass : public MctDoBase
{
public:
    /// constructor
    MctRedoClass(KoTextDocument *kotextdoc);
    ~MctRedoClass();

    /**
     * add changeset to undo abstarct graph
     *
     * @param changesetNode
     * @param redoChangeList
     * @return
     */
    MctChangeset * addChangesetToUndo(MctChangeset * changesetNode, QList<MctChange*> * undoChangeList);
    /**
     * prepare changes into changeset object
     *
     * @param changesetNode
     * @param add2Graph
     */
    void redoChangeset(MctChangeset * changesetNode, bool add2Graph = true);

    /**
     * call type related private function (redo the change)
     *
     * @param changeNode individual change which is tested
     * @return properly configured change object
     */
    MctChange* redoChange(MctChange * changeNode);

private:
    /// redo added string change and commit to undo
    MctChange * redoAddedString(MctChange * changeNode);

    /// redo removed string change and commit to undo
    MctChange * redoRemovedString(MctChange * changeNode);

    /// redo moved string change and commit to undo
    MctChange * redoMovedString(MctChange * changeNode);

    /// redo added string (in table) change and commit to undo
    MctChange * redoAddedStringInTable(MctChange * changeNode);

    /// redo removed string (in table) change and commit to undo
    MctChange * redoRemovedStringInTable(MctChange * changeNode);

    /// redo moved string change (in table) and commit to undo
    MctChange * redoMovedStringInTable(MctChange * changeNode);

    /// redo added paragraph break change and commit to undo
    MctChange * redoParagraphBreak(MctChange * changeNode);

    /// redo deleted paragraph break change and commit to undo
    MctChange * redoDelParagraphBreak(MctChange * changeNode);

    /// redo added paragraph break change (in table) and commit to undo
    MctChange * redoParagraphBreakInTable(MctChange * changeNode);

    /// redo deleted paragraph break change (in table) and commit to undo
    MctChange * redoDelParagraphBreakInTable(MctChange * changeNode);

    /// redo style change and commit to undo
    MctChange * redoStyleChange(MctChange * changeNode);

    /// wrapper for different style type changes
    void redoPropsChange(QTextCursor *cursor, ChangeEventList * propchanges);

    /// redo style change (in table) and commit to undo
    MctChange * redoStyleChangeInTable(MctChange * changeNode);

    /// redo added object change and commit to undo
    MctChange * redoAddedTextGraphicObjects(MctChange * changeNode, bool withprops = true);

    /// redo removed object change and commit to undo
    MctChange * redoRemovedTextGraphicObjects(MctChange * changeNode, bool withprops = true);

    /// redo added onbject change (in table) and commit to undo
    MctChange * redoAddedTextGraphicObjectsInTable(MctChange * changeNode, bool withprops = true);

    /// redo removed object change (in table) and commit to undo
    MctChange * redoRemovedTextGraphicObjectsInTable(MctChange * changeNode, bool withprops = true);

    /// redo added text table change and commit to undo
    MctChange * redoAddedTextTable(MctChange * changeNode, bool withprops = true);

    /// redo removed text table change and commit to undo
    MctChange * redoRemovedTextTable(MctChange * changeNode);

    /// redo added text table change (in table) and commit to undo
    MctChange * redoAddedTextTableInTable(MctChange * changeNode, bool withprops = true);

    /// redo removed text table change (in table) and commit to undo
    MctChange * redoRemovedTextTableInTable(MctChange * changeNode);

    /// redo added row change in table and commit to undo
    MctChange * redoAddedRowInTable(MctChange * changeNode);

    /// redo removed row change in table and commit to undo
    MctChange * redoRemovedRowInTable(MctChange * changeNode);

    /// redo added col change in table and commit to undo
    MctChange * redoAddedColInTable(MctChange * changeNode);

    /// redo added col change in table  and commit to undo
    MctChange * redoRemovedColInTable(MctChange * changeNode);
};

#endif // MCTREDOCLASS_H
