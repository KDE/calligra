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

#include <QString>
#include <QStringList>
#include <QHash>
#include <QList>
#include <QUrl>

#include <KDebug>

#include <pion/net/HTTPResponseWriter.hpp>

#include <kexidb/roweditbuffer.h>
#include <kexidb/connection.h>
#include <kexidb/queryschema.h>
#include <kexidb/cursor.h>

#include <google/template.h>

#include "model/Database.h"

#include "auth/Authenticator.h"
#include "auth/User.h"
#include "auth/Permission.h"

#include "DataProvider.h"
#include "TemplateProvider.h"

#include "UpdateService.h"

using namespace pion::net;

namespace KexiWebForms {

    void UpdateService::operator()(pion::net::HTTPRequestPtr& request, pion::net::TCPConnectionPtr& tcp_conn) {
        HTTPResponseWriterPtr writer(HTTPResponseWriter::create(tcp_conn, *request,
                                                                boost::bind(&TCPConnection::finish, tcp_conn)));

        PionUserPtr userPtr(request->getUser());
        Auth::User u = Auth::Authenticator::getInstance()->authenticate(userPtr);

        if (u.can(Auth::UPDATE)) {
            m_dict = initTemplate("update.tpl");


            QStringList queryString(QString(request->getOriginalResource().c_str()).split('/'));
            QString requestedTable = queryString.at(2);
            QString pkeyName = queryString.at(3);
            QString pkeyValue = queryString.at(4);
            uint pkeyValueUInt = pkeyValue.toUInt();
            uint current = 0;


            setValue("TABLENAME", requestedTable);
            setValue("PKEY_NAME", pkeyName);
            setValue("PKEY_VALUE", pkeyValue);

            // Initialize our needed Objects
            KexiDB::TableSchema tableSchema(*gConnection->tableSchema(requestedTable));
            KexiDB::QuerySchema schema(tableSchema);
            schema.addToWhereExpression(schema.field(pkeyName), QVariant(pkeyValue));

            KexiWebForms::Model::Database db;

            /*!
             * @note We shouldn't use executeQuery otherwise the corresponding table will
             * be locked and we won't be able to update it
             */
            KexiDB::Cursor* cursor = gConnection->prepareQuery(schema);

            // Fill the cachedPkeys list
            if (cachedPkeys[requestedTable].isEmpty()) {
                kDebug() << "Cached Pkeys is empty, updating" << endl;
                KexiDB::QuerySchema idSchema;
                idSchema.addField(tableSchema.primaryKey()->field(0));
                KexiDB::Cursor* idCursor = gConnection->executeQuery(idSchema);
                while (idCursor->moveNext()) {
                    kDebug() << "Appending " << idCursor->value(0).toUInt() << " to cache" << endl;
                    cachedPkeys[requestedTable].append(idCursor->value(0).toUInt());
                }
                gConnection->deleteCursor(idCursor);
            }

            // Retrieve current position in cache
            for (int i = 0; i < cachedPkeys[requestedTable].size(); i++) {
                if (cachedPkeys[requestedTable].at(i) == pkeyValueUInt)
                    current = i;
            }

            // Compute new primary key values for first, last, previous and next record
            if (current < uint( cachedPkeys[requestedTable].size()-1 )) {
                m_dict->ShowSection("NEXT_ENABLED");
                m_dict->SetValue("NEXT", QVariant(cachedPkeys[requestedTable].at(current+1)).toString().toUtf8().constData());
            } else {
                m_dict->ShowSection("NEXT_DISABLED");
            }

            if (current > 0) {
                m_dict->ShowSection("PREV_ENABLED");
                m_dict->SetValue("PREV", QVariant(cachedPkeys[requestedTable].at(current-1)).toString().toUtf8().constData());
            } else {
                m_dict->ShowSection("PREV_DISABLED");
            }

            if (current >= cachedPkeys[requestedTable].at(0)) {
                m_dict->ShowSection("FIRST_ENABLED");
                m_dict->SetValue("FIRST", QVariant(cachedPkeys[requestedTable].at(0)).toString().toUtf8().constData());
            } else {
                m_dict->ShowSection("FIRST_DISABLED");
            }


            if (current < uint( cachedPkeys[requestedTable].size()-1 )) {
                m_dict->ShowSection("LAST_ENABLED");
                m_dict->SetValue("LAST", QVariant(cachedPkeys[requestedTable].at(cachedPkeys[requestedTable].size()-1)).toString().toUtf8().constData());
            } else {
                m_dict->ShowSection("LAST_DISABLED");
            }



            if (!cursor) {
                m_dict->ShowSection("ERROR");
                setValue("MESSAGE", "No cursor object available");
            } else {
                if (request->getQuery("dataSent") == "true") {
                    cursor = gConnection->prepareQuery(schema);

                    QStringList fieldsList(QUrl::fromPercentEncoding(QString(
                                                                         request->getQuery("tableFields").c_str()).toUtf8()
                                               ).split("|:|"));
                    kDebug() << "Fields: " << fieldsList;

                    QStringListIterator iterator(fieldsList);

                    KexiDB::RecordData recordData(tableSchema.fieldCount());
                    KexiDB::RowEditBuffer editBuffer(true);

                    QVector<int> pkeyFields(schema.pkeyFieldsOrder());
                    for (int i = 0; i < pkeyFields.size(); i++) {
                        int fieldId = pkeyFields.at(i);
                        if (schema.field(fieldId)->name() == pkeyName) {
                            recordData.insert(fieldId, pkeyValue);
                            /**
                             * @note No need to fill other primary key values.
                             * As reported by Jaroslaw KexiDB supports multi pkey
                             * tables but the table designer not
                             */
                            break;
                        }
                    }

                    /*! @note Making the wrong assumption on what the pkey id is */
                    recordData.insert(0, QVariant(pkeyValue));

                    while (iterator.hasNext()) {
                        QString currentFieldName(iterator.next());
                        QString currentFieldValue(QUrl::fromPercentEncoding(QString(
                                                                                request->getQuery(currentFieldName.toUtf8().constData()).c_str()).toUtf8()
                                                      ));
                        // safeQString currentFieldValue(request->getQuery(currentFieldName.toUtf8().constData()).c_str());

                        /*! @fixme This removes pluses */
                        currentFieldValue.replace("+", " ");
                        QVariant currentValue(currentFieldValue);

                        if (currentFieldName != pkeyName) {
                            kDebug() << "Inserting " << currentFieldName << "=" << currentValue.toString() << endl;
                            editBuffer.insert(*schema.columnInfo(currentFieldName), currentValue);
                        }
                    }


                    if (cursor->updateRow(recordData, editBuffer)) {
                        m_dict->ShowSection("SUCCESS");
                        setValue("MESSAGE", "Row updated successfully");
                        // A successful update marks the cache as empty
                        cachedPkeys[requestedTable].clear();
                    } else {
                        m_dict->ShowSection("ERROR");
                        setValue("MESSAGE", gConnection->errorMsg().toUtf8().constData());
                    }

                    kDebug() << "Deleting cursor..." << endl;
                    gConnection->deleteCursor(cursor);
                }

                kDebug() << "Showing fields" << endl;

                cursor = gConnection->executeQuery(schema);

                m_dict->ShowSection("FORM");

                
                QString formData;
                QStringList formFieldsList;

                QMap< QPair<QString, QString>, QPair<QString, KexiDB::Field::Type> > data(db.getSchema(requestedTable,
                                                                                                       pkeyName, pkeyValue.toInt()));
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
                    } else if (valueTypePair.second == KexiDB::Field::BLOB) {
                        formData.append(QString("<td><img src=\"/blob/%1/%2/%3/%4\" alt=\"Image\"/></td>")
                                        .arg(requestedTable).arg(captionNamePair.second).arg(pkeyName)
                                        .arg(pkeyValue));
                    } else {
                        formData.append(QString("\t\t<td><input type=\"text\" name=\"%1\" value=\"%2\"/></td>\n")
                                        .arg(captionNamePair.second).arg(valueTypePair.first));
                    }
                    formData.append("\t</tr>\n");
                    formFieldsList << captionNamePair.second;
                }
                
                setValue("TABLEFIELDS", formFieldsList.join("|:|"));
                setValue("FORMDATA", formData);

                gConnection->deleteCursor(cursor);
            }

            renderTemplate(m_dict, writer);
            delete m_dict;
        } else {
            writer->write("Not Authorized");
            writer->send();
        }
    }
    
}
