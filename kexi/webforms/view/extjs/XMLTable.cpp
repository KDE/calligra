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
#include <QTextDocument>

#include <pion/net/HTTPResponseWriter.hpp>

#include <KDebug>

#include <db/cursor.h>
#include <db/connection.h>
#include <db/indexschema.h>

#include "../../model/Database.h"
#include "../../model/DataProvider.h"

#include "XMLTable.h"

namespace KexiWebForms {
namespace View {

void XMLTable::view(const QHash<QString, QString>& d, pion::net::HTTPResponseWriterPtr writer) {
    QString requestedTable(d["uri-table"]);

    QString tableData;
    KexiDB::TableSchema* tableSchema = gConnection->tableSchema(requestedTable);
    KexiDB::QuerySchema querySchema(*tableSchema);
    KexiDB::Cursor* cursor = gConnection->executeQuery(querySchema);

    // XML Preamble and Content Type
    writer->getResponse().setContentType("text/xml");
    writer->write("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    writer->write("<table>");

    // Metadata
    KexiWebForms::Model::Database db;
    QMap< QPair<QString, QString>, QPair<QString, KexiDB::Field::Type> > data(db.getSchema(requestedTable));
    QList< QPair<QString, QString> > dataKeys(data.keys());
    typedef QPair<QString, QString> QCaptionNamePair;

    writer->write("\t<metadata>\n");
    foreach(const QCaptionNamePair& captionNamePair, data.keys()) {
        writer->write("\t\t<field>\n");

        QPair<QString, KexiDB::Field::Type> valueTypePair(data[captionNamePair]);
        writer->write("\t\t\t<name>");
        writer->write(captionNamePair.second.toUtf8().constData());
        writer->write("</name>\n");
        writer->write("\t\t\t<caption>");
        writer->write(captionNamePair.first.toUtf8().constData());
        writer->write("</caption>\n");
        writer->write("\t\t\t<type>");
        if (valueTypePair.second == KexiDB::Field::BLOB) {
            writer->write("blob");
        } else if (valueTypePair.second == KexiDB::Field::Boolean) {
            writer->write("boolean");
        }
        writer->write("</type>\n");

        writer->write("\t\t</field>\n");
    }
    writer->write("\t</metadata>\n\n");

    // Data
    writer->write("\t<data>\n");
    if (!cursor) {
        //setValue("ERROR", "Unable to execute the query (" __FILE__ ")");
    } else if (tableSchema->primaryKey()->fields()->isEmpty()) {
        //setValue("ERROR", "This table has no primary key!");
    } else {
        KexiDB::Field* primaryKey = tableSchema->primaryKey()->field(0);
        while (cursor->moveNext()) {
            QString pkeyVal(cursor->value(tableSchema->indexOf(primaryKey)).toString());
            writer->write("\t\t<record>\n");
            for (uint i = 0; i < cursor->fieldCount(); i++) {
                KexiDB::Field* field = querySchema.field(i);
                const KexiDB::Field::Type type = field->type();

                writer->write("\t\t\t<");
                writer->write(field->name().toUtf8().constData());
                writer->write(">");
                if (type == KexiDB::Field::BLOB) {
                    /*valueString = QString("<img src=\"/blob/%1/%2/%3/%4\" alt=\"Image\"/>")
                        .arg(requestedTable).arg(field->name()).arg(primaryKey->name())
                        .arg(pkeyVal);*/
                } else {
                    writer->write(cursor->value(i).toString().toUtf8().constData());
                }
                writer->write("</");
                writer->write(field->name().toUtf8().constData());
                writer->write(">\n");
            }
            writer->write("\t\t</record>\n");
        }
        gConnection->deleteCursor(cursor);
    }
    writer->write("\t</data>");

    writer->write("</table>");

}

}
}
