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

#include <koUndo.h>

#include <qstring.h>
#include <qstringlist.h>

KoCommandHistory::KoCommandHistory( int _number, int _maxundoredo )
  : m_current( -1 )
  , m_maxUndoRedo( _maxundoredo )
  , m_numToolbarItems( _number )
{
  m_history.setAutoDelete( true );
}

void KoCommandHistory::addCommand( KoCommand* _command )
{
  if( m_current < ( (int)m_history.count() - 1 ) )
  {
    KoCommandList commands;
    commands.setAutoDelete( false );

    for( int i = 0; i <= m_current; i++ )
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
  {
    m_history.append( _command );
  }

  if( m_history.count() > m_maxUndoRedo )
    m_history.removeFirst();
  else
    m_current++;

  emitSignals();
}

void KoCommandHistory::undo()
{
  if( m_current > -1 )
  {
    m_history.at( m_current )->unexecute();
    m_current--;

    emitSignals();
  }
}

void KoCommandHistory::redo()
{
  if( m_current > -1 )
  {
    if( m_current < ( (int)m_history.count() - 1 ) )
    {
      m_current++;
      m_history.at( m_current )->execute();

      emitSignals();
    }
  }
  else
  {
    if( m_history.count() > 0 )
    {
      m_current++;
      m_history.at( m_current )->execute();

      emitSignals();
    }
  }
}

QString KoCommandHistory::getUndoName()
{
  if( m_current > -1 )
    return m_history.at( m_current )->name();
  else
    return QString();
}

QString KoCommandHistory::getRedoName()
{
  if( m_current > -1 )
  {
    if( m_current < ( (int)m_history.count() - 1 ) )
      return m_history.at( m_current + 1 )->name();
    else
      return QString();
  }
  else
  {
    if( m_history.count() > 0 )
      return m_history.at( 0 )->name();
    else
      return QString();
  }
}

QStringList KoCommandHistory::getUndoList()
{
  unsigned int i;
  int item;
  QString tmp;
  QStringList list;

  for( i = 0; i < m_maxUndoRedo; i++ )
  {
    item = m_current - i;
    if( item >= 0 )
    {
      tmp = m_history.at( item )->name();
      list.append( tmp );
    }
  }

  return list;
}

QStringList KoCommandHistory::getRedoList()
{
  unsigned int i;
  unsigned int item;
  QString tmp;
  QStringList list;

  for( i = 0; i < m_maxUndoRedo; i++ )
  {
    item = m_current + i + 1;
    if( item < m_history.count() )
    {
      tmp = m_history.at( item )->name();
      list.append( tmp );
    }
  }

  return list;
}

void KoCommandHistory::setMaxUndoRedo( int _maxundoredo )
{
  m_maxUndoRedo = _maxundoredo;

  if( m_history.count() > m_maxUndoRedo )
  {
    KoCommandList commands;
    commands.setAutoDelete( false );

    for( uint i = 0; i <= m_maxUndoRedo; i++ )
    {
      commands.insert( i, m_history.at( 0 ) );
      m_history.take( 0 );
    }

    m_history.clear();
    m_history = commands;
    m_history.setAutoDelete( true );  }
}

void KoCommandHistory::setNumToolbarItems( int _number )
{
  m_numToolbarItems = _number;

  emitSignals();
}

void KoCommandHistory::emitSignals()
{
  emit undoRedoChanged( getUndoName(), getRedoName() );
  emit undoRedoChanged( getUndoList(), getRedoList() );
}

#include "koUndo.moc"

