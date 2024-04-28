/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "PasteStrategy.h"

#include <QApplication>
#include <QClipboard>

#include "../Selection.h"
#include "../commands/PasteCommand.h"

using namespace Calligra::Sheets;

class Q_DECL_HIDDEN PasteStrategy::Private
{
public:
};

PasteStrategy::PasteStrategy(CellToolBase *cellTool, const QPointF &documentPos, Qt::KeyboardModifiers modifiers)
    : SelectionStrategy(cellTool, documentPos, modifiers)
    , d(new Private)
{
}

PasteStrategy::~PasteStrategy()
{
    delete d;
}

KUndo2Command *PasteStrategy::createCommand()
{
    PasteCommand *const command = new PasteCommand();
    command->setSheet(selection()->activeSheet());
    command->add(*selection());
    QClipboard *clipboard = QApplication::clipboard();
    command->setMimeData(clipboard->mimeData(QClipboard::Selection), clipboard->ownsSelection());
    return command;
}
