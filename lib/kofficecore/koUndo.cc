/* This file is part of the KDE project
   Copyright (C) 1999 Michael Koch <koch@kde.org>
 
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

#include "koUndo.h"

KoCommandHistory::KoCommandHistory()
  : m_current( -1 )
{
  m_history.setAutoDelete( true );
}

void KoCommandHistory::addCommand( KoCommand* _command )
{
  if( m_current < static_cast<int> ( m_history.count() ) )
  {
    QList<KoCommand> commands;
    commands.setAutoDelete( false );

    for( int i = 0; i < m_current; i++ )
    {
      commands.insert( i, m_history.at( 0 ) );
      m_history.take( 0 );
    }

    commands.append( _command );
    m_history.clear();
    m_history = commands;
    m_history.setAutoDelete( true );
  }
  else
    m_history.append( _command );

  if( m_history.count() > MAX_UNDO_REDO )
    m_history.removeFirst();
  else
    m_current++;

  emit undoRedoChanged( getUndoName(), getRedoName() );
}

void KoCommandHistory::undo()
{
  if( m_current > 0 )
  {
    m_history.at( m_current - 1 )->unexecute();
    m_current--;
    emit undoRedoChanged( getUndoName(), getRedoName() );
  }
}

void KoCommandHistory::redo()
{
  if( m_current < static_cast<int> ( m_history.count() ) &&
      m_current > -1 )
  {
    m_history.at( m_current )->execute();
    m_current++;
    emit undoRedoChanged( getUndoName(), getRedoName() );
  }
}

QString KoCommandHistory::getUndoName()
{
  if( m_current > 0 )
    return m_history.at( m_current - 1 )->name();
  else
    return QString();
}

QString KoCommandHistory::getRedoName()
{
  if( m_current < static_cast<int> ( m_history.count() ) &&
      m_current > -1 )
    return m_history.at( m_current )->name();
  else
    return QString();
}

#include "koUndo.moc"

