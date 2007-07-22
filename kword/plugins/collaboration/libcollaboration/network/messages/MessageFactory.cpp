/*
 * Copyright (C) 2007 Igor Stepin <igor_for_os@stepin.name>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include "MessageFactory.h"
#include <QDomDocument>
#include <QDomElement>
#include <QtDebug>
using namespace kcollaborate;
using namespace kcollaborate::Message;

MessageFactory::MessageFactory( QObject *parent ):
        QObject( parent )
{}

MessageFactory::~MessageFactory()
{}

bool MessageFactory::processRawMessage( const QString &msg )
{
    QDomDocument domDocument;
    QString errorStr;
    int errorLine;
    int errorColumn;
    if ( !domDocument.setContent( msg, true, &errorStr, &errorLine,
                                  &errorColumn ) ) {
        qWarning() << "Parse error in the recieved message at line "
        << errorLine << ", column " << errorColumn << ": " << errorStr;
        return false;
    }

    QDomElement root = domDocument.documentElement();
    if ( root.tagName() != "Messages" ) {
        qWarning() << "Unknown XML format";
        return false;
    } else if ( root.hasAttribute( "version" )
                && root.attribute( "version" ) != "0.1" ) {
        qWarning() << "Unsupported protocol version";
        return false;
    }

    for ( QDomNode n = root.firstChild(); !n.isNull(); n = n.nextSibling() ) {
        if ( n.nodeType() != QDomNode::ElementNode ) {
            qWarning() << "Unexpected node type: " << n.nodeName() << ":" << n.nodeValue();
        } else {
            QDomElement element = n.toElement();
            QString tagName = element.tagName();

            if ( "Hello" == tagName ) {
                Message::Hello msg( element );
                emit messageReceivedHello( msg );
            } else if ( "HelloAnswer" == tagName ) {
                Message::HelloAnswer msg( element );
                emit messageReceivedHelloAnswer( msg );
            } else if ( "Session" == tagName ) {
                //get session id
                if ( !element.hasAttribute( "id" ) ) {
                    qWarning() << "Incorrect 'Session' tag without id attribute";
                }
                QString sessionId = element.attribute( "id" );

                for ( QDomNode n2 = element.firstChild(); !n2.isNull(); n2 = n2.nextSibling() ) {
                    if ( n2.nodeType() != QDomNode::ElementNode ) {
                        //TODO: attr expected?
                        qWarning() << "Unexpected node type: " << n.nodeName() << ":" << n.nodeValue();
                    } else {
                        QDomElement element2 = n2.toElement();
                        QString tagName2 = element2.tagName();

                        if ( "Update" == tagName2 ) {
                            Message::Update msg( sessionId, element2 );
                            emit messageReceivedUpdate( msg );
                        } else if ( "UpdateAnswer" == tagName2 ) {
                            Message::UpdateAnswer msg( sessionId, element2 );
                            emit messageReceivedUpdateAnswer( msg );
                        } else if ( "Closed" == tagName2 ) {
                            emit messageReceivedSessionClosed( sessionId );
                        } else {
                            qWarning() << "Unsupported session message: " << tagName2;
                            continue;
                        }
                    }
                }
            } else {
                qWarning() << "Unsupported message: " << tagName;
                continue;
            }
        }
    }

    return true;
}

#include "MessageFactory.moc"
