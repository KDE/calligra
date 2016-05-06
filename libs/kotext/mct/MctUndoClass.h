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

class MctUndoClass : MctDoBase
{
public:
    MctUndoClass(KoTextDocument *kotextdoc);
    ~MctUndoClass();

    MctChangeset * addChangesetToRedo(MctChangeset * changesetNode, QList<MctChange*> * redoChangeList);
    void undoChangeset(MctChangeset * changesetNode, bool add2Graph = true);
    MctChange* undoChange(MctChange * changeNode);

private:
    MctChange * undoAddedString(MctChange * changeNode);
    MctChange * undoRemovedString(MctChange * changeNode);
    MctChange * undoMovedString(MctChange * changeNode);

    MctChange * undoAddedStringInTable(MctChange * changeNode);
    MctChange * undoRemovedStringInTable(MctChange * changeNode);
    MctChange * undoMovedStringInTable(MctChange * changeNode);

    MctChange * undoParagraphBreak(MctChange * changeNode);
    MctChange * undoDelParagraphBreak(MctChange * changeNode);

    MctChange * undoParagraphBreakInTable(MctChange * changeNode);
    MctChange * undoDelParagraphBreakInTable(MctChange * changeNode);

    MctChange * undoStyleChange(MctChange * changeNode);
    void        undoPropsChange(QTextCursor *cursor, ChangeEventList * changeNode);
    MctChange * undoStyleChangeInTable(MctChange * changeNode);

    MctChange * undoAddedTextGraphicObjects(MctChange * changeNode, bool withprops = true);
    MctChange * undoRemovedTextGraphicObjects(MctChange * changeNode, bool withprops = true);

    MctChange * undoAddedTextGraphicObjectsInTable(MctChange * changeNode, bool withprops = true);
    MctChange * undoRemovedTextGraphicObjectsInTable(MctChange * changeNode, bool withprops = true);

    MctChange * undoAddedTextTable(MctChange * changeNode, bool withprops = true);
    MctChange * undoRemovedTextTable(MctChange * changeNode);

    MctChange * undoAddedTextTableInTable(MctChange * changeNode, bool withprops = true);
    MctChange * undoRemovedTextTableInTable(MctChange * changeNode);

    MctChange * undoAddedRowInTable(MctChange * changeNode);
    MctChange * undoRemovedRowInTable(MctChange * changeNode);

    MctChange * undoAddedColInTable(MctChange * changeNode);
    MctChange * undoRemovedColInTable(MctChange * changeNode);
};

#endif // MCTUNDOCLASS_H
