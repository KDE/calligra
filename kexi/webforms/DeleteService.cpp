/* This file is part of the KDE project

   (C) Copyright 2008 by Lorenzo Villani <lvillani@binaryhelix.net>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include <string>

#include <google/template.h>
#include <pion/net/HTTPResponseWriter.hpp>

#include <QString>

#include <KDebug>

#include <kexidb/utils.h>
#include <kexidb/queryschema.h>
#include <kexidb/cursor.h>

#include "auth/Authenticator.h"
#include "auth/User.h"
#include "auth/Permission.h"

#include "DataProvider.h"
#include "TemplateProvider.h"

#include "DeleteService.h"

using namespace pion::net;

namespace KexiWebForms {
    
    void DeleteService::operator()(pion::net::HTTPRequestPtr& request, pion::net::TCPConnectionPtr& tcp_conn) {
        HTTPResponseWriterPtr writer(HTTPResponseWriter::create(tcp_conn, *request,
                    boost::bind(&TCPConnection::finish, tcp_conn)));


        PionUserPtr userPtr(request->getUser());
        Auth::User u = Auth::Authenticator::getInstance()->authenticate(userPtr);
        
        if (u.can(Auth::DELETE)) {
            /// @todo ensure that there's the correct number of parameters
            QStringList queryString(QString(request->getOriginalResource().c_str()).split('/'));
            QString requestedTable = queryString.at(2);
            QString pkeyName = queryString.at(3);
            QString pkeyValue = queryString.at(4);

            setValue("TABLENAME", requestedTable);

            kDebug() << "Trying to delete row..." << endl;
            if (KexiDB::deleteRow(*gConnection, gConnection->tableSchema(requestedTable),
                                pkeyName, pkeyValue)) {
                m_dict->ShowSection("SUCCESS");
                setValue("MESSAGE", "Row deleted successfully");
            } else {
                m_dict->ShowSection("ERROR");
                /// @todo retrieve proper error message
                setValue("MESSAGE", "Error while trying to delete row!");
            }
            
            renderTemplate(m_dict, writer);
            
        } else {
            writer->write("Not Authorized");
            writer->send();
        }
    }

}
