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

#include <QDomDocument>
#include <kdebug.h>

#include "events.h"

using namespace KFormDesigner;

Connection::Connection(const QString &sender, const QString &signal,
                       const QString &receiver, const QString &slot)
{
    m_sender = sender;
    m_signal = signal;
    m_receiver = receiver;
    m_slot = slot;
}

///////////////////////////////////////

ConnectionBuffer::ConnectionBuffer()
{
//Qt4    setAutoDelete(true);
}

void
ConnectionBuffer::fixName(const QString &oldName, const QString &newName)
{
    foreach (Connection *c, *this) {
        if (c->sender() == oldName)
            c->setSender(newName);
        if (c->receiver() == oldName)
            c->setReceiver(newName);
    }
}

ConnectionBuffer*
ConnectionBuffer::allConnectionsForWidget(const QString &widget)
{
    ConnectionBuffer *list = new ConnectionBuffer();
//Qt4    list->setAutoDelete(false); // or it will delete all our connections
    foreach (Connection *c, *this) {
        if ((c->sender() == widget) || (c->receiver() == widget))
            list->append(c);
    }

    return list;
}

void
ConnectionBuffer::save(QDomNode &parentNode)
{
    if (isEmpty())
        return;

    QDomDocument domDoc = parentNode.ownerDocument();
    QDomElement connections;
    if (!parentNode.firstChildElement("connections").isNull())
        connections = parentNode.firstChildElement("connections");
    else
        connections = domDoc.createElement("connections");
    parentNode.appendChild(connections);

    foreach (Connection *c, *this) {
        QDomElement connection = domDoc.createElement("connection");
        connection.setAttribute("language", "C++");
        connections.appendChild(connection);

        QDomElement sender = domDoc.createElement("sender");
        connection.appendChild(sender);
        QDomText senderText = domDoc.createTextNode(c->sender());
        sender.appendChild(senderText);

        QDomElement signal = domDoc.createElement("signal");
        connection.appendChild(signal);
        QDomText signalText = domDoc.createTextNode(c->signal());
        signal.appendChild(signalText);

        QDomElement receiver = domDoc.createElement("receiver");
        connection.appendChild(receiver);
        QDomText receiverText = domDoc.createTextNode(c->receiver());
        receiver.appendChild(receiverText);

        QDomElement slot = domDoc.createElement("slot");
        connection.appendChild(slot);
        QDomText slotText = domDoc.createTextNode(c->slot());
        slot.appendChild(slotText);
    }
}

void
ConnectionBuffer::saveAllConnectionsForWidget(const QString &widget, QDomNode parentNode)
{
    ConnectionBuffer *buff = allConnectionsForWidget(widget);
    buff->save(parentNode);
    delete buff;
}

void
ConnectionBuffer::load(QDomNode node)
{
    for (QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling()) {
        Connection *conn = new Connection();
        conn->setSender(n.firstChildElement("sender").text());
        conn->setSignal(n.firstChildElement("signal").text());
        conn->setReceiver(n.firstChildElement("receiver").text());
        conn->setSlot(n.firstChildElement("slot").text());
        append(conn);
    }
}

void
ConnectionBuffer::removeAllConnectionsForWidget(const QString &widget)
{
    ConnectionList toRemove;
    foreach (Connection *c, *this) {
        if ((c->sender() == widget) || (c->receiver() == widget)) {
            toRemove.append(c);
        }
    }
    foreach (Connection *c, toRemove) {
        removeAll(c);
    }
    qDeleteAll(toRemove);
}

//#include "events.moc"
