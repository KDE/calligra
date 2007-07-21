/*
 * Copyright (C) 2007 Igor Stepin <igor_for_os@stepin.name>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include "UndoStack.h"
#include <QtDebug>
#include <QObject>
#include "UndoCommand.h"
using namespace kcollaborate;

UndoStack::UndoStack( QObject *parent ):
        QUndoStack( parent )
{}

UndoStack::~UndoStack()
{}

void UndoStack::push( QUndoCommand *cmd )
{
    UndoCommand* undoCommand = dynamic_cast<UndoCommand*>( cmd );
//    UndoCommand* undoCommand = qobject_cast<UndoCommand*>(cmd); -- it's not QObject
    if ( undoCommand == NULL ) return;//ignore incorrect commands for now
    qDebug() << "[UndoStack::push] " << undoCommand->text();
    qDebug() << "[UndoStack::push] name:" << undoCommand->commandName();
    qDebug() << "[UndoStack::push] data:" << undoCommand->data();
    QUndoStack::push( cmd );
}

#include "UndoStack.moc"
