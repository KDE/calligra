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

#include <pion/net/HTTPResponseWriter.hpp>

#include <google/template.h>

#include <kexidb/tableschema.h>

#include "model/DataProvider.h"
#include "model/Database.h"

#include "auth/Authenticator.h"
#include "auth/User.h"
#include "auth/Permission.h"

#include "TemplateProvider.h"

#include "Create.h"

using namespace pion::net;

namespace KexiWebForms {
namespace View {
    
    void Create::view(const QHash<QString, QString>& d, pion::net::HTTPResponseWriterPtr writer) {

        /*PionUserPtr userPtr(request->getUser());
        Auth::User u = Auth::Authenticator::getInstance()->authenticate(userPtr);

        if (u.can(Auth::CREATE)) {*/
            m_dict = initTemplate("create.tpl");

            /* Retrieve the requested table name */
            QString requestedTable(d["uri-table"]);
            setValue("TABLENAME", requestedTable);

            KexiWebForms::Model::Database db;
            KexiDB::TableSchema* tableSchema = db.tableSchema(requestedTable);


            /* Build the form */
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
                
                if (db.updateRow(requestedTable, data, true)) {
                    m_dict->ShowSection("SUCCESS");
                    setValue("MESSAGE", "Row added successfully");
                } else {
                    m_dict->ShowSection("ERROR");
                    setValue("MESSAGE", gConnection->errorMsg());
                }
            }

            QString formData;
            QStringList fieldsList;
            
            QMap< QPair<QString, QString>, QPair<QString, KexiDB::Field::Type> > data(db.getSchema(requestedTable));
            QList< QPair<QString, QString> > dataKeys(data.keys());

            // WORK AROUND
            typedef QPair<QString, QString> QCaptionNamePair;
            
            // FIXME: Regression, no icons, this way
            foreach(const QCaptionNamePair& captionNamePair, data.keys()) {
                formData.append("\t<tr>\n");
                QPair<QString, KexiDB::Field::Type> valueTypePair(data[captionNamePair]);
                formData.append("\t\t<td>").append(captionNamePair.first).append("</td>\n");
                if (valueTypePair.second == KexiDB::Field::LongText) {
                    formData.append(QString("\t\t<td><textarea name=\"%1\"></textarea></td>\n").arg(captionNamePair.second));
                } else {
                    formData.append(QString("\t\t<td><input type=\"text\" name=\"%1\" value=\"%2\"/></td>\n")
                                    .arg(captionNamePair.second).arg(valueTypePair.first));
                }
                formData.append("\t</tr>\n");
                fieldsList << captionNamePair.second;
            }

            setValue("TABLEFIELDS", fieldsList.join("|:|"));
            setValue("FORMDATA", formData);

            renderTemplate(m_dict, writer);
            delete m_dict;
            /*} else {
            writer->write("Not Authorized");
            writer->send();
            }*/
    }
}
}
