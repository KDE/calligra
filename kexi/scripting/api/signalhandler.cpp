/***************************************************************************
 * signalhandler.cpp
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

#include "signalhandler.h"

#include "interpreter.h"
#include "qtobject.h"
#include "../main/scriptcontainer.h"

using namespace Kross::Api;

namespace Kross { namespace Api {

    /// Private class to handle Qt signal/slot connections.
    class SignalHandlerConnection
    {
        public:
            QGuardedPtr<QObject> sender;
            //QGuardedPtr<QObject> receiver;
            const char* signal;
            QString function;
    };

}}

SignalHandler::SignalHandler(ScriptContainer* scriptcontainer, QtObject* qtobj)
    : QObject(scriptcontainer) //QObject(qtobj ? qtobj->getObject() : scriptcontainer)
    , m_scriptcontainer(scriptcontainer)
    , m_qtobj(qtobj)
{
}

SignalHandler::~SignalHandler()
{
    for(QValueList<SignalHandlerConnection*>::Iterator it = m_connections.begin(); it != m_connections.end(); ++it)
        delete *it;
}

bool SignalHandler::connect(QObject *sender, const char *signal, const QString& functionname)
{
    SignalHandlerConnection* connection = new SignalHandlerConnection();
    connection->sender = sender;
    connection->signal = signal;
    connection->function = functionname;
    m_connections << connection;
    return connect(connection);
}

bool SignalHandler::disconnect(QObject *sender, const char *signal, const QString& /*functionname*/)
{
    /*
    for(QValueList<Connection>::Iterator it = m_connections.begin(); it != m_connections.end(); ++it) {
        Connection conn = *it;
        if((QObject*)conn.sender == sender
           && qstrcmp(conn.signal, signal) == 0
           //&& qstrcmp(conn.function, functionname) == 0 //TODO
        ) {
            if(! disconnect(conn)) return false;
            m_connections.remove(it);
            return true;
        }
    }
    return false;
    */
    return false;
}

bool SignalHandler::connect(SignalHandlerConnection* connection)
{
    if(! connection || ! connection->sender) {
        kdDebug() << "SignalHandler::connect() failed cause connection is invalid." << endl;
        return false;
    }
    QObject::connect(
        (QObject*)connection->sender,
        connection->signal,
        this,
        SLOT(callback())
    );
    return true;
}

bool SignalHandler::disconnect(SignalHandlerConnection* connection)
{
    //TODO
    return true;
}

void SignalHandler::callback()
{
    QObject* senderobj = (QObject*)sender();
    if(! senderobj)
        throw RuntimeException("SignalHandler::callback() failed cause sender is not a QObject.");
    kdDebug()<<QString("SignalHandler callback() sender='%1'").arg(senderobj->name())<<endl;

    for(QValueList<SignalHandlerConnection*>::Iterator it = m_connections.begin(); it != m_connections.end(); ++it) {

        if(static_cast<QObject*>((*it)->sender) == senderobj
           //&& qstrcmp(conn.signal, signal) == 0
           //&& qstrcmp(conn.slot, slot) == 0) //TODO
        )
        {
            kdDebug() << QString("SignalHandler callback() sender='%1' signal='%2' slot='%3'")
                         .arg(senderobj->name()).arg((*it)->signal).arg((*it)->function) << endl;
            //TODO
            m_scriptcontainer->callFunction((*it)->function);
        }
    }
}
