/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "AutoFillStrategy.h"

#include "../Selection.h"
#include "../commands/AutoFillCommand.h"

using namespace Calligra::Sheets;

class Q_DECL_HIDDEN AutoFillStrategy::Private
{
public:
    // If we use the lower right corner of the marker to start autofilling, then this
    // rectangle contains all cells that were already marker when the user started
    // to mark the rectangle which the user wants to become autofilled.
    QRect autoFillSource;
};

AutoFillStrategy::AutoFillStrategy(CellToolBase *cellTool, const QPointF &documentPos, Qt::KeyboardModifiers modifiers)
    : AbstractSelectionStrategy(cellTool, documentPos, modifiers)
    , d(new Private)
{
    d->autoFillSource = selection()->lastRange();
}

AutoFillStrategy::~AutoFillStrategy()
{
    delete d;
}

KUndo2Command *AutoFillStrategy::createCommand()
{
    if (d->autoFillSource == selection()->lastRange()) {
        return nullptr;
    }
    AutoFillCommand *command = new AutoFillCommand();
    command->setSheet(selection()->activeSheet());
    command->setSourceRange(d->autoFillSource);
    command->setTargetRange(selection()->lastRange());
    return command;
}
