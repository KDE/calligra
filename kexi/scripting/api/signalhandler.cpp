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

/* SignalConnection */

SignalConnection::SignalConnection(SignalHandler* signalhandler)
    : QObject(signalhandler)
    , m_signalhandler(signalhandler)
{
}

void SignalConnection::callback()
{
    QObject* obj = (QObject*)sender();
    if(! obj) //FIXME is it wise to throw exceptions within slots?
        throw RuntimeException("SignalHandler::callback() failed cause sender is not a QObject.");

    kdDebug() << QString("SignalConnection::callback() sender='%1' signal='%2' function='%3'")
                 .arg(obj->name()).arg(signal).arg(function) << endl;

    //TODO parse arguments

    m_signalhandler->m_scriptcontainer->callFunction(function);
}

/* SignalHandler */

SignalHandler::SignalHandler(ScriptContainer* scriptcontainer, QtObject* qtobj)
    : QObject(scriptcontainer) //QObject(qtobj ? qtobj->getObject() : scriptcontainer)
    , m_scriptcontainer(scriptcontainer)
    , m_qtobj(qtobj)
{
}

SignalHandler::~SignalHandler()
{
}

bool SignalHandler::connect(QObject *sender, const char *signal, const QString& functionname)
{
    SignalConnection* conn = new SignalConnection(this);
    conn->senderobj = sender;
    conn->signal = signal;
    conn->function = functionname;
    m_connections << conn;

    return QObject::connect(
        (QObject*)conn->senderobj,
        conn->signal,
        conn,
        SLOT(callback())
    );
}

bool SignalHandler::disconnect(QObject *sender, const char *signal, const QString& functionname)
{
    bool ok = false;
    for(QValueList<SignalConnection*>::Iterator it = m_connections.begin(); it != m_connections.end(); ++it) {
        if( (QObject*)(*it)->senderobj == sender
            && qstrcmp((*it)->signal, signal) == 0
            && (*it)->function == functionname )
        {
            if( QObject::disconnect(sender, signal, *it, SLOT(callback())) ) {
                m_connections.remove(it);
                ok = true;
            }
        }
    }
    return ok;
}

