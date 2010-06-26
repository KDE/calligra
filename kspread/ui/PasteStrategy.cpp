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

#include "PasteStrategy.h"

#include <QApplication>
#include <QClipboard>

#include "Global.h"
#include "commands/PasteCommand.h"
#include "Selection.h"
#include "Sheet.h"

using namespace KSpread;

class PasteStrategy::Private
{
public:
};

PasteStrategy::PasteStrategy(CellToolBase *cellTool,
                             const QPointF documentPos, Qt::KeyboardModifiers modifiers)
        : SelectionStrategy(cellTool, documentPos, modifiers)
        , d(new Private)
{
}

PasteStrategy::~PasteStrategy()
{
    delete d;
}

QUndoCommand* PasteStrategy::createCommand()
{
    PasteCommand *const command = new PasteCommand();
    command->setSheet(selection()->activeSheet());
    command->add(*selection());
    command->setMimeData(QApplication::clipboard()->mimeData(QClipboard::Selection));
    return command;
}
