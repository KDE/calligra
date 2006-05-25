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

#ifndef __VCOMMAND_H__
#define __VCOMMAND_H__


//#include <assert.h>

#include <qobject.h>
#include <q3ptrlist.h>

#include "vvisitor.h"

class VDocument;
class KarbonPart;
class KAction;

/**
 * The base class for all karbon commands.
 *
 * It basically defines the common interface that all commands should implement.
 */
class VCommand : public VVisitor
{
public:
	/**
	 * Constructs a new command.
	 *
	 * @param doc the document the command should work on
	 * @param name the name of the command (appears in command history)
	 * @param icon the icon of the command (appears in command history)
	 */
	VCommand( VDocument* doc, const QString& name, const QString& icon = "14_action" )
			: m_document( doc ), m_name( name ), m_icon( icon )
	{
// A crash because of an assert() is not much better than an crash because of a null
// pointer. Allowing null pointers allows the usage of the vitors ascpect of a VCommand.
//		assert( doc );
	}

	/** Destroys the command */
	virtual ~VCommand() {}

	/**
	 * Executes the command.
	 *
	 * All the changes to the document are done here.
	 * All commands have to implement this function.
	 */
	virtual void execute() = 0;

	/**
	 * Unexecutes the command.
	 *
	 * All changes to the document have to be undone here.
	 */
	virtual void unexecute() {}

	/**
	 * Returns if the command changes the actual document selection.
	 *
	 * This flag is checked to determine if the document has to be redrawn.
	 *
	 * @return true if the selection is changed, else false
	 */
	virtual bool changesSelection() const { return false; }

	/**
	 * Returns the name of the command.
	 *
	 * @return the command name
	 */
	QString name() const
	{
		return m_name;
	}

	/**
	 * Sets the name of the command.
	 *
	 * @param name the new command name
	 */ 
	void setName( const QString& name )
	{
		m_name = name;
	}

	/**
	 * Returns the icon of the command.
	 *
	 * @return the command icon
	 */
	QString icon() const
	{
		return m_icon;
	}

	/**
	 * Returns the document the command works on.
	 *
	 * @return the command's document
	 */
	VDocument* document() const
	{
		return m_document;
	}

private:
	VDocument* m_document;

	QString m_name;
	QString m_icon;
};

/**
 * Manages a set of commands.
 *
 * It keeps the commands in a list, commands higher in the list are older
 * than lower commands.
 * All commands in the list can be undone, beginning from the latest command
 * at the end of the list. Undone commands can be redone, beginning at the
 * oldest undone command. That makes it possible to go back and forth to a 
 * specific document state.
 */
class VCommandHistory : public QObject
{
	Q_OBJECT

public:
	/**
	 * Constructs a command history.
	 *
	 * @param part the part the commands are managed for
	 */
	VCommandHistory( KarbonPart* part );
	
	/** Destroys the command history. */
	~VCommandHistory();

	/**
	 * Clears the command history by removing all commands.
	 *
	 * Emits the historyCleared signal
	 */
	void clear();

	/**
	 * Adds a new command to the history.
	 *
	 * @param command the new command to add
	 * @param execute controls if the new command should be executed
	 */
	void addCommand( VCommand* command, bool execute = true );


	// limits
	/**
	 * Returns the actual undo limit.
	 *
	 * @return the undo limit
	 */
	unsigned int undoLimit() const
	{
		return m_undoLimit;
	}

	/**
	 * Sets a new undo limit.
	 *
	 * The undo limit controls how many commands are stored in the history.
	 * If the new limit is lower than the actual history size, the oldest
	 * commands are removed unitl the size matches the undo limit.
	 * 
	 * @param limit the new undo limit
	 */
	void setUndoLimit( unsigned int limit );

	/**
	 * Returns the actual redo limit.
	 *
	 * @return the redo limit
	 */
	unsigned int redoLimit() const
	{
		return m_redoLimit;
	}

	/**
	 * Sets a new redo limit.
	 *
	 * The redo limit controls how many undone commands are stored in history.
	 * If the new limit is lower than the actual number of undone commands,
	 * the newest commands are removed until the number matches the redo limit.
	 *
	 * @param limit the new redo limit
	 */
	void setRedoLimit( unsigned int limit );

	/**
	 * Read only access to the command history list.
	 *
	 * @return pointer to the list of commands
	 */
	const Q3PtrList<VCommand>* commands() const
	{
		return & m_commands;
	}

public slots:
	/** Undoes the last command not already undone. */
	void undo();
	
	/** Redoes the last command not already undone. */
	void redo();
	
	/**
	 * Undoes the specified command.
	 *
	 * @param command the command to undo
	 */
	void undo( VCommand* command );

	/**
	 * Redoes the specified command.
	 *
	 * @param command the command to redo
	 */
	void redo( VCommand* command );
	
	/**
	 * Undoes all command up to the specified command.
	 *
	 * @param command the command up to which all later commands should be undone
	 */
	void undoAllTo( VCommand* command );

	/**
	 * Redoes all command up to the specified command.
	 *
	 * @param command the command up to which all former commands should be redone
	 */
	void redoAllTo( VCommand* command );

	/**
	 * Marks the actual document state as saved.
	 *
	 * The position within the list corresponding to the actual document state is saved.
	 */
	void documentSaved();

signals:
	/** This signal is emitted when the command history gets cleared. */
	void historyCleared();

	/** 
	 * This signal is emitted when a command is executed.
	 *
	 * The executed command is given as the argument.
	 */
	void commandExecuted( VCommand* );

	/** This signal is emitted when a command is executed. */
	void commandExecuted();

	/** 
	 * This signal is emitted when a command is added to the history.
	 *
	 * The added command is given as the argument.
	 */
	void commandAdded( VCommand* );

	/** This signal is emitted when the first (oldest) command is removed. */
	void firstCommandRemoved();

	/** This signal is emitted when the last (latest) command is removed. */
	void lastCommandRemoved();

	/** 
	* This signal is emitted when the actual document state matches the last saved one.
	*
	* Use documentSaved to set the last saved document state.
	*/
	void documentRestored();

private:
	// helpers
	void clipCommands();
	void updateActions();

	KarbonPart *m_part;
	unsigned int m_undoLimit;
	unsigned int m_redoLimit;
	KAction *m_undo;
	KAction *m_redo;
	Q3PtrList<VCommand> m_commands;
	int m_savedPos;
};

#endif

