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
#include <QByteArray>

#include <KMimeType>
#include <KDebug>

#include <pion/net/HTTPResponseWriter.hpp>
#include <pion/net/PionUser.hpp>

#include <kexidb/cursor.h>
#include <kexidb/connection.h>
#include <kexidb/queryschema.h>
#include <kexidb/roweditbuffer.h>
#include <kexidb/field.h>

#include "auth/Authenticator.h"
#include "auth/User.h"
#include "auth/Permission.h"

#include "DataProvider.h"

#include "BlobService.h"

using namespace pion::net;

namespace KexiWebForms {

    void BlobService::operator()(pion::net::HTTPRequestPtr& request, pion::net::TCPConnectionPtr& tcp_conn) {
        HTTPResponseWriterPtr writer(HTTPResponseWriter::create(tcp_conn, *request,
                    boost::bind(&TCPConnection::finish, tcp_conn)));
        
        PionUserPtr userPtr(request->getUser());
        Auth::User u = Auth::Authenticator::getInstance()->authenticate(userPtr);

        if (u.can(Auth::READ)) {
            QStringList queryString(QString(request->getOriginalResource().c_str()).split('/'));
            QString table(queryString.at(2));
            QString fieldName(queryString.at(3));
            QString pkey(queryString.at(4));
            QString pkeyVal(queryString.at(5));

            KexiDB::TableSchema* tableSchema = gConnection->tableSchema(table);
            KexiDB::Field* field = tableSchema->field(fieldName);

            if (field->type() == KexiDB::Field::BLOB) {
                // Perform the rest of the query
                KexiDB::QuerySchema query(*tableSchema);
                query.addToWhereExpression(tableSchema->field(pkey), QVariant(pkeyVal));
                KexiDB::Cursor* cursor = gConnection->executeQuery(query);

                QByteArray blobData;
                // There should be only one record
                cursor->moveNext();
                for (uint i = 0; i < cursor->fieldCount(); i++) {
                    if (query.field(i) == field) {
                        blobData = QByteArray(cursor->value(i).toByteArray());
                        break;
                    }
                }

                // Resolve the mime type for blobData
                KSharedPtr<KMimeType> mime = KMimeType::findByContent(blobData);
                if (mime) {
                    /// @todo wrong assumption: blobs are not always image/png
                    writer->getResponse().setContentType("image/png");
                    writer->writeNoCopy(blobData.data(), blobData.size());
                    writer->writeNoCopy(HTTPTypes::STRING_CRLF);
                    writer->writeNoCopy(HTTPTypes::STRING_CRLF);
                    writer->send();
                }

                gConnection->deleteCursor(cursor);
            }
        } else {
            writer->write("Not Authorized");
            writer->send();
        }
            
    }
    
}
