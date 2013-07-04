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


/////// PartActivateEvent ////////

class KoParts::PartActivateEventPrivate
{
public:
    PartActivateEventPrivate( bool activated,
                              Part *part,
                              QWidget *widget ) :
        m_bActivated( activated ),
        m_part( part ),
        m_widget( widget )
    {
    }
    static const char *s_strPartActivateEvent;
    bool m_bActivated;
    Part *m_part;
    QWidget *m_widget;
};

const char *PartActivateEventPrivate::s_strPartActivateEvent = "KoParts/PartActivateEvent";

PartActivateEvent::PartActivateEvent( bool activated,
                                      Part *part,
                                      QWidget *widget ) :
    Event( PartActivateEventPrivate::s_strPartActivateEvent ),
    d( new PartActivateEventPrivate(activated,part,widget) )
{
}

PartActivateEvent::~PartActivateEvent()
{
    delete d;
}

bool PartActivateEvent::activated() const
{
    return d->m_bActivated;
}

Part *PartActivateEvent::part() const
{
    return d->m_part;
}

QWidget *PartActivateEvent::widget() const
{
    return d->m_widget;
}

bool PartActivateEvent::test( const QEvent *event )
{
    return Event::test( event, PartActivateEventPrivate::s_strPartActivateEvent );
}


/////// PartSelectEvent ////////

class KoParts::PartSelectEventPrivate
{
public:
    PartSelectEventPrivate( bool selected,
                            Part *part,
                            QWidget *widget ) :
        m_bSelected( selected ),
        m_part( part ),
        m_widget( widget )
    {
    }
    static const char *s_strPartSelectEvent;
    bool m_bSelected;
    Part *m_part;
    QWidget *m_widget;
};

const char *PartSelectEventPrivate::s_strPartSelectEvent =
                            "KoParts/PartSelectEvent";

PartSelectEvent::PartSelectEvent( bool selected,
                                  Part *part,
                                  QWidget *widget ) :
    Event( PartSelectEventPrivate::s_strPartSelectEvent ),
    d( new PartSelectEventPrivate(selected,part,widget) )
{
}

PartSelectEvent::~PartSelectEvent()
{
    delete d;
}

bool PartSelectEvent::selected() const
{
    return d->m_bSelected;
}

Part *PartSelectEvent::part() const
{
    return d->m_part;
}

QWidget *PartSelectEvent::widget() const
{
    return d->m_widget;
}

bool PartSelectEvent::test( const QEvent *event )
{
    return Event::test( event, PartSelectEventPrivate::s_strPartSelectEvent );
}

