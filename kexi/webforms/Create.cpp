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

#include <KDebug>

#include <google/template.h>

#include <kexidb/cursor.h>
#include <kexidb/connection.h>
#include <kexidb/queryschema.h>
#include <kexidb/roweditbuffer.h>
#include <kexidb/field.h>

#include "Request.h"
#include "HTTPStream.h"
#include "DataProvider.h"
#include "TemplateProvider.h"

#include "Create.h"

namespace KexiWebForms {

    void createCallback(RequestData* req) {
        HTTPStream stream(req);
        google::TemplateDictionary* dict = initTemplate("create.tpl");

        /* Retrieve the requested table name */
        QString requestedTable = Request::requestUri(req).split('/').at(2);
        dict->SetValue("TABLENAME", requestedTable.toUtf8().constData());


        KexiDB::TableSchema* tableSchema = gConnection->tableSchema(requestedTable);


        /* Build the form */
        if (Request::request(req, "dataSent") == "true") {
            KexiDB::QuerySchema schema(*tableSchema);
            KexiDB::Cursor* cursor = gConnection->prepareQuery(schema);

            QStringList fieldsList(Request::request(req, "tableFields").split("|:|"));
            kDebug() << "Fields: " << fieldsList;

            QStringListIterator iterator(fieldsList);

            KexiDB::RecordData recordData(tableSchema->fieldCount());
            KexiDB::RowEditBuffer editBuffer(true);

            while (iterator.hasNext()) {
                QString currentFieldName(iterator.next());
                QString currentFieldValue(QUrl::fromPercentEncoding(Request::request(req, currentFieldName).toUtf8()));

                /*! @fixme This removes pluses */
                currentFieldValue.replace("+", " ");
                QVariant currentValue(currentFieldValue);

                kDebug() << "Inserting " << currentFieldName << "=" << currentValue.toString() << endl;
                editBuffer.insert(*schema.columnInfo(currentFieldName), currentValue);
            }


            if (cursor->insertRow(recordData, editBuffer)) {
                dict->ShowSection("SUCCESS");
                dict->SetValue("MESSAGE", "Row added successfully");
            } else {
                dict->ShowSection("ERROR");
                dict->SetValue("MESSAGE", gConnection->errorMsg().toUtf8().constData());
            }

            kDebug() << "Deleting cursor..." << endl;
            gConnection->deleteCursor(cursor);
        } else {
            dict->ShowSection("FORM");

            QString formData;
            QStringList fieldsList;

            for (uint i = 0; i < tableSchema->fieldCount(); i++) {
                KexiDB::Field* currentField = tableSchema->field(i);
                QString fieldName(currentField->name());

                formData.append("<tr>");
                    
                formData.append("<td>").append(currentField->captionOrName()).append("</td>");
                formData.append("<td>").append("<input type=\"text\" name=\"");
                formData.append(fieldName).append("\" value=\"").append(currentField->defaultValue().toString());
                formData.append("\"/></td>");

                // Field properties images
                formData.append("<td>");
                currentField->isAutoIncrement() ? formData.append("<img src=\"/toolbox/auto-increment.png\" alt=\"Auto increment\"/>&nbsp;") : 0;
                currentField->isPrimaryKey() ? formData.append("<img src=\"/toolbox/primary-key.png\" alt=\"Primary Key\"/>&nbsp;") : 0;
                currentField->isNotEmpty() ? formData.append("<img src=\"/toolbox/emblem-required.png\" alt=\"Required\"/>&nbsp;") : 0;
                formData.append("</td>");
                    
                formData.append("</tr>");
                fieldsList << fieldName;
                
                /*QString fieldName(tableSchema->field(i)->name());

                formData.append("<tr>");
                formData.append("<td>").append(tableSchema->field(i)->captionOrName()).append("</td>");
                formData.append("<td><input type=\"text\" name=\"");
                formData.append(fieldName).append("\" value=\"\"/></td>");
                formData.append("</tr>");
                fieldsList << fieldName;*/
            }

            dict->SetValue("TABLEFIELDS", fieldsList.join("|:|").toUtf8().constData());
            dict->SetValue("FORMDATA", formData.toUtf8().constData());
        }


        renderTemplate(dict, stream);
    }


    // Create Handler
    CreateHandler::CreateHandler() : Handler(createCallback) {}
}
