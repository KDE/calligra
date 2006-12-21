/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>

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

#ifndef KFORMDESIGNEREVENTS_H
#define KFORMDESIGNEREVENTS_H

#include <q3ptrlist.h>
#include <qstring.h>

#include <kexi_export.h>

class QDomNode;

namespace KFormDesigner {

class KFORMEDITOR_EXPORT Connection
{
	public:
		Connection(const QString &sender, const QString &signal,
			const QString &receiver, const QString &slot);
		Connection() {;}
		~Connection() {;}

		QString	sender() const { return m_sender; }
		QString	receiver() const { return m_receiver; }
		QString	signal() const { return m_signal; }
		QString	slot() const { return m_slot; }

		void	setSender(const QString &v) { m_sender = v; }
		void	setReceiver(const QString &v) { m_receiver = v; }
		void	setSignal(const QString &v) { m_signal = v; }
		void	setSlot(const QString &v) { m_slot = v; }

	protected:
		QString m_sender;
		QString m_signal;
		QString m_receiver;
		QString m_slot;
};

typedef Q3PtrList<Connection> ConnectionList;

class KFORMEDITOR_EXPORT ConnectionBuffer : public ConnectionList
{
	public:
		ConnectionBuffer();
		~ConnectionBuffer() {;}

		void save(QDomNode &parentNode);
		void load(QDomNode parentNode);

		/*! This function is called when a widget is renamed from \a oldname
		to \a newname. All the Connections for this widget are updated. */
		void    fixName(const QString &oldname, const QString &newName);

		ConnectionBuffer*     allConnectionsForWidget(const QString &widget);
		void     saveAllConnectionsForWidget(const QString &widget, QDomNode parentNode);
		void     removeAllConnectionsForWidget(const QString &widget);
};

}

#endif

