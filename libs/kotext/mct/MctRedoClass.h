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

class MctRedoClass : public MctDoBase
{
public:
    MctRedoClass(KoTextDocument *kotextdoc);
    ~MctRedoClass();

    MctChangeset * addChangesetToUndo(MctChangeset * changesetNode, QList<MctChange*> * undoChangeList);
    void redoChangeset(MctChangeset * changesetNode, bool add2Graph = true);
    MctChange* redoChange(MctChange * changeNode);

private:
    MctChange * redoAddedString(MctChange * changeNode);
    MctChange * redoRemovedString(MctChange * changeNode);
    MctChange * redoMovedString(MctChange * changeNode);

    MctChange * redoAddedStringInTable(MctChange * changeNode);
    MctChange * redoRemovedStringInTable(MctChange * changeNode);
    MctChange * redoMovedStringInTable(MctChange * changeNode);

    MctChange * redoParagraphBreak(MctChange * changeNode);
    MctChange * redoDelParagraphBreak(MctChange * changeNode);

    MctChange * redoParagraphBreakInTable(MctChange * changeNode);
    MctChange * redoDelParagraphBreakInTable(MctChange * changeNode);

    MctChange * redoStyleChange(MctChange * changeNode);
    void redoPropsChange(QTextCursor *cursor, ChangeEventList * propchanges);
    MctChange * redoStyleChangeInTable(MctChange * changeNode);

    MctChange * redoAddedTextGraphicObjects(MctChange * changeNode, bool withprops = true);
    MctChange * redoRemovedTextGraphicObjects(MctChange * changeNode, bool withprops = true);

    MctChange * redoAddedTextGraphicObjectsInTable(MctChange * changeNode, bool withprops = true);
    MctChange * redoRemovedTextGraphicObjectsInTable(MctChange * changeNode, bool withprops = true);

    MctChange * redoAddedTextTable(MctChange * changeNode, bool withprops = true);
    MctChange * redoRemovedTextTable(MctChange * changeNode);

    MctChange * redoAddedTextTableInTable(MctChange * changeNode, bool withprops = true);
    MctChange * redoRemovedTextTableInTable(MctChange * changeNode);

    MctChange * redoAddedRowInTable(MctChange * changeNode);
    MctChange * redoRemovedRowInTable(MctChange * changeNode);

    MctChange * redoAddedColInTable(MctChange * changeNode);
    MctChange * redoRemovedColInTable(MctChange * changeNode);
};

#endif // MCTREDOCLASS_H
