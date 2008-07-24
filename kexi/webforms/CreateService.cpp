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

#include <QPair>
#include <QHash>
#include <QString>

#include <KDebug>

#include <pion/net/HTTPResponseWriter.hpp>

#include <google/template.h>

#include <kexidb/cursor.h>
#include <kexidb/connection.h>
#include <kexidb/queryschema.h>
#include <kexidb/roweditbuffer.h>
#include <kexidb/field.h>

#include "model/Database.h"

#include "auth/Authenticator.h"
#include "auth/User.h"
#include "auth/Permission.h"

#include "DataProvider.h"
#include "TemplateProvider.h"

#include "CreateService.h"

using namespace pion::net;

namespace KexiWebForms {

    void CreateService::operator()(pion::net::HTTPRequestPtr& request, pion::net::TCPConnectionPtr& tcp_conn) {
        HTTPResponseWriterPtr writer(HTTPResponseWriter::create(tcp_conn, *request,
                    boost::bind(&TCPConnection::finish, tcp_conn)));


        PionUserPtr userPtr(request->getUser());
        Auth::User u = Auth::Authenticator::getInstance()->authenticate(userPtr);

        if (u.can(Auth::CREATE)) {
            m_dict = initTemplate("create.tpl");

            /* Retrieve the requested table name */
            QString requestedTable(QString(request->getOriginalResource().c_str()).split('/').at(2));
            setValue("TABLENAME", requestedTable);


            KexiDB::TableSchema* tableSchema = gConnection->tableSchema(requestedTable);
            KexiWebForms::Model::Database db;


            /* Build the form */
            if (request->getQuery("dataSent") == "true") {
                QStringList fieldsList(QUrl::fromPercentEncoding(QString(
                            request->getQuery("tableFields").c_str()).toUtf8()
                ).split("|:|"));
                kDebug() << "Fields: " << fieldsList;

                QHash<QString, QVariant> data;
                foreach(const QString& field, fieldsList) {
                    KexiDB::Field* currentField = tableSchema->field(field);
                    if (currentField)
                        data[field] = QVariant(request->getQuery(field.toUtf8().constData()).c_str());
                }
                
                if (db.createRow(requestedTable, data)) {
                    m_dict->ShowSection("SUCCESS");
                    setValue("MESSAGE", "Row added successfully");
                } else {
                    m_dict->ShowSection("ERROR");
                    setValue("MESSAGE", gConnection->errorMsg());
                }
            }

            QString formData;
            QStringList fieldsList;
            
            QMap< QPair<QString, QString>, QPair<QString, KexiDB::Field::Type> > data(db.getSchema(requestedTable));
            QList< QPair<QString, QString> > dataKeys(data.keys());

            // WORK AROUND
            typedef QPair<QString, QString> QCaptionNamePair;
            
            // FIXME: Regression, no icons, this way
            foreach(const QCaptionNamePair& captionNamePair, data.keys()) {
                formData.append("\t<tr>\n");
                QPair<QString, KexiDB::Field::Type> valueTypePair(data[captionNamePair]);
                formData.append("\t\t<td>").append(captionNamePair.first).append("</td>\n");
                if (valueTypePair.second == KexiDB::Field::LongText) {
                    formData.append(QString("\t\t<td><textarea name=\"%1\"></textarea></td>\n").arg(captionNamePair.second));
                } else {
                    formData.append(QString("\t\t<td><input type=\"text\" name=\"%1\" value=\"%2\"/></td>\n")
                                    .arg(captionNamePair.second).arg(valueTypePair.first));
                }
                formData.append("\t</tr>\n");
                fieldsList << captionNamePair.second;
            }

            setValue("TABLEFIELDS", fieldsList.join("|:|"));
            setValue("FORMDATA", formData);

            renderTemplate(m_dict, writer);
            delete m_dict;
        } else {
            writer->write("Not Authorized");
            writer->send();
        }
    }

}
