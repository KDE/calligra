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

#include "kexievents.h"
//Added by qt3to4:
#include <Q3CString>

Event::Event(QObject *sender, const Q3CString &signal,
  QObject *receiver, const Q3CString &slot)
{
  m_sender = sender;
  m_receiver = receiver;
  m_slot = slot;
  m_signal = signal;
}

Event::Event(QObject *sender, const Q3CString &signal,
  const Q3CString &functionName)
{
  m_sender = sender;
  m_signal = signal;
  m_slot = functionName;
}

void
EventList::addEvent(Event *event)
{
  if(event)
    append(event);
}

void
EventList::addEvent(QObject *sender, const Q3CString &signal, QObject *receiver, const Q3CString &slot)
{
  Event *ev = new Event(sender, signal, receiver, slot);
  append(ev);
}

void
EventList::addEvent(QObject *sender, const Q3CString &signal, const Q3CString &function)
{
  Event *ev = new Event(sender, signal, function);
  append(ev);
}

void
EventList::removeEvent(Event *event)
{
  if(!event)  return;
  remove(event);
  delete event;
}

EventList*
EventList::allEventsForObject(QObject *widget)
{
  if(!widget)  return 0;

  EventList *l = new EventList();
  EventList::ConstIterator endIt = constEnd();
  for(EventList::ConstIterator it = constBegin(); it != endIt; ++it) {
    if( ((*it)->sender() == widget) || ( (*it)->receiver() == widget) )
      l->addEvent(*it);
  }

  return l;
}

void
EventList::removeAllEventsForObject(QObject *widget)
{
  EventList::ConstIterator endIt = constEnd();
  for(EventList::ConstIterator it = constBegin(); it != endIt; ++it) {
    if( ((*it)->sender() == widget) || ( (*it)->receiver() == widget) )
      removeEvent(*it);
  }
}

