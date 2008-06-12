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

#include <KDebug>

#include <kexidb/roweditbuffer.h>
#include <kexidb/connection.h>
#include <kexidb/queryschema.h>
#include <kexidb/cursor.h>

#include <google/template.h>

#include "DataProvider.h"
#include "HTTPStream.h"
#include "Request.h"

#include "Update.h"

namespace KexiWebForms {
    namespace Update {
        void show(RequestData* req) {
            HTTPStream stream(req);
            google::TemplateDictionary dict("update");


            QStringList queryString = Request::requestUri(req).split("/");
            QString requestedTable = queryString.at(2);
            QString pkeyName = queryString.at(3);
            QString pkeyValue = queryString.at(4);


            dict.SetValue("TABLENAME", requestedTable.toLatin1().constData());
            dict.SetValue("PKEY_NAME", pkeyName.toLatin1().constData());
            dict.SetValue("PKEY_VALUE", pkeyValue.toLatin1().constData());
            
            KexiDB::TableSchema tableSchema(*gConnection->tableSchema(requestedTable));
            KexiDB::QuerySchema schema(tableSchema);
            schema.addToWhereExpression(schema.field(pkeyName), QVariant(pkeyValue));
            KexiDB::Cursor* cursor = gConnection->executeQuery(schema);


            /// @fixme: Can this code be improved?
            if (Request::request(req, "dataSent") == "true" && cursor) {
                QStringList fieldsList = Request::request(req, "tableFields").split("|:|");
                kDebug() << "Fields: " << fieldsList;

                QStringListIterator iterator(fieldsList);

                KexiDB::RecordData recordData(tableSchema.fieldCount());
                KexiDB::RowEditBuffer editBuffer(true);
                
                while (iterator.hasNext()) {
                    QString currentFieldName(iterator.next());
                    QVariant* currentValue = new QVariant(Request::request(req, currentFieldName));

                    kDebug() << "Inserting " << currentFieldName << "=" << currentValue->toString() << endl;
                    editBuffer.insert(*schema.columnInfo(currentFieldName), *currentValue);
                }

                if (cursor) {
                    if (cursor->updateRow(recordData, editBuffer)) {
                        dict.ShowSection("SUCCESS");
                        dict.SetValue("MESSAGE", "Row updated successfully");
                    } else {
                        dict.ShowSection("ERROR");
                        dict.SetValue("MESSAGE", "Failed to update row");
                        kDebug() << "Connection ERROR: " << gConnection->errorMsg() << endl;
                    }
                }
            } else {
                kDebug() << "Showing fields" << endl;

                dict.ShowSection("FORM");

                QString formData;
                QStringList fieldsList;
                if (cursor) {
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
                }
            }
			
			
            //
            // Produce the final output	
            //
            std::string output;			
            google::Template* tpl = google::Template::GetTemplate("update.tpl", google::DO_NOT_STRIP);
            tpl->Expand(&output, &dict);
            stream << output << webend;
        }
    }
}
