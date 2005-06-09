/***************************************************************************
 * event.cpp
 * This file is part of the KDE project
 * copyright (C)2004-2005 by Sebastian Sauer (mail@dipe.org)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * You should have received a copy of the GNU Library General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 ***************************************************************************/

#include "event.h"
//#include "object.h"
//#include "variant.h"
//#include "eventmanager.h"
#include "../main/scriptcontainer.h"

//#include <qobject.h>
//#include <qsignal.h>
//#include <kaction.h>
//#include <qglobal.h>
//#include <qobjectdefs.h>
//#include <qmetaobject.h>

using namespace Kross::Api;

Event::Event(const QString& name)
    : QObject()
    , Kross::Api::Class<Event>(name)
{
}

Event::~Event()
{
}

/*
bool Event::attach(Event::Ptr event)
{
    if(m_events.contains(event))
        return false;
    m_events.append(event);
    connect(this, SIGNAL(executed(Event::Ptr)), event, SLOT(execute(Event::Ptr)));
    //connect(event, SIGNAL(executeResult(Object*)), this, SIGNAL(executeResult(Object*)));
    return true;
}

bool Event::detach(Event::Ptr event)
{
    if(! m_events.contains(event))
        return false;
    m_events.remove(event);
    disconnect(this, SIGNAL(executed(Event::Ptr)), event, SLOT(execute(Event::Ptr)));
    return true;
}

void Event::execute(Event::Ptr event)
{
    emit executed(event);
}
*/
