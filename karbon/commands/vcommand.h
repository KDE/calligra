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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __VCOMMAND_H__
#define __VCOMMAND_H__


#include <assert.h>

#include <qobject.h>

#include "karbon_part.h"
#include "vvisitor.h"


class VDocument;


class VCommand : public VVisitor
{
	public:
		VCommand( VDocument* doc, const QString& name, const QString& icon = "14_action" )
			: m_document( doc ), m_name( name ), m_icon( icon )
		{
			assert( doc );
		}
		virtual ~VCommand() {}

		virtual void execute() = 0;
		virtual void unexecute() {}
		virtual bool isExecuted() = 0;

		VDocument* document() const { return m_document; }
		void       setName( const QString& name ) { m_name = name; }
		QString    name() const { return m_name; }
		QString    icon() const { return m_icon; }

	private:
		VDocument* m_document;
		QString    m_name;
		QString    m_icon;
}; // VCommand


class VCommandHistory : public QObject
{
	Q_OBJECT

	public:
		VCommandHistory( KarbonPart* part );
		~VCommandHistory();

		 // command manipulation
		void clear();
		void addCommand( VCommand* command, bool execute = true );
    
		 // limits
		int undoLimit() const { return m_undoLimit; }
		void setUndoLimit( int limit );
		int redoLimit() const { return m_redoLimit; }
		void setRedoLimit( int limit );
 
		const QPtrList<VCommand>* commands() const { return &m_commands; }

	public slots:
		void undo();
		void redo();
		void undo( VCommand* command );
		void redo( VCommand* command );
		void undoAllTo( VCommand* command );
		void redoAllTo( VCommand* command );

	signals:
		void historyCleared();
		void commandExecuted( VCommand* );
		void commandExecuted();
		void commandAdded( VCommand* );
		void firstCommandRemoved();
		void lastCommandRemoved();
		
	private:
			// helpers
		void clipCommands();
		void updateActions();
	
		KarbonPart*          m_part;
		int                  m_undoLimit;
		int                  m_redoLimit;
		KAction*             m_undo;
		KAction*             m_redo;
		QPtrList<VCommand>   m_commands;
}; // VCommandHistory

#endif

