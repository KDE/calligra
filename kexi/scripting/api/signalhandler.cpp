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

//#include "object.h"
#include "interpreter.h"
#include "qtobject.h"
//#include "script.h"
#include "../main/scriptcontainer.h"
//#include "python/pythonextension.h"
//#include "python/pythonmodule.h"
//#include "../python/pythoninterpreter.h"
//#include "../kjs/kjsinterpreter.h"

using namespace Kross::Api;

SignalHandler::SignalHandler(QtObject* qtobj)
    : QObject(qtobj->m_object)
    , m_qtobj(qtobj)
{
}

SignalHandler::~SignalHandler()
{
}

//void SignalHandler::connect(QObject *sender, const char *signal, const char *slot)
void SignalHandler::connect(QObject *sender, const char *signal, const QString& function)
{
    Connection conn;
    conn.sender = sender;
    conn.signal = signal;
    conn.function = function;
    m_connections << conn;
    connect(conn);
}

bool SignalHandler::disconnect(QObject *sender, const char *signal, const char *slot)
{
    for(QValueList<Connection>::Iterator it = m_connections.begin(); it != m_connections.end(); ++it) {
        Connection conn = *it;
        if((QObject*)conn.sender == sender
           && qstrcmp(conn.signal, signal) == 0
           //&& qstrcmp(conn.slot, slot) == 0
        ) {
            disconnect(conn);
            m_connections.remove(it);
            return true;
        }
    }
    return false;
}

/*
void SignalHandler::connect(const char *signal, QObject *receiver, const char *slot)
{
}

bool SignalHandler::disconnect(const char *signal, QObject *receiver, const char *slot)
{
}
*/

void SignalHandler::connect(const Connection& connection)
{
    if(connection.sender) {

        QString s = "blaaaaaaaaaaaaaaa";
        QObject::connect( (QObject*)connection.sender, connection.signal,
                          this, SLOT(callback()) );
                          //this, connection.slot ); //SLOT(callback()) );
                          //connection.slot
    }

    /*
    if (!object) return;
    if (!connection.sender && !connection.receiver) return;
    if (connection.sender)
        QObject::connect((QObject*)connection.sender, connection.signal, (QObject*)object, connection.slot);
    else
        QObject::connect((QObject*)object, connection.signal, (QObject*)connection.receiver, connection.slot);
    }
    */
}

void SignalHandler::disconnect(const Connection& connection)
{
}

void SignalHandler::callback()
{
    QObject* senderobj = (QObject*)sender();
    if(! senderobj)
        throw RuntimeException("SignalHandler::callback() failed cause sender is not a QObject.");

    for(QValueList<Connection>::Iterator it = m_connections.begin(); it != m_connections.end(); ++it) {
        Connection conn = *it;
        if((QObject*)conn.sender == senderobj)
           //&& qstrcmp(conn.signal, signal) == 0
           //&& qstrcmp(conn.slot, slot) == 0)
        {
            kdDebug() << QString("SignalHandler callback() sender='%1' signal='%2' slot='%3'")
                         .arg(senderobj->name()).arg(conn.signal).arg(conn.function) << endl;

//TODO
//m_qtobj->m_scriptcontainer->execute();

        }
    }

    kdDebug()<<QString("SignalHandler callback() sender='%1'").arg(senderobj->name())<<endl;
}
