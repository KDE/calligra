/***************************************************************************
 * eventmanager.cpp
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

#include "eventmanager.h"

#include "qtobject.h"
#include "../main/scriptcontainer.h"
#include "../main/manager.h"
#include "eventsignal.h"
#include "eventslot.h"

using namespace Kross::Api;

EventManager::EventManager(ScriptContainer* scriptcontainer, QtObject* qtobj)
    : QObject(scriptcontainer) //QObject(qtobj ? qtobj->getObject() : scriptcontainer)
    , m_scriptcontainer(scriptcontainer)
    , m_qtobj(qtobj)
{
    //m_slots.append( new EventSlot(this) );
}

EventManager::~EventManager()
{
}

bool EventManager::connect(QObject *sender, const QCString& signal, const QString& functionname)
{
    QValueList<EventSlot*> slotslist = m_scriptcontainer->getManager()->getEventSlots();
    for(QValueList<EventSlot*>::Iterator it = slotslist.begin(); it != slotslist.end(); ++it) {
        QCString slot = (*it)->getSlot(signal);
        if(! slot.isNull()) {
            return (*it)->connect(this, sender, signal, functionname, slot);
        }
    }
    return false;

/*
    // create the matching EventSlot
    EventSlot* eventslot = new EventSlot(this);
    // and try to connect the signal
    if(! eventslot->connect(sender, signal, functionname)) {
        delete eventslot;
        return false;
    }
    m_slots << eventslot; // remember the EventSlot instance.
    return true;
*/
}

bool EventManager::disconnect(QObject *sender, const QCString& signal, const QString& functionname)
{
    bool ok = false;
    for(QValueList<EventSlot*>::Iterator it = m_slots.begin(); it != m_slots.end(); ++it) {
        if( (QObject*)(*it)->m_sender == sender
            && qstrcmp((*it)->m_signal, signal) == 0
            && (*it)->m_function == functionname )
        {
            //TODO: disconnect correct slot
            if( QObject::disconnect(sender, signal, *it, SLOT(callback())) ) {
                m_slots.remove(it);
                ok = true;
            }
        }
    }
    return ok;
}

