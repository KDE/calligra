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

#include <QHash>
#include <QString>

#include <pion/net/HTTPResponseWriter.hpp>

#include <KDebug>

#include <kexidb/cursor.h>
#include <kexidb/connection.h>
#include <kexidb/indexschema.h>

#include "../../model/Database.h"
#include "../../model/DataProvider.h"

#include "TemplateProvider.h"

#include "Read.h"

namespace KexiWebForms {
namespace View {

    void Read::view(const QHash<QString, QString>& d, pion::net::HTTPResponseWriterPtr writer) {
        QString requestedTable(d["uri-table"]);

        QString tableData;

        QPair< KexiDB::TableSchema, QMap<uint, QList<QString> > > pair = KexiWebForms::Model::Database::readTable(requestedTable);

        uint totalRecords = pair.second.count();
        QString primaryKey(pair.first.primaryKey()->field(0)->name());

        // Table header
        tableData.append("<tr>\t<th scope=\"col\">Record</th>\n");
        foreach(const KexiDB::Field* f, *pair.first.fields()) {
            tableData.append(QString("\t<th scope=\"col\">%1</th>\n").arg(f->captionOrName()));
        }
        tableData.append("</tr>\n");

        // Table contents
        foreach(const uint record, pair.second.keys()) {
            tableData.append("<tr>");
            tableData.append(QString("<td>%1 of %2</td>").arg(record).arg(totalRecords));
            QString pkeyVal(pair.second[record].at(pair.first.primaryKey()->field(0)->order()));
            
            foreach(const QString& value, pair.second[record]) {
                tableData.append(QString("\t<td>%1</td>\n").arg(value));
            }
            // Toolbox
            // Edit
            tableData.append(QString("<td><a href=\"/update/%1/%2/%3\">"
                                     "<img src=\"/f/toolbox/draw-freehand.png\" alt=\"Edit\"/></a></td>")
                             .arg(requestedTable).arg(primaryKey).arg(pkeyVal));
            // Delete
            tableData.append(QString("<td><a href=\"/delete/%1/%2/%3\">"
                                     "<img src=\"/f/toolbox/draw-eraser.png\" alt=\"Edit\"/></a></td>")
                             .arg(requestedTable).arg(primaryKey).arg(pkeyVal));
            tableData.append("</tr>\n");
        }

        setValue("TABLEDATA", tableData);

        /*bool readOnly = (querySchema.connection() && querySchema.connection()->isReadOnly());
        if (readOnly) {
            setValue("TABLENAME", requestedTable.append(" (read only)"));
        } else {
            setValue("TABLENAME", requestedTable);
        }

        /* awful 
        int recordsTotal = 0;
        while (cursor->moveNext())
            recordsTotal++;
        gConnection->deleteCursor(cursor);

        /* even more awful 
        cursor = gConnection->executeQuery(querySchema);

        if (!cursor) {
            setValue("ERROR", "Unable to execute the query (" __FILE__ ")");
        } else if (tableSchema->primaryKey()->fields()->isEmpty()) {
            setValue("ERROR", "This table has no primary key!");
        } else {
            KexiDB::Field* primaryKey = tableSchema->primaryKey()->field(0);

            // Create labels with field name
            tableData.append("<tr>\t<th scope=\"col\">Record</th>\n");
            for (uint i = 0; i < cursor->fieldCount(); i++) {
                tableData.append(QString("\t<th scope=\"col\">%1</th>\n")
                    .arg(querySchema.field(i)->captionOrName()));
            }
            tableData.append("</tr>\n");


            // Create labels with fields data
            int currentRecord = 0;
            QString totalRecords(QVariant(recordsTotal).toString());
            while (cursor->moveNext()) {
                currentRecord++;

                tableData.append(QString("<tr><td>%1 of %2</td>").arg(QVariant(currentRecord).toString())
                    .arg(totalRecords));

                QString pkeyVal(cursor->value(tableSchema->indexOf(primaryKey)).toString());

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
                        valueString = QString("<img src=\"/blob/%1/%2/%3/%4\" alt=\"Image\"/>")
                            .arg(requestedTable).arg(field->name()).arg(primaryKey->name())
                            .arg(pkeyVal);
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
                if (!readOnly) {
                    // Edit
                    tableData.append(QString("<td><a href=\"/update/%1/%2/%3\">"
                        "<img src=\"/f/toolbox/draw-freehand.png\" alt=\"Edit\"/></a></td>")
                        .arg(requestedTable).arg(primaryKey->name()).arg(pkeyVal));
                    // Delete
                    tableData.append(QString("<td><a href=\"/delete/%1/%2/%3\">"
                        "<img src=\"/f/toolbox/draw-eraser.png\" alt=\"Edit\"/></a></td>")
                        .arg(requestedTable).arg(primaryKey->name()).arg(pkeyVal));
                    // End row
                }
                tableData.append("</tr>\n");
            }

            setValue("TABLEDATA", tableData);

            kDebug() << "Deleting cursor..." << endl;
            gConnection->deleteCursor(cursor);
        }*/

        renderTemplate(m_dict, writer);

    }
    
}    
}
