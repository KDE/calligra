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

#include <QMap>
#include <QPair>
#include <QHash>
#include <QString>

#include <pion/net/HTTPResponseWriter.hpp>

#include <KDebug>

#include <kexidb/tableschema.h>

#include "../../model/Database.h"
#include "../../model/DataProvider.h"

#include "TemplateProvider.h"

#include "Read.h"

namespace KexiWebForms {
namespace View {

void Read::view(const QHash<QString, QString>& d, pion::net::HTTPResponseWriterPtr writer) {
    QString requestedTable(d["kwebforms__table"]);

    QPair< KexiDB::TableSchema, QMap<uint, QList<QString> > > pair = KexiWebForms::Model::Database::readTable(requestedTable);

    QString tableData;
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
    setValue("TABLENAME", requestedTable);
    renderTemplate(m_dict, writer);
}

}
}
