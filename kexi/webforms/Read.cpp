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

#include <Qt>
#include <QtGlobal>
#include <QString>

#include <KDebug>

#include <google/template.h>

#include <kexidb/indexschema.h>
#include <kexidb/connection.h>
#include <kexidb/cursor.h>

#include "Request.h"
#include "HTTPStream.h"
#include "DataProvider.h"
#include "TemplateProvider.h"

#include "Read.h"

namespace KexiWebForms {
    void readCallback(RequestData* req) {
        HTTPStream stream(req);
        google::TemplateDictionary* dict = initTemplate("read.tpl");


        QString requestedTable(Request::requestUri(req).split('/').at(2));
        dict->SetValue("TABLENAME", requestedTable.toLatin1().constData());

        QString tableData;
        KexiDB::TableSchema* tableSchema = gConnection->tableSchema(requestedTable);
        KexiDB::QuerySchema querySchema(*tableSchema);
        KexiDB::Cursor* cursor = gConnection->executeQuery(querySchema);

        if (!cursor) {
            dict->SetValue("ERROR", "Unable to execute the query (" __FILE__ ")");
        } else if (tableSchema->primaryKey()->fields()->isEmpty()) {
            dict->SetValue("ERROR", "This table has no primary key!");
        } else {
            KexiDB::Field* primaryKey = tableSchema->primaryKey()->field(0);

            // Create labels with field name
            tableData.append("<tr>");
            for (uint i = 0; i < cursor->fieldCount(); i++) {
                tableData.append("\t<th scope=\"col\">");
                tableData.append(querySchema.field(i)->captionOrName());
                tableData.append("</th>\n");
            }
            tableData.append("</tr>\n");


            // Create labels with fields data
            while (cursor->moveNext()) {
                tableData.append("<tr>");
                for (uint i = 0; i < cursor->fieldCount(); i++) {
                    tableData.append("<td>");

                    //
                    // Use Kexi functions to retrieve and represent the Value
                    //! @todo use Kexi the same functions for rendering values as Kexi table and form view
                    //
                    KexiDB::Field* field = querySchema.field(i);
                    const KexiDB::Field::Type type = field->type();
                    QString valueString;
                    if (type == KexiDB::Field::BLOB) {
                        //! @todo decode image and display it if possible
                        valueString = "(Object)";
                    }
                    else if (field->isTextType()) {
                        valueString = cursor->value(i).toString();
                        //! @fixme: why I don't have Qt::escape ?
                        //valueString = Qt::escape( cursor->value(i).toString() );
                    }
                    else {
                        valueString = cursor->value(i).toString();
                    }
                    tableData.append(valueString);
                    tableData.append("</td>");
                }
                // Toolbox
                QString pkeyVal(cursor->value(tableSchema->indexOf(primaryKey)).toString());
                // Edit
                tableData.append("<td><a href=\"/update/").append(requestedTable).append("/");
                tableData.append(primaryKey->name()).append("/");
                tableData.append(pkeyVal).append("\"><img src=\"/toolbox/draw-freehand.png\" alt=\"Edit\"/></a></td>");
                // Delete
                tableData.append("<td><a href=\"/delete/").append(requestedTable).append("/");
                tableData.append(primaryKey->name()).append("/");
                tableData.append(pkeyVal).append("\"><img src=\"/toolbox/draw-eraser.png\" alt=\"Delete\"/></a></td>");
                // End row
                tableData.append("</tr>");

                dict->SetValue("TABLEDATA", tableData.toUtf8().constData());
            }

            kDebug() << "Deleting cursor..." << endl;
            gConnection->deleteCursor(cursor);
        }

        renderTemplate(dict, stream);
    }

    // Read Handler
    ReadHandler::ReadHandler() : Handler(readCallback) {}
}
