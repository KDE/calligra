/*  This file is part of the KDE libraries
    Copyright (C) 2006 Michaël Larouche <michael.larouche@kdemail.net>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; version 2
    of the License.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/
#include "kmessage.h"


#include <QtCore/QLatin1String>

#include <iostream>

class StaticMessageHandler
{
public:
    StaticMessageHandler() : m_handler(0) {}
    ~StaticMessageHandler()
    {
        delete m_handler;
    }
    /* Sets the new message handler and deletes the old one */
    void setHandler(KMessageHandler *handler)
    {
        delete m_handler;
        m_handler = handler;
    }
    KMessageHandler *handler() const
    {
        return m_handler;
    }

protected:
    KMessageHandler *m_handler;
};
Q_GLOBAL_STATIC(StaticMessageHandler, s_messageHandler)

static void internalMessageFallback(KMessage::MessageType messageType, const QString &text, const QString &caption)
{
    QString prefix;
    switch(messageType)
    {
        case KMessage::Error:
             prefix = QLatin1String("ERROR: ");
             break;
        case KMessage::Fatal:
             prefix = QLatin1String("FATAL: ");
             break;
        case KMessage::Information:
             prefix = QLatin1String("INFORMATION: ");
             break;
        case KMessage::Sorry:
             prefix = QLatin1String("SORRY: ");
             break;
        case KMessage::Warning:
             prefix = QLatin1String("WARNING: ");
             break;
    }

    QString message;

    if( !caption.isEmpty() )
        message += QLatin1Char('(') + caption + QLatin1Char(')');

    message += prefix + text;

    // Show a message to the developer to setup a KMessageHandler
    std::cerr << "WARNING: Please setup an KMessageHandler with KMessage::setMessageHandler to display message propertly." << std::endl;
    // Show message to stdout
    std::cerr << qPrintable(message) << std::endl;
}

void KMessage::setMessageHandler(KMessageHandler *handler)
{
    // Delete old message handler.
    s_messageHandler()->setHandler(handler);
}

void KMessage::message(KMessage::MessageType messageType, const QString &text, const QString &caption)
{
    // Use current message handler if available, else use stdout
    if(s_messageHandler()->handler())
    {
        s_messageHandler()->handler()->message(messageType, text, caption);
    }
    else
    {
       internalMessageFallback(messageType, text, caption);
    }
}

// kate: space-indent on; indent-width 4; encoding utf-8; replace-tabs on;
