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

#ifndef __VHISTORYDOCKER_H__
#define __VHISTORYDOCKER_H__

#include <qlistbox.h>

#include "vcommand.h"
#include "vdocker.h"
#include "karbon_view.h"

class VHistoryItem : public QListBoxItem
{
	public:
		VHistoryItem( QListBox* parent, VCommand* command );
		~VHistoryItem() {}

		VCommand* command() { return m_command; }

		virtual int height( const QListBox* ) const { return 22; }
		virtual int width( const QListBox* lb ) const;

	protected:
		virtual void paint( QPainter* p );

	private:
		VCommand* m_command;
}; // VHistoryItem

class VHistoryDocker : public VDocker
{
	Q_OBJECT

	public:
		VHistoryDocker( KarbonView* view );
		~VHistoryDocker() {}

	public slots:
		void historyCleared();
		void commandExecuted( VCommand* command );
		void commandAdded( VCommand* command );
		void removeFirstCommand();
		void removeLastCommand();

		void commandClicked( int button, QListBoxItem* item, const QPoint& );

	signals:
		void undoCommand( VCommand* command );
		void redoCommand( VCommand* command );
		void undoCommandsTo( VCommand* command );
		void redoCommandsTo( VCommand* command );

	private:
		KarbonView*    m_view;
		QListBox*      m_history;
}; // VHistoryDocker


#endif /* __VHISTORYDOCKER_H__ */

