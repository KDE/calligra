/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <commandhistory.h>

/******************************************************************/
/* Class: CommandHistory                                          */
/******************************************************************/

/*====================== constructor =============================*/
CommandHistory::CommandHistory()
{
    commands.setAutoDelete( true );
    present = 0;
}

/*======================= destructor =============================*/
CommandHistory::~CommandHistory()
{
    clear();
}

void CommandHistory::clear()
{
    commands.clear();
}

/*====================== add command =============================*/
void CommandHistory::addCommand( Command *_command )
{
    if ( present < static_cast<int>( commands.count() ) )
    {
        QList<Command> _commands;
        _commands.setAutoDelete( false );

        for ( int i = 0; i < present; i++ )
        {
            _commands.insert( i, commands.at( 0 ) );
            commands.take( 0 );
        }

        _commands.append( _command );
        commands.clear();
        commands = _commands;
        commands.setAutoDelete( true );
    }
    else
        commands.append( _command );

    if ( commands.count() > MAX_UNDO_REDO )
        commands.removeFirst();
    else
        present++;

    emit undoRedoChanged( getUndoName(), getRedoName() );
}

/*======================= undo ===================================*/
void CommandHistory::undo()
{
    if ( present > 0 )
    {
        commands.at( present - 1 )->unexecute();
        present--;
        emit undoRedoChanged( getUndoName(), getRedoName() );
    }
}

/*======================= redo ===================================*/
void CommandHistory::redo()
{
    if ( present < static_cast<int>( commands.count() ) )
    {
        commands.at( present )->execute();
        present++;
        emit undoRedoChanged( getUndoName(), getRedoName() );
    }
}

/*======================== get undo name ========================*/
QString CommandHistory::getUndoName()
{
    if ( present > 0 )
        return commands.at( present - 1 )->getName();
    else
        return QString();
}

/*======================== get redo name ========================*/
QString CommandHistory::getRedoName()
{
    if ( present < static_cast<int>( commands.count() ) )
        return commands.at( present )->getName();
    else
        return QString();
}

#include <commandhistory.moc>
