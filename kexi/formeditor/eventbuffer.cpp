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

#include <kdebug.h>

#include "eventbuffer.h"

EventBufferItem::EventBufferItem(const QString &sender, const QString &receiver,
 const QString &signal, const QString &slot, bool fake, QObject *osender)
{
	m_sender = sender;
	m_receiver = receiver;
	m_signal = signal;
	m_slot = slot;
	m_fake = fake;
	m_osender = osender;
}

EventBufferItem::~EventBufferItem()
{
}


//Eventbuffer it self!

namespace KFormEditor
{
	EventBuffer::EventBuffer()
	{
	}

	EventBuffer::~EventBuffer()
	{
	}

	void
	EventBuffer::insertEvent(EventBufferItem *e)
	{
		kdDebug() << "EventBuffer::insertEvent() e=" << e << endl;
//		insert(e);
		kdDebug() << "EventBuffer::insertEvent() i am: " << this << endl;
		kdDebug() << "EventBuffer::insertEvent() bufferbase is: " << &m_eb << endl;
		m_eb.append(e);
		kdDebug() << "EventBuffer::insertEvent() peh, updated!" << endl;
	}

	void
	EventBuffer::takeEvent(EventBufferItem *e)
	{
		m_eb.remove(e);
	}

	EventBuffer
	EventBuffer::sender(const QString &)
	{
		return EventBuffer();
	}

	EventBuffer
	EventBuffer::sender(QObject *o)
	{
		EventBuffer eb;

		EventBufferItem *it;
		for(it = first(); it; it = next())
		{
			if(it->osender() == o)
			{
				eb.insertEvent(it);
			}
		}

		return eb;
	}

	EventBufferItem*
	EventBuffer::sender(QObject *o, const QString &signal)
	{
		kdDebug() << "EventBuffer::sender() o=" << o << endl;

		EventBufferItem *it;
		for(it = m_eb.first(); it; it = m_eb.next())
		{
			kdDebug() << "EventBuffer::sender(): sender: " << it->sender()
			 << " " << it->signal() << " == " << it->osender() << endl;
			if(it->osender() == o && it->signal() == signal)
			{
				kdDebug() << "EventBuffer::sender(): matching" << endl;
				return it;
			}
		}

		return 0;
	}

	void
	EventBuffer::appendFake(const QString &name, FakeHandler *f)
	{
		m_fakes.insert(name, f);

		EventBufferItem *it;
		for(it = m_eb.first(); it; it = m_eb.next())
		{
			if(it->receiver() == name)
				f->callConnect(it->osender(), it->signal().latin1(), it->slot());
		}
	}
};
