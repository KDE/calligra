/*
 * This file is part of the KDE project
 *
 * (C) Copyright 2008 by Lorenzo Villani <lvillani@binaryhelix.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <QUrl>
#include <QString>
#include <QByteArray>
#include <QStringList>

#include <kdebug.h>

//#include <pion/net/PionUser.hpp>
#include <pion/net/HTTPTypes.hpp>
#include <pion/net/WebService.hpp>
#include <pion/net/HTTPResponseWriter.hpp>

//#include "auth/User.h"
//#include "auth/Permission.h"
//#include "auth/Authenticator.h"

#include "../../view/extjs/Objects.h"
#include "../../view/extjs/XMLTable.h"

#include "Controller.h"

namespace KexiWebForms {

Controller::Controller() {
    m_objects = new View::Objects();
    m_xmlTable = new View::XMLTable();
}

Controller::~Controller() {
    delete m_objects;
    delete m_xmlTable;
}

void Controller::operator()(pion::net::HTTPRequestPtr& request, pion::net::TCPConnectionPtr& tcp_conn) {
    pion::net::HTTPResponseWriterPtr writer(pion::net::HTTPResponseWriter::create(tcp_conn, *request,
                                            boost::bind(&pion::net::TCPConnection::finish, tcp_conn)));

    // Authentication data
    /*pion::net::PionUserPtr userPtr;
    Auth::User u;
    if (request->getUser()) {
        userPtr = pion::net::PionUserPtr(request->getUser());
        u = Auth::Authenticator::getInstance()->authenticate(userPtr);
        }*/

    // Request URI handling & dispatch
    QStringList requestURI(QString(request->getOriginalResource().c_str()).split('/'));
    requestURI.removeFirst();

    QString action(requestURI.at(0));
    requestURI.removeFirst();

    QHash<QString, QString> data;

    // Convert all the stuff from hash_multimap and put it in data
    typedef pion::net::HTTPTypes::QueryParams::const_iterator SDIterator;
    pion::net::HTTPTypes::QueryParams params(request->getQueryParams());

    /**
     * some nasty things can happen here, for example someone can overwrite
     * the data hash, this is not good
     */
    for (SDIterator it = params.begin(); it != params.end(); ++it) {
        data[QUrl::fromPercentEncoding(QByteArray(it->first.c_str()))] =
            QUrl::fromPercentEncoding(QByteArray(it->second.c_str()));
    }

    kDebug() << "ACTION :" << action;
    kDebug() << "PARAMETERS COUNT: " << requestURI.count();

    bool malformedRequest = true;
    if (action == "objects") {
        if (requestURI.count() == 0) {
            m_objects->view(data, writer);
            malformedRequest = false;
        }
    } else if (action == "xmltable") {
        if (requestURI.count() == 1) {
            data["uri-table"] = requestURI.at(0);
            m_xmlTable->view(data, writer);
            malformedRequest = false;
        }
    }

    if (malformedRequest)
        writer->writeNoCopy("<h1>Malformed Request</h1>");

    writer->writeNoCopy(pion::net::HTTPTypes::STRING_CRLF);
    writer->writeNoCopy(pion::net::HTTPTypes::STRING_CRLF);
    writer->send();
}

}
