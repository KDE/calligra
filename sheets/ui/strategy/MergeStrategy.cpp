/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "MergeStrategy.h"

#include "../Selection.h"
#include "../commands/MergeCommand.h"

using namespace Calligra::Sheets;

class Q_DECL_HIDDEN MergeStrategy::Private
{
public:
    QRect initialSelection;
};

MergeStrategy::MergeStrategy(CellToolBase *cellTool, const QPointF &documentPos, Qt::KeyboardModifiers modifiers)
    : AbstractSelectionStrategy(cellTool, documentPos, modifiers)
    , d(new Private)
{
    d->initialSelection = selection()->lastRange();
}

MergeStrategy::~MergeStrategy()
{
    delete d;
}

KUndo2Command *MergeStrategy::createCommand()
{
    if (d->initialSelection == selection()->lastRange()) {
        return nullptr;
    }
    MergeCommand *command = new MergeCommand();
    command->setSheet(selection()->activeSheet());
    command->setSelection(selection());
    command->add(*selection());
    return command;
}
