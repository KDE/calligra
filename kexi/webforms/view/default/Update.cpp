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

#include <QUrl>
#include <QHash>
#include <QList>
#include <QString>
#include <QStringList>

#include <KDebug>

#include <kexidb/tableschema.h>

#include <pion/net/HTTPResponseWriter.hpp>

#include "../../model/DataProvider.h"
#include "../../model/Database.h"
#include "../../model/Cache.h"

#include "TemplateProvider.h"

#include "Update.h"

namespace KexiWebForms {
namespace View {
    
    void Update::view(const QHash<QString, QString>& d, pion::net::HTTPResponseWriterPtr writer) {
        m_dict = initTemplate("update.tpl");
        
        QString requestedTable(d["uri-table"]);
        QString pkeyName(d["uri-pkey"]);
        QString pkeyValue(d["uri-pval"]);
        uint pkeyValueUInt = pkeyValue.toUInt();
        uint current = 0;
        
        setValue("TABLENAME", requestedTable);
        setValue("PKEY_NAME", pkeyName);
        setValue("PKEY_VALUE", pkeyValue);
        
        // Initialize needed objects
        KexiWebForms::Model::Cache* cache = KexiWebForms::Model::Cache::getInstance();
        QPair<KexiDB::TableSchema, QList<QVariant> > pair(
            KexiWebForms::Model::Database::getSchema(requestedTable, pkeyName, pkeyValueUInt));
        
        KexiDB::TableSchema tableSchema(pair.first);
        
        cache->updateCachedPkeys(requestedTable);
        QList<uint> cachedPkeys(cache->getCachedPkeys(requestedTable));
        current = cache->getCurrentCachePosition(requestedTable, pkeyValueUInt);
        
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
                kDebug() << "UPDATING: " << field << "=" << d[field] << endl;
                if (currentField)
                    data[field] = QVariant(d[field]);
            }
            
            if (KexiWebForms::Model::Database::updateRow(requestedTable, data, false, pkeyValue.toInt())) {
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

        for (uint i = 0; i < tableSchema.fieldCount(); i++) {
            KexiDB::Field* field = tableSchema.field(i);
            
            formData.append("\t<tr>\n");
            formData.append(QString("\t\t<td>%1</td>\n").arg(field->captionOrName()));
            if (field->type() == KexiDB::Field::LongText) {
                formData.append(QString("\t\t<td><textarea name=\"%1\"></textarea></td>\n").arg(field->name()));
            } else if (field->type() == KexiDB::Field::BLOB) {
                formData.append(QString("<td><img src=\"/blob/%1/%2/%3/%4\" alt=\"Image\"/><br/>"
                                        "<!-- <input type=\"file\" name=\"%2\"/> --></td>")
                                .arg(requestedTable).arg(field->name()).arg(pkeyName).arg(pkeyValue));
            } else {
                formData.append(QString("\t\t<td><input type=\"text\" name=\"%1\" value=\"%2\"/></td>\n")
                                .arg(field->name()).arg(pair.second.at(i).toString()));
            }
            formData.append("\t</tr>\n");
            formFieldsList << field->name();
        }
        
        setValue("TABLEFIELDS", formFieldsList.join("|:|"));
        setValue("FORMDATA", formData);
            
        
        renderTemplate(m_dict, writer);
        delete m_dict;   
    }
    
}
}
