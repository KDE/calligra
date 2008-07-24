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
#include <QHash>
#include <QList>
#include <QUrl>

#include <KDebug>

#include <pion/net/HTTPResponseWriter.hpp>

#include <kexidb/tableschema.h>

#include <google/template.h>

#include "model/DataProvider.h"
#include "model/Database.h"

#include "auth/Authenticator.h"
#include "auth/User.h"
#include "auth/Permission.h"

#include "TemplateProvider.h"

#include "Update.h"

namespace KexiWebForms {
namespace View {
    
    void Update::view(const QHash<QString, QString>& d, pion::net::HTTPResponseWriterPtr writer) {

        /*PionUserPtr userPtr(request->getUser());
        Auth::User u = Auth::Authenticator::getInstance()->authenticate(userPtr);

        if (u.can(Auth::UPDATE)) {*/
            m_dict = initTemplate("update.tpl");
            
            QString requestedTable(d["uri-table"]);
            QString pkeyName(d["uri-pkey"]);
            QString pkeyValue(d["uri-pval"]);
            uint pkeyValueUInt = pkeyValue.toUInt();
            uint current = 0;
            
            setValue("TABLENAME", requestedTable);
            setValue("PKEY_NAME", pkeyName);
            setValue("PKEY_VALUE", pkeyValue);

            // Initialize needed Objects
            KexiWebForms::Model::Database db;
            KexiDB::TableSchema tableSchema(*db.tableSchema(requestedTable));
            db.updateCachedPkeys(requestedTable);

            // Retrieve current position in cache
            QList<uint> cachedPkeys(db.getCachedPkeys(requestedTable));
            for (int i = 0; i < cachedPkeys.size(); i++) {
                if (cachedPkeys.at(i) == pkeyValueUInt)
                    current = i;
            }

            // Compute new primary key values for first, last, previous and next record
            if (current < uint( cachedPkeys.size()-1 )) {
                m_dict->ShowSection("NEXT_ENABLED");
                m_dict->SetValue("NEXT", QVariant(cachedPkeys.at(current+1)).toString().toUtf8().constData());
                m_dict->ShowSection("LAST_ENABLED");
                m_dict->SetValue("LAST", QVariant(cachedPkeys.at(cachedPkeys.size()-1)).toString().toUtf8().constData());
            } else {
                m_dict->ShowSection("NEXT_DISABLED");
                m_dict->ShowSection("LAST_DISABLED");
            }

            if (current > 0) {
                m_dict->ShowSection("PREV_ENABLED");
                m_dict->SetValue("PREV", QVariant(cachedPkeys.at(current-1)).toString().toUtf8().constData());
                m_dict->ShowSection("FIRST_ENABLED");
                m_dict->SetValue("FIRST", QVariant(cachedPkeys.at(0)).toString().toUtf8().constData());
            } else {
                m_dict->ShowSection("PREV_DISABLED");
                m_dict->ShowSection("FIRST_DISABLED");
            }

            
            if (d["dataSent"] == "true") {
                    

                QStringList fieldsList(d["tableFields"].split("|:|"));

                QHash<QString, QVariant> data;
                foreach(const QString& field, fieldsList) {
                    KexiDB::Field* currentField = tableSchema.field(field);
                    if (currentField)
                        data[field] = QVariant(d[field]);
                }

                if (db.updateRow(requestedTable, data, false, pkeyValue.toInt())) {
                    m_dict->ShowSection("SUCCESS");
                    setValue("MESSAGE", "Updated");
                } else {
                    m_dict->ShowSection("ERROR");
                    setValue("MESSAGE", gConnection->errorMsg());
                }
            }

            kDebug() << "Showing fields" << endl;

            m_dict->ShowSection("FORM");

                
            QString formData;
            QStringList formFieldsList;

            QMap< QPair<QString, QString>, QPair<QString, KexiDB::Field::Type> > data(db.getSchema(requestedTable,
                                                                                                   pkeyName, pkeyValue.toInt()));
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
                } else if (valueTypePair.second == KexiDB::Field::BLOB) {
                    formData.append(QString("<td><img src=\"/blob/%1/%2/%3/%4\" alt=\"Image\"/></td>")
                                    .arg(requestedTable).arg(captionNamePair.second).arg(pkeyName)
                                    .arg(pkeyValue));
                } else {
                    formData.append(QString("\t\t<td><input type=\"text\" name=\"%1\" value=\"%2\"/></td>\n")
                                    .arg(captionNamePair.second).arg(valueTypePair.first));
                }
                formData.append("\t</tr>\n");
                formFieldsList << captionNamePair.second;
            }
                
            setValue("TABLEFIELDS", formFieldsList.join("|:|"));
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
