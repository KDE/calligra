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

#include <QTextDocument>
#include <QString>

#include <pion/net/HTTPResponseWriter.hpp>

#include <KDebug>

#include <kexidb/indexschema.h>
#include <kexidb/connection.h>
#include <kexidb/cursor.h>

#include "DataProvider.h"
#include "TemplateProvider.h"

#include "ReadService.h"

using namespace pion::net;

namespace KexiWebForms {
    
    void ReadService::operator()(pion::net::HTTPRequestPtr& request, pion::net::TCPConnectionPtr& tcp_conn) {
        HTTPResponseWriterPtr writer(HTTPResponseWriter::create(tcp_conn, *request,
                    boost::bind(&TCPConnection::finish, tcp_conn)));

        QString requestedTable(QString(request->getOriginalResource().c_str()).split('/').at(2));

        QString tableData;
        KexiDB::TableSchema* tableSchema = gConnection->tableSchema(requestedTable);
        KexiDB::QuerySchema querySchema(*tableSchema);
        KexiDB::Cursor* cursor = gConnection->executeQuery(querySchema);
        
        bool readOnly = (querySchema.connection() && querySchema.connection()->isReadOnly());
        if (readOnly) {
            setValue("TABLENAME", requestedTable.append(" (read only)"));
        } else {
            setValue("TABLENAME", requestedTable);
        }

        /* awful */
        int recordsTotal = 0;
        while (cursor->moveNext())
            recordsTotal++;
        cursor->close();

        /* even more awful */
        cursor = gConnection->executeQuery(querySchema);

        if (!cursor) {
            setValue("ERROR", "Unable to execute the query (" __FILE__ ")");
        } else if (tableSchema->primaryKey()->fields()->isEmpty()) {
            setValue("ERROR", "This table has no primary key!");
        } else {
            KexiDB::Field* primaryKey = tableSchema->primaryKey()->field(0);

            // Create labels with field name
            tableData.append("<tr>");
            tableData.append("\t<th scope=\"col\">Record</th>\n");
            for (uint i = 0; i < cursor->fieldCount(); i++) {
                tableData.append("\t<th scope=\"col\">");
                tableData.append(querySchema.field(i)->captionOrName());
                tableData.append("</th>\n");
            }
            tableData.append("</tr>\n");


            // Create labels with fields data
            int currentRecord = 0;
            QString totalRecords(QVariant(recordsTotal).toString());
            while (cursor->moveNext()) {
                currentRecord++;
                
                tableData.append("<tr>");
                tableData.append("<td>").append(QVariant(currentRecord).toString()).append(" of ");
                tableData.append(totalRecords).append("</td>");
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
                        valueString = "<img src=\"data:image/png;base64,";
                        valueString.append(cursor->value(i).toByteArray().toBase64());
                        valueString.append("\" alt=\"").append(field->captionOrName()).append("\"/>\n");
                    }
                    else if (field->isTextType()) {
                        valueString = Qt::escape( cursor->value(i).toString() );
                    }
                    else {
                        valueString = Qt::escape( cursor->value(i).toString() );
                    }
                    tableData.append(valueString);
                    tableData.append("</td>");
                }
                // Toolbox
                QString pkeyVal(cursor->value(tableSchema->indexOf(primaryKey)).toString());
                if (!readOnly) {
                    // Edit
                    tableData.append("<td><a href=\"/update/").append(requestedTable).append("/");
                    tableData.append(primaryKey->name()).append("/");
                    tableData.append(pkeyVal).append("\"><img src=\"/f/toolbox/draw-freehand.png\" alt=\"Edit\"/></a></td>");
                    // Delete
                    tableData.append("<td><a href=\"/delete/").append(requestedTable).append("/");
                    tableData.append(primaryKey->name()).append("/");
                    tableData.append(pkeyVal).append("\"><img src=\"/f/toolbox/draw-eraser.png\" alt=\"Delete\"/></a></td>");
                    // End row
                }
                tableData.append("</tr>\n");
            }

            setValue("TABLEDATA", tableData);

            kDebug() << "Deleting cursor..." << endl;
            gConnection->deleteCursor(cursor);
        }

        renderTemplate(m_dict, writer);
    }
    
}
