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

class Connection::Private
{
public:
    Private(const QString &sender_, const QString &signal_, const QString &receiver_, const QString &slot_);
    Private()
    {

    }

    ~Private()
    {

    }

    QString sender;
    QString signal;
    QString receiver;
    QString slot;
};

Connection::Private::Private(const QString &sender_, const QString &signal_, const QString &receiver_, const QString &slot_)
    :sender(sender_), signal(signal_), receiver(receiver_), slot(slot_)

{

}

Connection::Connection(const QString &sender, const QString &signal,
                       const QString &receiver, const QString &slot) : d(new Private(sender, signal, receiver, slot))
{

}

Connection::Connection() : d(new Private())
{

}

Connection::~Connection()
{
    delete d;
}

QString Connection::sender() const
{
    return d->sender;
}

QString Connection::receiver() const
{
    return d->receiver;
}

QString Connection::signal() const
{
    return d->signal;
}

QString Connection::slot() const
{
    return d->slot;
}

void Connection::setSender(const QString &v)
{
    d->sender = v;
}

void Connection::setReceiver(const QString &v)
{
    d->receiver = v;
}

void Connection::setSignal(const QString &v)
{
    d->signal = v;
}

void Connection::setSlot(const QString &v)
{
    d->slot = v;
}
///////////////////////////////////////

ConnectionBuffer::ConnectionBuffer()
{

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
