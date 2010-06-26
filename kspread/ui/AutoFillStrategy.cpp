/* This file is part of the KDE project
   Copyright 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#include "AutoFillStrategy.h"

#include "commands/AutoFillCommand.h"
#include "Selection.h"

using namespace KSpread;

class AutoFillStrategy::Private
{
public:
    // If we use the lower right corner of the marker to start autofilling, then this
    // rectangle conatins all cells that were already marker when the user started
    // to mark the rectangle which he wants to become autofilled.
    QRect autoFillSource;
};

AutoFillStrategy::AutoFillStrategy(CellToolBase *cellTool,
                                   const QPointF documentPos, Qt::KeyboardModifiers modifiers)
        : AbstractSelectionStrategy(cellTool, documentPos, modifiers)
        , d(new Private)
{
    d->autoFillSource = selection()->lastRange();
}

AutoFillStrategy::~AutoFillStrategy()
{
    delete d;
}

QUndoCommand* AutoFillStrategy::createCommand()
{
    if (d->autoFillSource == selection()->lastRange()) {
        return 0;
    }
    AutoFillCommand* command = new AutoFillCommand();
    command->setSheet(selection()->activeSheet());
    command->setSourceRange(d->autoFillSource);
    command->setTargetRange(selection()->lastRange());
    return command;
}
