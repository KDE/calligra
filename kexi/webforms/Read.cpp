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
#include "Server.h"
#include "HTTPStream.h"
#include "DataProvider.h"

#include "Read.h"

namespace KexiWebForms {
    void readCallback(RequestData* req) {
        HTTPStream stream(req);
        google::TemplateDictionary dict("table");

            
        QString requestedTable = Request::requestUri(req);
        // FIXME: Mhhh, nasty things can happen with that
        requestedTable.remove(0, 6);
        dict.SetValue("TABLENAME", requestedTable.toLatin1().constData());

        QString tableData;
        KexiDB::TableSchema* tableSchema = gConnection->tableSchema(requestedTable);

            
        /*
         * Check if requested table has primary keys
         * If yes use it to generate query strings, using the first
         * primary key found, if no primary key is found, print an error message
         */
        KexiDB::IndexSchema* indexSchema = tableSchema->primaryKey();
        if (indexSchema->fields()->isEmpty()) {
            dict.SetValue("ERROR", "This table has no primary key!");
        } else {
            KexiDB::Field* primaryKey = indexSchema->field(0);
            KexiDB::QuerySchema querySchema(*tableSchema);
            KexiDB::Cursor* cursor = gConnection->executeQuery(querySchema);

            if (cursor) {
                // Create labels with field name
                tableData.append("<tr>");
                for (uint i = 0; i < cursor->fieldCount(); i++) {
                    tableData.append("\t<td><strong>");
                    tableData.append(querySchema.field(i)->captionOrName());
                    tableData.append("</strong></td>\n");
                }
                tableData.append("</tr>\n");

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
                    tableData.append(pkeyVal).append("\">Edit</a></td>");
                    // Delete
                    tableData.append("<td><a href=\"/delete/").append(requestedTable).append("/");
                    tableData.append(primaryKey->name()).append("/");
                    tableData.append(pkeyVal).append("\">Delete</a></td>");
                    // End row
                    tableData.append("</tr>");
                }
                dict.SetValue("TABLEDATA", tableData.toLatin1().constData());
            } else {
                dict.SetValue("TABLEDATA", "Error in " __FILE__); // mhh...
            }

            kDebug() << "Deleting cursor..." << endl;
            gConnection->deleteCursor(cursor);
        }

        // Render the template
        std::string output;
        google::Template* tpl = google::Template::GetTemplate("table.tpl", google::DO_NOT_STRIP);
        tpl->Expand(&output, &dict);
        stream << output << webend;
    }

    // Read Handler
    ReadHandler::ReadHandler() : Handler(readCallback) {}
}
