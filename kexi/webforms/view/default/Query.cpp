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
#include <QVector>
#include <QTextDocument>

#include <KDebug>

#include <kexidb/queryschema.h>

#include <pion/net/HTTPResponseWriter.hpp>

#include "../../model/Database.h"
#include "TemplateProvider.h"

#include "Query.h"

using namespace pion::net;

namespace KexiWebForms {
namespace View {

void Query::view(const QHash<QString, QString>& d, pion::net::HTTPResponseWriterPtr writer) {
    QString requestedQuery(d["kwebforms__query"]);

    QPair< KexiDB::TableSchema, QMap<uint, QList<QString> > > pair =  KexiWebForms::Model::Database::readQuery(requestedQuery);

    QString queryData;
    uint totalRecords = pair.second.count();

    // Query header
    queryData.append("<tr>\t<th scope=\"col\">Record</th>\n");
    foreach(const KexiDB::Field* f, *pair.first.fields()) {
        queryData.append(QString("\t<th scope=\"col\">%1</th>\n").arg(f->captionOrName()));
    }
    queryData.append("</tr>\n");

    // Query contents
    foreach(const uint record, pair.second.keys()) {
        queryData.append("<tr>");
        queryData.append(QString("<td>%1 of %2</td>").arg(record).arg(totalRecords));

        foreach(const QString& value, pair.second[record]) {
            kDebug() << "Number of fields: " << pair.second[record].count() << endl;
            queryData.append(QString("\t<td>%1</td>\n").arg(value));
        }
        queryData.append("</tr>\n");
    }

    setValue("QUERYDATA", queryData);
    setValue("QUERYNAME", requestedQuery);
    renderTemplate(m_dict, writer);
}

}
}
