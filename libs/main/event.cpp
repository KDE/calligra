/* This file is part of the KDE project
   Copyright (C) 1999 Simon Hausmann <hausmann@kde.org>
             (C) 1999 David Faure <faure@kde.org>

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
   Boston, MA 02110-1301, USA.
*/
#include "event.h"

using namespace KoParts;

//the answer!
#define KPARTS_EVENT_MAGIC 42

class KoParts::EventPrivate
{
public:
    EventPrivate( const char *eventName ) :
        m_eventName(eventName)
    {
    }
    const char* m_eventName;
};

Event::Event( const char *eventName )
 : QEvent( (QEvent::Type)(QEvent::User + KPARTS_EVENT_MAGIC) )
 , d( new EventPrivate(eventName) )
{
}

Event::~Event()
{
    delete d;
}

const char *Event::eventName() const
{
  return d->m_eventName;
}

bool Event::test( const QEvent *event )
{
  if ( !event )
    return false;

  return ( event->type() == (QEvent::Type)(QEvent::User + KPARTS_EVENT_MAGIC ) );
}

bool Event::test( const QEvent *event, const char *name )
{
  if ( !test( event ) )
    return false;

  return ( strcmp( name, ((Event*)event)->eventName() ) == 0 );
}


/////// GUIActivateEvent ////////

class KoParts::GUIActivateEventPrivate
{
public:
    GUIActivateEventPrivate( bool activated )
        : m_bActivated( activated )
    {
    }
    static const char *s_strGUIActivateEvent;
    bool m_bActivated;
};

const char *GUIActivateEventPrivate::s_strGUIActivateEvent = "KoParts/GUIActivate";

GUIActivateEvent::GUIActivateEvent( bool activated ) :
    Event( GUIActivateEventPrivate::s_strGUIActivateEvent ),
    d( new GUIActivateEventPrivate(activated) )
{
}

GUIActivateEvent::~GUIActivateEvent()
{
    delete d;
}

bool GUIActivateEvent::activated() const
{
    return d->m_bActivated;
}

bool GUIActivateEvent::test( const QEvent *event )
{
    return Event::test( event, GUIActivateEventPrivate::s_strGUIActivateEvent );
}
