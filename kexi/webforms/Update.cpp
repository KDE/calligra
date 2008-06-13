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
#include <QMap>
#include <QUrl>

#include <KDebug>

#include <kexidb/roweditbuffer.h>
#include <kexidb/connection.h>
#include <kexidb/queryschema.h>
#include <kexidb/cursor.h>

#include <google/template.h>

#include "Handler.h"

#include "DataProvider.h"
#include "HTTPStream.h"
#include "Request.h"

#include "Update.h"

namespace KexiWebForms {

    void updateCallback(RequestData* req) {
        HTTPStream stream(req);
        google::TemplateDictionary dict("update");


        QStringList queryString = Request::requestUri(req).split("/");
        QString requestedTable = queryString.at(2);
        QString pkeyName = queryString.at(3);
        QString pkeyValue = queryString.at(4);


        dict.SetValue("TABLENAME", requestedTable.toLatin1().constData());
        dict.SetValue("PKEY_NAME", pkeyName.toLatin1().constData());
        dict.SetValue("PKEY_VALUE", pkeyValue.toLatin1().constData());

        // Initialize our needed Objects
        KexiDB::TableSchema tableSchema(*gConnection->tableSchema(requestedTable));
        KexiDB::QuerySchema schema(tableSchema);
        schema.addToWhereExpression(schema.field(pkeyName), QVariant(pkeyValue));
            
        /*!
         * @note We shouldn't use executeQuery otherwise the corresponding table will
         * be locked and we won't be able to update it
         */
        KexiDB::Cursor* cursor = gConnection->prepareQuery(schema);

            
        if (!cursor) {
            dict.ShowSection("ERROR");
            dict.SetValue("MESSAGE", "No cursor object available");
        } else if (Request::request(req, "dataSent") == "true") {
            cursor = gConnection->prepareQuery(schema);
                
            QStringList fieldsList = Request::request(req, "tableFields").split("|:|");
            kDebug() << "Fields: " << fieldsList;

            QStringListIterator iterator(fieldsList);

            KexiDB::RecordData recordData(tableSchema.fieldCount());
            KexiDB::RowEditBuffer editBuffer(true);

            /*! @fixme Making the wrong assumption on what the pkey id is */
            recordData.insert(0, QVariant(pkeyValue));
                
            while (iterator.hasNext()) {
                QString currentFieldName(iterator.next());
                QString currentFieldValue(QUrl::fromPercentEncoding(Request::request(req, currentFieldName).toLatin1()));

                /*! @fixme This removes pluses */
                currentFieldValue.replace("+", " ");
                QVariant currentValue(currentFieldValue);
                    
                if (currentFieldName != pkeyName) {
                    kDebug() << "Inserting " << currentFieldName << "=" << currentValue.toString() << endl;
                    editBuffer.insert(*schema.columnInfo(currentFieldName), currentValue);
                }
            }

                
            if (cursor->updateRow(recordData, editBuffer)) {
                dict.ShowSection("SUCCESS");
                dict.SetValue("MESSAGE", "Row updated successfully");
            } else {
                dict.ShowSection("ERROR");
                dict.SetValue("MESSAGE", gConnection->errorMsg().toLatin1().constData());
            }
                
            kDebug() << "Deleting cursor..." << endl;
            gConnection->deleteCursor(cursor);
        } else {
            kDebug() << "Showing fields" << endl;

            cursor = gConnection->executeQuery(schema);

            dict.ShowSection("FORM");

            QString formData;
            QStringList fieldsList;

            while (cursor->moveNext()) {
                for (uint i = 0; i < cursor->fieldCount(); i++) {
                    QString fieldName(schema.field(i)->name());
                            
                    formData.append("<tr>");
                    formData.append("<td>").append(schema.field(i)->captionOrName()).append("</td>");
                    formData.append("<td><input type=\"text\" name=\"");
                    formData.append(fieldName).append("\" value=\"");
                    formData.append(cursor->value(i).toString()).append("\"/></td>");
                    formData.append("</tr>");
                    fieldsList << fieldName;
                }
            }
            dict.SetValue("TABLEFIELDS", fieldsList.join("|:|").toLatin1().constData());
            dict.SetValue("FORMDATA", formData.toLatin1().constData());

            kDebug() << "Deleting cursor..." << endl;
            gConnection->deleteCursor(cursor);
        }
			
			
        //
        // Produce the final output	
        //
        std::string output;			
        google::Template* tpl = google::Template::GetTemplate("update.tpl", google::DO_NOT_STRIP);
        tpl->Expand(&output, &dict);
        stream << output << webend;
    }
    

    // Update Handler
    UpdateHandler::UpdateHandler() : Handler(updateCallback) {}
}
