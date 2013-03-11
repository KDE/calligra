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

#include <QMap>
#include <QPair>
#include <QHash>
#include <QString>

#include <KDebug>

#include <db/tableschema.h>

#include <google/template.h>

#include <pion/net/HTTPResponseWriter.hpp>

#include "model/DataProvider.h"
#include "model/Database.h"

#include "TemplateProvider.h"

#include "Create.h"

namespace KexiWebForms {
namespace View {

void Create::view(const QHash<QString, QString>& d, pion::net::HTTPResponseWriterPtr writer) {
    m_dict = initTemplate("create.tpl");

    /* Retrieve the requested table name */
    QString requestedTable(d["kwebforms__table"]);
    setValue("TABLENAME", requestedTable);

    KexiDB::TableSchema* tableSchema = KexiWebForms::Model::Database::getSchema(requestedTable);


    /* send form data */
    if (d["dataSent"] == "true") {
        QString tableFields(d["tableFields"]);
        QStringList fieldsList(QUrl::fromPercentEncoding(tableFields.toUtf8()).split("|:|"));
        kDebug() << "Fields: " << fieldsList;

        QHash<QString, QVariant> data;
        foreach(const QString& field, fieldsList) {
            KexiDB::Field* currentField = tableSchema->field(field);
            if (currentField)
                data[field] = QVariant(d[field]);
        }

        if (KexiWebForms::Model::Database::updateRow(requestedTable, data, true)) {
            m_dict->ShowSection("SUCCESS");
            setValue("MESSAGE", "Row added successfully");
        } else {
            m_dict->ShowSection("ERROR");
            setValue("MESSAGE", KexiWebForms::Model::gConnection->errorMsg());
        }
    }

    QString formData;
    QStringList fieldsList;

    foreach(const KexiDB::Field* f, *tableSchema->fields()) {
        formData.append("\t<tr>\n");
        formData.append(QString("\t\t<td>%1</td>\n").arg(f->captionOrName()));
        if (f->type() == KexiDB::Field::LongText) {
            formData.append(QString("\t\t<td><textarea name=\"%1\">%2</textarea></td>\n")
                            .arg(f->name()).arg(f->defaultValue().toString()));
        } else if (f->type() == KexiDB::Field::Boolean) {
            // TODO: Create a checkbox instead
            formData.append(QString("\t\t<td><input type=\"text\" name=\"%1\" value=\"%2\"/></td>\n")
                            .arg(f->name()).arg(f->defaultValue().toString()));
        } else {
            formData.append(QString("\t\t<td><input type=\"text\" name=\"%1\" value=\"%2\"/></td>\n")
                            .arg(f->name()).arg(f->defaultValue().toString()));
        }
        // Field icons
        formData.append("\t\t<td>\n");
        if (f->isPrimaryKey())
            formData.append("<img src=\"/f/toolbox/primary-key.png\" alt=\"Primary Key\"/>");

        if (f->isNotEmpty() && f->isAutoIncrement()) {
            formData.append("<img src=\"/f/toolbox/auto-increment.png\" alt=\"Auto Increment\"/>");
        } else if (f->isNotNull()) {
            formData.append("<img src=\"/f/toolbox/emblem-required.png\" alt=\"Required\"/>");
        }
        formData.append("\n</td>\n\t</tr>\n");
        fieldsList << f->name();
    }

    setValue("TABLEFIELDS", fieldsList.join("|:|"));
    setValue("FORMDATA", formData);

    renderTemplate(m_dict, writer);
    delete m_dict;
}
}
}
