/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers

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
 * Boston, MA 02110-1301, USA.
*/


#include "kaction.h"
#include "klocale.h"

#include "vcommand.h"
#include "karbon_part.h"


VCommandHistory::VCommandHistory( KarbonPart* part )
	: m_part( part ), m_undoLimit( 50 ), m_redoLimit( 30 ), m_savedPos( 0 )
{
	m_commands.setAutoDelete( true );

	m_undo = KStdAction::undo( this, SLOT( undo() ), m_part->actionCollection(), "koffice_undo" );
	m_redo = KStdAction::redo( this, SLOT( redo() ), m_part->actionCollection(), "koffice_redo" );

	clear();
}

VCommandHistory::~VCommandHistory()
{
}

void
VCommandHistory::clear()
{
	if( m_savedPos != int( m_commands.count() - 1 ) )
		m_savedPos = -1;
	else
		m_savedPos = 0;

	m_commands.clear();

	emit historyCleared();

	if( m_undo != 0 )
	{
		m_undo->setEnabled( false );
		m_undo->setText( i18n( "&Undo" ) );
	}

	if( m_redo != 0 )
	{
		m_redo->setEnabled( false );
		m_redo->setText( i18n( "&Redo" ) );
	}
}

void
VCommandHistory::addCommand( VCommand* command, bool execute )
{
	if( command == 0L )
		return;

	if( !m_commands.isEmpty() )
	{
		while( m_commands.last() && !m_commands.last()->success() )
		{
			m_commands.removeLast();
			emit lastCommandRemoved();
		}
	}

	m_commands.append( command );
	kdDebug(38000) << "History: new command: " << m_commands.findRef( command ) << endl;

	if( execute )
	{
		command->execute();
		emit commandExecuted( command );
	}

	updateActions();

	emit commandAdded( command );
}

void
VCommandHistory::setUndoLimit( unsigned int limit )
{
	m_undoLimit = limit;
	clipCommands();
}

void
VCommandHistory::setRedoLimit( unsigned int limit )
{
	m_redoLimit = limit;
	clipCommands();
}

void
VCommandHistory::undo()
{
	int i = m_commands.count() - 1;

	if( i == -1 )
		return;

	while( ( i >= 0 ) && !( m_commands.at( i )->success() ) )
	{
		i--;
	}

	if( i < 0 )
		return;

	VCommand* cmd = m_commands.at( i );

	cmd->unexecute();

	emit commandExecuted( cmd );

	emit commandExecuted();

	clipCommands();

	updateActions();

	m_part->repaintAllViews();
}

void
VCommandHistory::redo()
{
	int i = m_commands.count() - 1;

	if( i == -1 )
		return;

	while( ( i >= 0 ) && !( m_commands.at( i )->success() ) )
	{
		i--;
	}

	i++;

	if( i >= int( m_commands.count() ) )
		return;

	VCommand* cmd;

	if( ( cmd = m_commands.at( i ) ) == 0L )
		return;

	cmd->execute();

	emit commandExecuted( cmd );
	emit commandExecuted();

	updateActions();

	m_part->repaintAllViews();
}

void
VCommandHistory::undo( VCommand* command )
{
	if( ( m_commands.findRef( command ) == -1 ) || ( !command->success() ) )
		return;

	command->unexecute();

	emit commandExecuted( command );
	emit commandExecuted();

	updateActions();

	m_part->repaintAllViews();
}

void
VCommandHistory::redo( VCommand* command )
{
	if( ( m_commands.findRef( command ) == -1 ) || ( command->success() ) )
		return;

	command->execute();

	emit commandExecuted( command );
	emit commandExecuted();

	updateActions();

	m_part->repaintAllViews();
}

void
VCommandHistory::undoAllTo( VCommand* command )
{
	int to;

	if( ( to = m_commands.findRef( command ) ) == -1 )
		return;

	int i = m_commands.count() - 1;

	VCommand* cmd;

	while( i > to )
	{
		cmd = m_commands.at( i-- );

		if( cmd->success() )
		{
			cmd->unexecute();
			emit commandExecuted( cmd );
		}
	}

	emit commandExecuted();
	updateActions();

	m_part->repaintAllViews();
}

void
VCommandHistory::redoAllTo( VCommand* command )
{
	int to;

	if( ( to = m_commands.findRef( command ) ) == -1 )
		return;

	int i = 0;

	VCommand* cmd;

	while( i <= to )
	{
		cmd = m_commands.at( i++ );

		if( !cmd->success() )
		{
			cmd->execute();
			emit commandExecuted( cmd );
		}
	}

	emit commandExecuted();
	updateActions();

	m_part->repaintAllViews();
}

void
VCommandHistory::documentSaved()
{
	// I don't know how to make this work... This is a temporary hack...
	// Maybe remove all undone commands before the current one ?
	int i = m_commands.count() - 1;

	while( ( i >= 0 ) && !( m_commands.at( i )->success() ) )
	{
		i--;
	}

	i++;

	m_savedPos = i;
}

void
VCommandHistory::clipCommands()
{
	while( m_commands.count() > m_undoLimit )
	{
		if( m_commands.removeFirst() )
			m_savedPos--, emit firstCommandRemoved();
	}

	int i = 0;

	int c = m_commands.count();

	while( ( i < c ) && ( !m_commands.at( c - 1 - i )->success() ) )
	{
		i++;
	}

	i = i - m_redoLimit;

	for( int j = 0; j < i; j++ )
	{
		if( m_commands.removeLast() )
			emit lastCommandRemoved();
	}
}

void
VCommandHistory::updateActions()
{
	if( m_commands.count() == 0 )
	{
		if( m_undo != 0 )
		{
			m_undo->setEnabled( false );
			m_undo->setText( i18n( "&Undo" ) );
		}

		if( m_redo != 0 )
		{
			m_redo->setEnabled( false );
			m_redo->setText( i18n( "&Redo" ) );
		}

		return;
	}

	int i = m_commands.count() - 1;

	while( ( i >= 0 ) && !( m_commands.at( i )->success() ) )
	{
		i--;
	}

	if( m_undo != 0 )
	{
		if( i < 0 )
		{
			m_undo->setEnabled( false );
			m_undo->setText( i18n( "&Undo" ) );
		}
		else
		{
			m_undo->setEnabled( true );
			m_undo->setText( i18n( "&Undo: " ) + m_commands.at( i )->name() );
		}
	}

	if( m_redo != 0 )
	{
		if( ++i == int( m_commands.count() ) )
		{
			m_redo->setEnabled( false );
			m_redo->setText( i18n( "&Redo" ) );
		}
		else
		{
			m_redo->setEnabled( true );
			m_redo->setText( i18n( "&Redo: " ) + m_commands.at( i )->name() );
		}
	}

	if( m_savedPos >= 0 )
	{
		for( i = 0; i < m_savedPos; i++ )
		{
			if( !m_commands.at( i )->success() )
				return;
		}

		for( i = m_savedPos; i < int( m_commands.count() ); i++ )
		{
			if( m_commands.at( i )->success() )
				return;
		}

		emit documentRestored();
	}
}

#include "vcommand.moc"

