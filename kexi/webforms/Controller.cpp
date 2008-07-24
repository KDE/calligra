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

#include <KDebug>

#include <pion/net/PionUser.hpp>
#include <pion/net/HTTPTypes.hpp>
#include <pion/net/WebService.hpp>
#include <pion/net/HTTPResponseWriter.hpp>

#include "view/Index.h"
#include "view/Create.h"
#include "view/Read.h"
#include "view/Update.h"
#include "view/Delete.h"
#include "view/Query.h"

#include "auth/User.h"
#include "auth/Permission.h"
#include "auth/Authenticator.h"

#include "Controller.h"

namespace KexiWebForms {

    Controller::Controller() {
        m_index = new View::Index(*this, "index.tpl");
        m_create = new View::Create(*this, "create.tpl");
        m_read = new View::Read(*this, "read.tpl");
        m_update = new View::Update(*this, "update.tpl");
        m_delete = new View::Delete(*this, "delete.tpl");
        m_query = new View::Query(*this, "query.tpl");
    }

    Controller::~Controller() {
        delete m_index;
        delete m_create;
        delete m_read;
        delete m_update;
        delete m_delete;
        delete m_query;
    }

    void Controller::operator()(pion::net::HTTPRequestPtr& request, pion::net::TCPConnectionPtr& tcp_conn) {
        pion::net::HTTPResponseWriterPtr writer(pion::net::HTTPResponseWriter::create(tcp_conn, *request,
                                                                                      boost::bind(&pion::net::TCPConnection::finish, tcp_conn)));

        // Authentication data
        pion::net::PionUserPtr userPtr;
        Auth::User u;
        if (request->getUser()) {
            userPtr = pion::net::PionUserPtr(request->getUser());
            u = Auth::Authenticator::getInstance()->authenticate(userPtr);
        }

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
         * someone can overwrite the data hash, this is not good
         */
        for (SDIterator it = params.begin(); it != params.end(); ++it) {
            data[QUrl::fromPercentEncoding(QByteArray(it->first.c_str()))] =
                QUrl::fromPercentEncoding(QByteArray(it->second.c_str()));
        }
        u.can(Auth::READ);
        
        kDebug() << "ACTION :" << action << endl;
        kDebug() << "PARAMETERS COUNT: " << requestURI.count() << endl;

        bool malformedRequest = true;
        if (action == "") {
            if (!requestURI.count() != 0) {
                malformedRequest = false;
                m_index->view(data, writer);
            }
        } else if (action == "create") {
            if (!requestURI.count() != 1 && u.can(Auth::CREATE)) {
                data["uri-table"] = requestURI.at(0);
                m_create->view(data, writer);
                malformedRequest = false;
            }
        } else if (action == "read") {
            if (!requestURI.count() != 1 && u.can(Auth::READ)) {
                data["uri-table"] = requestURI.at(0);
                m_read->view(data, writer);
                malformedRequest = false;
            }
        } else if (action == "update") {
            if (!requestURI.count() != 3 && u.can(Auth::UPDATE)) {
                data["uri-table"] = requestURI.at(0);
                data["uri-pkey"] = requestURI.at(1);
                data["uri-pval"] = requestURI.at(2);
                m_update->view(data, writer);
                malformedRequest = false;
            }
        } else if (action == "delete" && u.can(Auth::DELETE)) {
            if (!requestURI.count() != 3) {
                data["uri-table"] = requestURI.at(0);
                data["uri-pkey"] = requestURI.at(1);
                data["uri-pval"] = requestURI.at(2);
                m_delete->view(data, writer);
                malformedRequest = false;
            }
        } else if (action == "query" && u.can(Auth::QUERY)) {
            if (!requestURI.count() != 1) {
                data["uri-query"] = requestURI.at(0);
                m_query->view(data, writer);
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
