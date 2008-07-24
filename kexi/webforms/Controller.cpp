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

#include <QString>
#include <QStringList>

#include <KDebug>

#include <pion/net/WebService.hpp>
#include <pion/net/HTTPResponseWriter.hpp>

#include "view/Index.h"

#include "Controller.h"

/*#include "CreateService.h"
#include "ReadService.h"
#include "UpdateService.h"
#include "DeleteService.h"
#include "QueryService.h"*/

namespace KexiWebForms {

    Controller::Controller() {
        m_index = new View::Index(*this, "create");
        /*m_createService = new CreateService(this, "create");
        m_readService = new ReadService(this, "read");
        m_updateService = new UpdateService(this, "update");
        m_deleteService = new DeleteService(this, "delete");
        m_queryService = new QueryService(this, "query");*/
    }

    Controller::~Controller() {
        delete m_index;
        /*delete m_createService;
        delete m_readService;
        delete m_updateService;
        delete m_deleteService;
        delete m_queryService;*/
    }

    void Controller::operator()(pion::net::HTTPRequestPtr& request, pion::net::TCPConnectionPtr& tcp_conn) {
        pion::net::HTTPResponseWriterPtr writer(pion::net::HTTPResponseWriter::create(tcp_conn, *request,
                                                                                      boost::bind(&pion::net::TCPConnection::finish, tcp_conn)));
        
        QStringList requestURI(QString(request->getOriginalResource().c_str()).split('/'));
        /*requestURI.removeFirst();
        
        QString action(requestURI.at(0));
        requestURI.removeFirst();*/

        QList<QString> foo;
        
        //kDebug() << "ACTION :" << action << endl;
        kDebug() << "PARAMETERS COUNT: " << requestURI.count() << endl;

        /*if (action == "") {
            if (requestURI.count() != 0) {
                writer->writeNoCopy("Malformed request");
                } else {*/
                m_index->view(foo, writer);
                /*    }
                      }*/

        writer->writeNoCopy(pion::net::HTTPTypes::STRING_CRLF);
        writer->writeNoCopy(pion::net::HTTPTypes::STRING_CRLF);
        writer->send();

        kDebug() << requestURI << endl;
    }
    
}
