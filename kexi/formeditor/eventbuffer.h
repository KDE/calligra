/* This file is part of the KDE project
   Copyright (C) 2003   Lucijan Busch <lucijan@gmx.at>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef EVENTBUFFER_H
#define EVENTBUFFER_H

#include <qobject.h>
#include <qptrlist.h>
#include "eventeditor.h"

class KFORMEDITOR_EXPORT EventBufferItem
{
	public:
		EventBufferItem(const QString &sender, const QString &receiver,
		 const QString &signal, const QString &slot, bool fake=false, QObject *osender=0);
		~EventBufferItem();

		QString	sender() const { return m_sender; }
		QString	receiver() const { return m_receiver; }
		QString	signal() const { return m_signal; }
		QString	slot() const { return m_slot; }
		bool	fake() { return m_fake; }
		QObject	*osender() { return m_osender; }

		void	setSender(const QString &v) { m_sender = v; }
		void	setReceiver(const QString &v) { m_receiver = v; }
		void	setSignal(const QString &v) { m_signal = v; }
		void	setSlot(const QString &v) { m_slot = v; }
		void	setOSender(QObject *v) { m_osender = v; }

	protected:
		QObject *m_osender;

		QString m_sender;
		QString m_receiver;
		QString m_signal;
		QString m_slot;

		bool	m_fake;
};


namespace KFormEditor
{
	typedef QPtrList<EventBufferItem> EventBufferBase;

	class KFORMEDITOR_EXPORT EventBuffer
	{
		public:
			EventBuffer();
			~EventBuffer();

			void		insertEvent(EventBufferItem *);
			void		takeEvent(EventBufferItem *);
			EventBuffer	sender(const QString &);
			EventBuffer	sender(QObject *);
			EventBufferItem	*sender(QObject *o, const QString &signal);

			//some std functions to access ptrlist
			EventBufferItem	*first() { return m_eb.first(); }
			EventBufferItem	*next() { return m_eb.next(); }

			void		appendFake(const QString &name, FakeHandler *);

		private:
			EventBufferBase	m_eb;
			Fakes		m_fakes;
	};
};

#endif
