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
#include <QVector>

#include <KDebug>

#include <pion/net/HTTPResponseWriter.hpp>

#include <kexidb/utils.h>
#include <kexidb/queryschema.h>
#include <kexidb/cursor.h>

#include <google/template.h>

#include "DataProvider.h"
#include "TemplateProvider.h"

#include "QueryService.h"

using namespace pion::net;

namespace KexiWebForms {
    
    void QueryService::operator()(pion::net::HTTPRequestPtr& request, pion::net::TCPConnectionPtr& tcp_conn) {
        HTTPResponseWriterPtr writer(HTTPResponseWriter::create(tcp_conn, *request,
                    boost::bind(&TCPConnection::finish, tcp_conn)));


        QString requestedQuery(QString(request->getOriginalResource().c_str()).split('/').at(2));

        QString queryData;
        bool ok = true;
        KexiDB::QuerySchema* querySchema = gConnection->querySchema(requestedQuery);
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
            KexiDB::Cursor* cursor = gConnection->executeQuery(*querySchema);

            /* awful */
            int recordsTotal = 0;
            while (cursor->moveNext())
                recordsTotal++;
            gConnection->deleteCursor(cursor);

            cursor = gConnection->executeQuery(*querySchema);

            setValue("QUERYNAME", querySchema->caption());

            /**
             * @note: the code is very very similar to the one available in Read.cpp
             * do some refactoring and avoid copy&paste programming
             * Situation should become better when I switch to the new OO server
             * backend
             */
            if (!cursor) {
                setValue("ERROR", "Unable to execute the query (" __FILE__ ")");
            } else if (tableSchema->primaryKey()->fields()->isEmpty()) {
                setValue("ERROR", "This table has no primary key!");
            } else {
                kDebug() << "Showing query results..." << endl;
                KexiDB::Field* primaryKey = tableSchema->primaryKey()->field(0);
                KexiDB::QueryColumnInfo::Vector expandedFields = querySchema->fieldsExpanded();
              
              
                // Create labels with field name
                queryData.append("<tr>");
                queryData.append("\t<th scope=\"col\">Record</th>\n");
                for (uint i = 0; i < uint( expandedFields.size() ); i++) {
                    queryData.append("\t<th scope=\"col\">");
                    queryData.append(expandedFields.at(i)->field->captionOrName());
                    queryData.append("</th>\n");
                }
                queryData.append("</tr>\n");


                int currentRecord = 0;
                QString totalRecords(QVariant(recordsTotal).toString());
                // Create labels with fields data
                while (cursor->moveNext()) {
                    currentRecord++;
                    
                    queryData.append("<tr>");
                    queryData.append("<td>").append(QVariant(currentRecord).toString()).append(" of ");
                    queryData.append(totalRecords).append("</td>");
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
                    // Toolbox
                    QString pkeyVal(cursor->value(tableSchema->indexOf(primaryKey)).toString());
                    // Edit -- currently disabled
                    /*
                      queryData.append("<td><a href=\"/update/").append(requestedTable).append("/");
                      queryData.append(primaryKey->name()).append("/");
                      queryData.append(pkeyVal).append("\"><img src=\"/toolbox/draw-freehand.png\" alt=\"Edit\"/></a></td>");
                    */
                    // Delete -- currently disabled
                    /*
                      queryData.append("<td><a href=\"/delete/").append(requestedTable).append("/");
                      queryData.append(primaryKey->name()).append("/");
                      queryData.append(pkeyVal).append("\"><img src=\"/toolbox/draw-eraser.png\" alt=\"Delete\"/></a></td>");
                    */
                    // End row
                    queryData.append("</tr>\n");
                    
                    gConnection->deleteCursor(cursor);
                    setValue("QUERYDATA", queryData);
                }
                
            }
        }
        renderTemplate(m_dict, writer);
    }
    
}
