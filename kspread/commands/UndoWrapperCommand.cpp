/* This file is part of the KDE project
   Copyright 2004 Ariya Hidayat <ariya@kde.org>
   Copyright 2004 Laurent Montel <montel@kde.org>

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

#include "Undo.h"

#include "UndoWrapperCommand.h"

using namespace KSpread;

// ----- UndoWrapperCommand -----

UndoWrapperCommand::UndoWrapperCommand( UndoAction* ua )
{
  undoAction = ua;
}

void UndoWrapperCommand::redo()
{
  // This is not really safe and functional, but UndoWrapperCommand
  // is a workaround anyway. So leave it as it it (Ariya)
  undoAction->redo();
}

void UndoWrapperCommand::undo()
{
  undoAction->undo();
}

QString UndoWrapperCommand::name() const
{
  return undoAction->getName();
}
