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
//#include "../../model/DataProvider.h"
#include "TemplateProvider.h"

#include "Query.h"

using namespace pion::net;

namespace KexiWebForms {
namespace View {
    
    void Query::view(const QHash<QString, QString>& d, pion::net::HTTPResponseWriterPtr writer) {
        QString requestedQuery(d["kwebforms__query"]);

        QPair< KexiDB::QuerySchema, QMap<uint, QList<QString> > > pair = KexiWebForms::Model::Database::readQuery(requestedQuery);

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
                queryData.append(QString("\t<td>%1</td>\n").arg(value));
            }
            queryData.append("</tr>\n");
        }

        setValue("QUERYDATA", queryData);
        setValue("QUERYNAME", requestedQuery);
        renderTemplate(m_dict, writer);
        
        /*QString queryData;
        bool ok = true;
        KexiDB::QuerySchema* querySchema = KexiWebForms::Model::gConnection->querySchema(requestedQuery);
        if (!querySchema) {
            setValue("ERROR", QString("Could not find query \"%1\"").arg(requestedQuery));
            ok = false;
        }
        KexiDB::TableSchema* tableSchema = 0;
        if (ok) {
            tableSchema = querySchema->masterTable();
            if (!tableSchema) {
                setValue("ERROR", QString("Could not find master table for query \"%1\"").arg(requestedQuery));
                ok = false;
            }
        }
        if (ok) {
            //! @todo more checks
            KexiDB::Cursor* cursor = KexiWebForms::Model::gConnection->executeQuery(*querySchema);
            
            /* awful 
            int recordsTotal = 0;
            while (cursor->moveNext())
                recordsTotal++;
            KexiWebForms::Model::gConnection->deleteCursor(cursor);
            
            cursor = KexiWebForms::Model::gConnection->executeQuery(*querySchema);
            
            setValue("QUERYNAME", querySchema->caption());
            
            
             * @note: the code is very very similar to the one available in Read.cpp
             * do some refactoring and avoid copy&paste programming
             * Situation should become better when I switch to the new OO server
             * backend
             
            if (!cursor) {
                setValue("ERROR", "Unable to execute the query (" __FILE__ ")");
            } else if (tableSchema->primaryKey()->fields()->isEmpty()) {
                setValue("ERROR", "This table has no primary key!");
            } else {
                kDebug() << "Showing query results..." << endl;
                //KexiDB::Field* primaryKey = tableSchema->primaryKey()->field(0);
                KexiDB::QueryColumnInfo::Vector expandedFields = querySchema->fieldsExpanded();
                
                
                // Create labels with field name
                queryData.append("<tr>\t<th scope=\"col\">Record</th>\n");
                for (uint i = 0; i < uint( expandedFields.size() ); i++) {
                    queryData.append(QString("\t<th scope=\"col\">%1</th>\n")
                                     .arg(expandedFields.at(i)->field->captionOrName()));
                }
                queryData.append("</tr>\n");
                
                
                int currentRecord = 0;
                QString totalRecords(QVariant(recordsTotal).toString());
                // Create labels with fields data
                while (cursor->moveNext()) {
                    currentRecord++;
                    
                    queryData.append(QString("<tr><td>%1 of %2</td>").arg(QVariant(currentRecord).toString())
                                     .arg(totalRecords));
                    
                    for (uint i = 0; i < uint( expandedFields.size() ); i++) {
                        queryData.append("<td>");
                        
                        //
                        // Use Kexi functions to retrieve and represent the Value
                        //! @todo use Kexi the same functions for rendering values as Kexi table and form view
                        //
                        //! @note this will make it crash, commented out for now
                        KexiDB::Field* field = expandedFields.at(i)->field;
                        const KexiDB::Field::Type type = field->type();
                        QString valueString;
                        if (type == KexiDB::Field::BLOB) {
                            //! @todo decode image and display it if possible
                            valueString = "(Object)";
                        } else if (field->isTextType()) {
                            valueString = Qt::escape( cursor->value(i).toString() );
                        } else {
                            valueString = Qt::escape( cursor->value(i).toString() );
                        }
                        queryData.append(valueString);
                        queryData.append("</td>");
                    }
                    // End row
                    queryData.append("</tr>\n");
                    
                    KexiWebForms::Model::gConnection->deleteCursor(cursor);
                    setValue("QUERYDATA", queryData);
                }
                
            }
        }
        renderTemplate(m_dict, writer);*/
    }
    
}
}
